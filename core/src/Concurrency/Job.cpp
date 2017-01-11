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

#include "Job.hpp"

using namespace crimild;
using namespace crimild::concurrency;

Job::Job( void )
	: _callback( nullptr ),
      _parent( nullptr ),
	  _childCount( 0 )
{

}

Job::~Job( void )
{

}

void Job::reset( void )
{
	_callback = nullptr;
	_parent = nullptr;
	_childCount = 0;
}

void Job::reset( JobCallback const &callback )
{
	_callback = callback;
	_parent = nullptr;
	_childCount = 1;
}

void Job::reset( JobPtr const &parent, JobCallback const &callback )
{
	_callback = callback;
    _parent = crimild::get_ptr( parent );
	_childCount = 1;

	if ( _parent != nullptr ) {
		_parent->increaseChildCount();
	}
}

void Job::increaseChildCount( void )
{
	++_childCount;
}

void Job::decreaseChildCount( void )
{
	if ( _childCount > 0 ) {
		--_childCount;
	}
}

void Job::attachContinuation( JobContinuationCallback const &callback )
{
	_continuations.push_back( callback );
}

void Job::execute( void )
{
	if ( _callback != nullptr ) {
		_callback();
	}

	finish();
}

void Job::finish( void )
{
	decreaseChildCount();
	if ( isCompleted() ) {
		if ( _parent != nullptr ) {
			_parent->finish();
		}
        _callback = nullptr;
	}
}

