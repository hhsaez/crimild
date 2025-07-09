/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holder nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "TextDecoder.hpp"

#include "Tags.hpp"

#include <Crimild_Foundation.hpp>

using namespace crimild;
using namespace crimild::coding;

void TextDecoder::fromString( std::string str )
{
   // TODO
   assert( false );
}

crimild::Bool TextDecoder::decode( std::string key, SharedPointer< coding::Codable > &codable )
{
   assert( false );
   return false;
}

crimild::Bool TextDecoder::decode( std::string key, std::string &value )
{
   assert( false );
   return false;
}

crimild::Size TextDecoder::beginDecodingArray( std::string key )
{
   assert( false );
   return 0;
}

std::string TextDecoder::beginDecodingArrayElement( std::string key, crimild::Size index )
{
   assert( false );
   return "";
}

void TextDecoder::endDecodingArrayElement( std::string key, crimild::Size index )
{
   assert( false );
}

void TextDecoder::endDecodingArray( std::string key )
{
   assert( false );
}
