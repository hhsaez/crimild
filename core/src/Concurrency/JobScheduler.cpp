#include "JobScheduler.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::concurrency;

JobScheduler::JobScheduler( void )
{

}

JobScheduler::~JobScheduler( void )
{

}

bool JobScheduler::start( int numWorkers )
{
	_state = JobScheduler::State::INITIALIZING;

	// initialize the main thread as another worker
    initWorker( true );

	if ( numWorkers < 0 ) {
		numWorkers = std::thread::hardware_concurrency();
	}

	Log::Info << "Initializing job scheduler with " << numWorkers << " workers" << Log::End;

	for ( int i = 0; i < numWorkers; i++ ) {
		_workers.push_back( std::thread( std::bind( &JobScheduler::worker, this ) ) );
	}

	_state = JobScheduler::State::RUNNING;

	return true;
}

void JobScheduler::stop( void )
{
	_state = JobScheduler::State::STOPPING;

	for ( auto &w : _workers ) {
		if ( w.joinable() ) {
			w.join();
		}
	}

    size_t jobCount = 0;
    Log::Debug << "Stats: ";
    for ( auto &it : _workerStats ) {
        Log::Debug << "\n\tWorker " << it.first << " executed " << it.second.jobCount << " jobs";
        jobCount += it.second.jobCount;
    }
    Log::Debug << "\n\tTotal jobs: " << jobCount;
    Log::Debug << Log::End;
    
	_workers.clear();
    _workerJobQueues.clear();

	_state = JobScheduler::State::STOPPED;
}

void JobScheduler::worker( void )
{
    initWorker();

	while ( getState() == JobScheduler::State::INITIALIZING ) {
		// wait for startup to complete
		yield();
	}
	
	while ( getState() == JobScheduler::State::RUNNING ) {
		executeNextJob();
	}
}

void JobScheduler::initWorker( bool mainWorker )
{
    std::unique_lock< std::mutex > lock( _mutex );
    
    if ( mainWorker ) {
        _mainWorkerId = getWorkerId();
    }

    _workerStats[ getWorkerId() ].jobCount = 0;
	_workerJobQueues[ getWorkerId() ] = crimild::alloc< WorkerJobQueue >();
}	

JobScheduler::WorkerId JobScheduler::getWorkerId( void ) const
{
	return std::this_thread::get_id();
}

JobScheduler::WorkerJobQueue *JobScheduler::getWorkerJobQueue( void )
{
	return crimild::get_ptr( _workerJobQueues[ getWorkerId() ] );
}

JobScheduler::WorkerJobQueue *JobScheduler::getRandomJobQueue( void )
{
	if ( _workerJobQueues.size() == 0 ) {
		return nullptr;
	}

	for ( auto &it : _workerJobQueues ) {
		if ( it.second != nullptr && !it.second->empty() ) {
			return crimild::get_ptr( it.second );
		}
	}

	return nullptr;
}

void JobScheduler::schedule( SharedPointer< Job > const &job )
{
	auto queue = getWorkerJobQueue();
	queue->push( job );
}

SharedPointer< Job > JobScheduler::getJob( void )
{
	auto queue = getWorkerJobQueue();

	if ( queue != nullptr && !queue->empty() ) {
		auto job = queue->pop();
		if ( job != nullptr ) {
			return job;
		}
	}

	auto stealQueue = getRandomJobQueue();
	if ( stealQueue == nullptr || stealQueue == queue ) {
		// do not steal from ourselves
		return nullptr;
	}

	if ( !stealQueue->empty() ) {
		return stealQueue->steal();
	}

	return nullptr;
}

bool JobScheduler::executeNextJob( void )
{
	auto job = getJob();
	if ( job != nullptr ) {
		execute( job );
		_workerStats[ getWorkerId() ].jobCount++;
		return true;
	}

	yield();
	return false;
}

void JobScheduler::execute( SharedPointer< Job > const &job )
{
	job->execute();
}

void JobScheduler::wait( SharedPointer< Job > const &job )
{
	while( !job->isCompleted() ) {
		executeNextJob();
	}
}

void JobScheduler::yield( void )
{
    if ( isMainWorker() ) {
        // main worker does not yield
        return;
    }

	std::this_thread::yield();
}
