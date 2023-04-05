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

#ifndef CRIMILD_SIMULATION_SYSTEMS_STREAMING_
#define CRIMILD_SIMULATION_SYSTEMS_STREAMING_

#include "Foundation/Containers/Map.hpp"
#include "Foundation/Log.hpp"
#include "Foundation/Macros.hpp"
#include "Simulation/Systems/System.hpp"

namespace crimild {

    class Node;
    class Group;

    namespace messaging {

        struct LoadScene {
            std::string fileName;
        };

        struct AppendScene {
            std::string fileName;
            Group *parentNode;

            /**
                           \remarks Called before starting components
                         */
            std::function< void( Node * ) > onLoadSceneCallback;
        };

        struct ReloadScene {
        };

        struct SceneLoadFailed {
            std::string fileName;
            std::string message;
        };

    }

    class StreamingSystem : public System {
        CRIMILD_IMPLEMENT_RTTI( crimild::StreamingSystem )

    public:
        StreamingSystem( void );
        ~StreamingSystem( void ) = default;

    public:
        using Builder = std::function< SharedPointer< Node >( std::string ) >;

        void registerBuilder( std::string extension, Builder const &builder ) { _builders.insert( extension, builder ); }

        template< class DECODER_CLASS >
        void registerDecoder( std::string extension )
        {
            registerBuilder( extension, []( std::string filePath ) -> SharedPointer< Node > {
                auto decoder = crimild::alloc< DECODER_CLASS >();
                if ( !decoder->decodeFile( filePath ) ) {
                    crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot decode file ", filePath );
                    return nullptr;
                }

                if ( decoder->getObjectCount() == 0 ) {
                    crimild::Log::error( CRIMILD_CURRENT_CLASS_NAME, "Cannot decode objects from file ", filePath );
                    return nullptr;
                }

                return decoder->template getObjectAt< crimild::Node >( 0 );
            } );
        }

    private:
        void onLoadScene( messaging::LoadScene const &message );
        void onAppendScene( messaging::AppendScene const &message );
        void onReloadScene( messaging::ReloadScene const &message );

    private:
        Map< std::string, Builder > _builders;
        std::string _lastSceneFileName;
    };

}

#endif
