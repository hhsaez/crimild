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

#include "AlphaState.hpp"

using namespace crimild;

SharedPointer< AlphaState > AlphaState::DISABLED( crimild::alloc< AlphaState >( false ) );
SharedPointer< AlphaState > AlphaState::ENABLED( crimild::alloc< AlphaState >( true ) );
SharedPointer< AlphaState > AlphaState::ENABLED_SRC_COLOR_ONLY( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::SRC_COLOR, AlphaState::DstBlendFunc::ZERO ) );
SharedPointer< AlphaState > AlphaState::ENABLED_ADDITIVE_BLEND( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::SRC_ALPHA, AlphaState::DstBlendFunc::ONE ) );
SharedPointer< AlphaState > AlphaState::ENABLED_MULTIPLY_BLEND( crimild::alloc< AlphaState >( true, AlphaState::SrcBlendFunc::DST_COLOR, AlphaState::DstBlendFunc::ZERO ) );

AlphaState::AlphaState( bool enabled, SrcBlendFunc srcBlendFunc, DstBlendFunc dstBlendFunc )
	: RenderState( enabled ),
	  _srcBlendFunc( srcBlendFunc ),
	  _dstBlendFunc( dstBlendFunc )
{

}

AlphaState::~AlphaState( void )
{

}

