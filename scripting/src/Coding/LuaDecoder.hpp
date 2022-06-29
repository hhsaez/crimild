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

#ifndef CRIMILD_SCRIPTING_CODING_LUA_DECODER_
#define CRIMILD_SCRIPTING_CODING_LUA_DECODER_

#include "Foundation/Scripted.hpp"
#include "Mathematics/Quaternion.hpp"

#include <Coding/Decoder.hpp>
#include <Foundation/Containers/Stack.hpp>

namespace crimild {

    namespace coding {

        class LuaDecoder : public coding::Decoder,
                           public scripting::Scripted {
        public:
            explicit LuaDecoder( std::string rootObjectName = "scene" );
            virtual ~LuaDecoder( void );

        public:
            virtual crimild::Bool decode( std::string key, SharedPointer< coding::Codable > &codable ) override;

            virtual crimild::Bool decode( std::string key, std::string &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Size &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt8 &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt16 &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Int16 &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Int32 &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::UInt32 &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Bool &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Real32 &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Real64 &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::ColorRGB &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::ColorRGBA &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector2f &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector3f &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Vector4f &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Matrix3f &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Matrix4f &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, crimild::Quaternion &value ) override { return decodeValue( key, value ); }
            virtual crimild::Bool decode( std::string key, Transformation &value ) override { return decodeValue( key, value ); }

            virtual crimild::Bool decode( std::string key, Extent3D &value ) override
            {
                // TODO
                return false;
            }

            virtual bool decode( std::string_view key, std::vector< std::byte > & ) override { return false; }

            virtual crimild::Bool decode( std::string key, ByteArray &value ) override
            {
                /* no-op */
                return true;
            }
            virtual crimild::Bool decode( std::string key, Array< crimild::Real32 > &value ) override { return true; };
            virtual crimild::Bool decode( std::string key, Array< Vector3f > &value ) override { return true; };
            virtual crimild::Bool decode( std::string key, Array< Vector4f > &value ) override { return true; };
            virtual crimild::Bool decode( std::string key, Array< Matrix3f > &value ) override { return true; };
            virtual crimild::Bool decode( std::string key, Array< Matrix4f > &value ) override { return true; };
            virtual crimild::Bool decode( std::string key, Array< Quaternion > &value ) override { return true; };

            void parse( std::string str );
            void parseFile( std::string filename );

            bool decodeFile( std::string fileName );

        protected:
            virtual crimild::Size beginDecodingArray( std::string key ) override;
            virtual std::string beginDecodingArrayElement( std::string key, crimild::Size index ) override;
            virtual void endDecodingArrayElement( std::string key, crimild::Size index ) override;
            virtual void endDecodingArray( std::string key ) override;

        private:
            template< typename T >
            crimild::Bool decodeValue( std::string key, T &value )
            {
                return _evals.top().getPropValue( key, value );
            }

        private:
            SharedPointer< SharedObject > buildObject( void );

        private:
            std::string _rootObjectName;
            Stack< scripting::ScriptEvaluator > _evals;
        };

    }

}

#endif
