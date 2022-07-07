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

#include "ObjectFactory.hpp"

#include "Log.hpp"

using namespace crimild;

std::set< std::string > ObjectFactory::filter( std::string_view prefix ) const noexcept
{
    std::set< std::string > ret;
    _builders.eachKey( [ & ]( const auto &key ) {
        if ( key.starts_with( prefix ) ) {
            ret.insert( key );
        }
    } );
    return ret;
}

std::set< std::string > ObjectFactory::filter( const std::set< std::string > &prefixes ) const noexcept
{
    std::set< std::string > ret;
    _builders.eachKey( [ & ]( const auto &key ) {
        for ( const auto &prefix : prefixes ) {
            if ( key.starts_with( prefix ) ) {
                ret.insert( key );
            }
        }
    } );
    return ret;
}

SharedPointer< SharedObject > ObjectFactory::build( std::string className )
{
    auto builder = getBuilder( className );
    if ( builder == nullptr ) {
        Log::warning( CRIMILD_CURRENT_CLASS_NAME, "Cannot find builder for type: ", className );
        return nullptr;
    }

    return builder();
}
