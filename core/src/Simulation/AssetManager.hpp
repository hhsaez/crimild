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

#ifndef CRIMILD_CORE_SIMULATION_ASSET_MANAGER_
#define CRIMILD_CORE_SIMULATION_ASSET_MANAGER_

#include "Foundation/SharedObject.hpp"
#include "Foundation/Macros.hpp"
#include "Foundation/Singleton.hpp"

#include <memory>
#include <map>
#include <string>

namespace crimild {

    class AssetManager : public DynamicSingleton< AssetManager > {
        CRIMILD_DISALLOW_COPY_AND_ASSIGN( AssetManager )
        
    public:
        AssetManager( void );
        virtual ~AssetManager( void );

        void add( std::string name, SharedObjectPtr const &asset )
        {
            _assets[ name ] = asset;
        }

        template< class T >
        SharedPointer< T > get( std::string name )
        {
            return std::static_pointer_cast< T >( _assets[ name ] );
        }

        void clear( void )
        {
            _assets.clear();
        }

    private:
        std::map< std::string, SharedObjectPtr > _assets;
    };

}

#endif

