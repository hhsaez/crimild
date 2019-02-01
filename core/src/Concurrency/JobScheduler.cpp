/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "JobScheduler.hpp"

#include "Foundation/Log.hpp"

using namespace crimild;
using namespace crimild::concurrency;

JobScheduler::JobScheduler( void )
	: _numWorkers( std::thread::hardware_concurrency() )
{

}

JobScheduler::~JobScheduler( void )
{

}

void JobScheduler::configure( int numWorkers )
{
    _numWorkers = numWorkers;
    if ( _numWorkers < 0 ) {
        _numWorkers = std::thread::hardware_concurrency();
    }
}

bool JobScheduler::start( void )
{
	_state = JobScheduler::State::INITIALIZING;

	// initialize the main thread as another worker
    initWorker( true );

    Log::info( CRIMILD_CURRENT_CLASS_NAME, "Initializing job scheduler with ", getNumWorkers(), " workers" );

	for ( int i = 0; i < getNumWorkers(); i++ ) {
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
    std::lock_guard< std::mutex > lock( _mutex );
    
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

void JobScheduler::schedule( JobPtr const &job )
{
    if ( job == nullptr ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot schedule null job" );
        return;
    }
    
    if ( getState() == JobScheduler::State::STOPPING || getState() == JobScheduler::State::STOPPED ) {
        Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot schedule new jobs since the scheduler is not running" );
        return;
    }

    if ( getNumWorkers() == 0 ) {
        // no workers. execute it
        execute( job );
        return;
    }
    
	auto queue = getWorkerJobQueue();
	queue->push( job );
}

JobPtr JobScheduler::getJob( void )
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

void JobScheduler::execute( JobPtr const &job )
{
	job->execute();
}

void JobScheduler::wait( JobPtr const &job )
{
	while( !job->isCompleted() ) {
		executeNextJob();
	}
}

void JobScheduler::yield( void )
{
//    if ( isMainWorker() ) {
//        // main worker does not yield
//        return;
//    }
//
	std::this_thread::yield();
}

void JobScheduler::delaySync( JobPtr const &job )
{
    _delayedSyncJobs.push_back( job );
}

void JobScheduler::delayAsync( JobPtr const &job )
{
    _delayedAsyncJobs.push_back( job );
}

void JobScheduler::executeDelayedJobs( void )
{
    // Schedule async jobs first since we don't need to wait for them
    // Flush lists afterwards

    _delayedAsyncJobs.each( [this]( JobPtr const &j ) {
        schedule( j );
    }, true );
    
    _delayedSyncJobs.each( [this]( JobPtr const &j ) {
        execute( j );
    }, true );
}

void JobScheduler::eachWorkerStat( std::function< void( WorkerId, const WorkerStat & ) > const &callback ) const
{
	for ( const auto &it : _workerStats ) {
		callback( it.first, it.second );
	}
}

void JobScheduler::clearWorkerStats( void )
{
	std::lock_guard< std::mutex > lock( _mutex );

	for ( auto &it : _workerStats ) {
		it.second.jobCount = 0;
	}
}

