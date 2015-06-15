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

#include "Task.hpp"
#include "TaskManager.hpp"
#include "Simulation.hpp"

using namespace crimild;

Task::Task( int priority )
	: _priority( priority )
{

}

Task::Task( void )
	: Task( Task::RepeatMode::ONCE, Task::ThreadMode::FOREGROUND, Task::SyncMode::NONE )
{

}

Task::Task( Task::RepeatMode repeatMode, Task::ThreadMode threadMode, Task::SyncMode syncMode )
	: _repeatMode( repeatMode ),
	  _threadMode( threadMode ),
	  _syncMode( syncMode )
{

}

Task::~Task( void )
{

}

void Task::execute( void )
{
    Simulation::getInstance()->addTask( getSharedPointer( this ) );
}

void Task::waitResult( void )
{
    /*
    Task::ScopedLock _lock( _mutex );
    _conditionVariable.wait( _lock );
     */
}

void Task::notifyResult( void )
{
    /*
    Task::ScopedLock _lock( _mutex );
    
    // Manual unlocking is done before notifying, to avoid waking up
    // the waiting thread only to block again (see notify_one for details)
    _lock.unlock();

    _conditionVariable.notify_one();
     */
}

