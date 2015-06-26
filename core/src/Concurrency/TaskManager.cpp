#include "Concurrency/TaskManager.hpp"
#include "Concurrency/Async.hpp"

#include "Foundation/Log.hpp"
#include "Foundation/Profiler.hpp"

using namespace crimild;

TaskManager::TaskManager( unsigned int numThreads )
	: _numThreads( numThreads )
{
	if ( _numThreads == 0 ) {
		_numThreads = std::thread::hardware_concurrency() + 1;
	}

	_writeList = 0;
	_readList = 1;
	_numTasksToWaitFor = 0;
}

TaskManager::~TaskManager( void )
{
	for ( auto &t : _threads ) {
		t.join();
	}
}

void TaskManager::start( void )
{
	Log::Debug << "Starting task manager" << Log::End;

	Log::Debug << "Spawning " << _numThreads << " threads" << Log::End;

    // provides at least one background task
    dummyTask();
    
	for ( unsigned int i = 0; i < _numThreads; i++ ) {
		_threads.push_back( std::thread( std::bind( &TaskManager::worker, this ) ) );
	}

	_running = true;
}

void TaskManager::pollMainTasks( void )
{
    while ( _running && !_tasks[ _readList ].empty() ) {
        auto task = _tasks[ _readList ].waitPop();
        executeTask( task );
        std::this_thread::yield();
    }
    
    if ( _running ) {
        synchronize();
        std::swap( _readList, _writeList );
    }
}

void TaskManager::stop( void )
{
	_running = false;
}

void TaskManager::worker( void )
{
	while ( _running ) {
		auto task = _backgroundTasks.waitPop();
		executeTask( task );

		if ( task.getSyncFrame() ) {
			ScopedLock lock( _syncMutex );
            if ( _numTasksToWaitFor > 0 ) --_numTasksToWaitFor;
			lock.unlock();
			_syncCondition.notify_one();
		}

		std::this_thread::yield();
	}
}

void TaskManager::addTask( Task const &task )
{
    if ( task.isThreadSafe() ) {
        if ( task.getSyncFrame() ) {
            _syncTasks.push( task );
        }
        else {
            _backgroundTasks.push( task );
        }
    }
    else {
        _tasks[ _writeList ] .push( task );
    }
}

void TaskManager::executeTask( Task &task )
{
    task.execute();
}

void TaskManager::synchronize( void )
{
	ScopedLock lock( _syncMutex );

	while ( _numTasksToWaitFor > 0 ) {
		_syncCondition.wait( lock );
	}

	_numTasksToWaitFor = _syncTasks.size();

	while ( !_syncTasks.empty() ) {
		_backgroundTasks.push( _syncTasks.waitPop() );
	}
}

void TaskManager::dummyTask( void )
{
    crimild::async( std::bind( &TaskManager::dummyTask, this ) );
}

