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

#ifndef CRIMILD_VULKAN_RENDERING_SHADER_COMPILER_
#define CRIMILD_VULKAN_RENDERING_SHADER_COMPILER_

#include "Foundation/Containers/Array.hpp"
#include "Foundation/Types.hpp"
#include "Rendering/Shader.hpp"

#include <string>
#include <unordered_map>

namespace crimild {

    namespace vulkan {

        class ShaderCompiler {
        private:
            class ShaderPreprocessor {
            public:
                Bool isInitialized( void ) const noexcept { return !m_chunks.empty(); }

                void addChunk( std::string key, std::string src ) noexcept
                {
                    const auto expandedKey = std::string( "#include <" ) + key + ">";
                    m_chunks[ expandedKey ] = expand( src );
                }

                std::string expand( std::string src ) noexcept
                {
                    for ( auto &it : m_chunks ) {
                        auto begin = src.find( it.first );
                        if ( begin != std::string::npos ) {
                            src.replace( begin, it.first.length(), it.second );
                        }
                    }

                    // Sanity check
                    const auto begin = src.find( "#include <" );
                    if ( begin != std::string::npos ) {
                        const auto end = src.find_first_of( ">", begin );
                        if ( end != std::string::npos ) {
                            std::cerr << "ERROR: Cannot expand " << src.substr( begin, ( end - begin + 1 ) ) << "\n";
                        }
                        exit( -1 );
                    }

                    return src;
                }

            private:
                std::unordered_map< std::string, std::string > m_chunks;
            };

        public:
            Bool init( void ) noexcept;

            void addChunks( const Array< SharedPointer< Shader > > &chunks ) noexcept;

            Bool compile( Shader::Stage stage, const std::string &source, Shader::Data &out ) noexcept;

            void resetPreprocessor( void ) noexcept;

        private:
            void initPreprocessor( void ) noexcept;

        private:
            Bool m_initialized = false;
            ShaderPreprocessor m_preprocessor;
        };

    }

}

#endif
