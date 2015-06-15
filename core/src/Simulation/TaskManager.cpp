#include "TaskManager.hpp"

#include "Foundation/Log.hpp"
#include "Foundation/Profiler.hpp"

using namespace crimild;

// provides at least one background task
class BackgroundDummyTask : public Task {
public:
	BackgroundDummyTask( void ) : Task( Task::RepeatMode::REPEAT, Task::ThreadMode::BACKGROUND, Task::SyncMode::NONE ) { }
	virtual ~BackgroundDummyTask( void ) { }

	virtual void run( void ) { }
};

TaskManager::TaskManager( unsigned int numThreads )
	: _numThreads( numThreads )
{
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messages::TerminateTasks, TaskManager, onTerminate );
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messages::ExecuteTask, TaskManager, onExecuteTask );
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messages::ExecuteTaskGroup, TaskManager, onExecuteTaskGroup );
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messages::TaskCompleted, TaskManager, onTaskCompleted );
    CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( messages::TaskGroupCompleted, TaskManager, onTaskGroupCompleted );

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

	addTask( crimild::alloc< BackgroundDummyTask >() );

	for ( unsigned int i = 0; i < _numThreads; i++ ) {
		_threads.push_back( std::thread( std::bind( &TaskManager::worker, this ) ) );
	}

	_running = true;
}

void TaskManager::step( void )
{
	// TODO: not sure if this is the correct organization

}

void TaskManager::stop( void )
{
	_running = false;
}

void TaskManager::run( void )
{
	start();

	while ( _running ) {
		if ( !_tasks[ _readList ].empty() ) {
			auto task = _tasks[ _readList ].waitPop();
			executeTask( task );
		}
		else {
			synchronize();
			std::swap( _readList, _writeList );
		}

		std::this_thread::yield();
	}
}

void TaskManager::worker( void )
{
	while ( _running ) {
		auto task = _backgroundTasks.waitPop();
		executeTask( task );

		if ( task->getSyncMode() == Task::SyncMode::FRAME ) {
			ScopedLock lock( _syncMutex );
            if ( _numTasksToWaitFor > 0 ) --_numTasksToWaitFor;
			lock.unlock();
			_syncCondition.notify_one();
		}

		std::this_thread::yield();
	}
}

void TaskManager::addTask( TaskPtr const &task )
{
	if ( task->getThreadMode() == Task::ThreadMode::BACKGROUND ) {
		if ( task->getSyncMode() == Task::SyncMode::FRAME ) {
			_syncTasks.push( task );
		}
		else {
			_backgroundTasks.push( task );
		}
	}
	else {
		_tasks[ _writeList ].push( task );
	}
}

void TaskManager::executeTask( TaskPtr const &task )
{
	broadcastMessage( messages::TaskStarted { task } );

	task->run();
	
	broadcastMessage( messages::TaskCompleted { task } );
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

void TaskManager::onTerminate( messages::TerminateTasks const & )
{
	stop();
}

void TaskManager::onExecuteTask( messages::ExecuteTask const &message )
{
    addTask( message.task );
}

void TaskManager::onExecuteTaskGroup( messages::ExecuteTaskGroup const &message )
{
    auto self = this;
    _taskGroups.add( message.tasks );

    message.tasks->getTasks().each( [self]( TaskPtr const &task ) {
        self->addTask( task );
    });
}

void TaskManager::onTaskCompleted( messages::TaskCompleted const &message )
{
    message.task->notifyResult();
    if ( message.task->getRepeatMode() == Task::RepeatMode::REPEAT ) {
        addTask( message.task );
    }
}

void TaskManager::onTaskGroupCompleted( messages::TaskGroupCompleted const &message )
{
    _taskGroups.remove( message.taskGroup );
}

