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

#ifndef CRIMILD_EDITOR_VIEWS_WINDOWS_LOG_
#define CRIMILD_EDITOR_VIEWS_WINDOWS_LOG_

#include "Foundation/ImGuiUtils.hpp"
#include "Foundation/Log.hpp"
#include "Views/Windows/Window.hpp"

namespace crimild::editor {

    class LogWindow : public Window {
        CRIMILD_IMPLEMENT_RTTI( crimild::editor::LogWindow )

    public:
        class OutputHandler
            : public Log::OutputHandler,
              public DynamicSingleton< OutputHandler > {
        public:
            OutputHandler( int level ) noexcept
                : Log::OutputHandler( level )
            {
                // no-op
            }

            virtual ~OutputHandler( void ) = default;

            inline ImGuiTextBuffer &getBuffer( void ) noexcept { return m_buffer; }
            inline ImVector< int > &getLineOffsets( void ) noexcept { return m_lineOffsets; }
            inline ImVector< uint32_t > &getColors( void ) noexcept { return m_colors; }

            virtual void print( int level, std::string const &line ) noexcept override
            {
                int offset = m_buffer.size();
                m_buffer.append( line.c_str() );
                m_lineOffsets.push_back( offset );

                switch ( level ) {
                    case Log::LOG_LEVEL_DEBUG:
                        m_colors.push_back( IM_COL32( 255, 255, 255, 255 ) );
                        break;

                    case Log::LOG_LEVEL_ERROR:
                    case Log::LOG_LEVEL_FATAL:
                        m_colors.push_back( IM_COL32( 255, 0, 0, 255 ) );
                        break;

                    case Log::LOG_LEVEL_INFO:
                        m_colors.push_back( IM_COL32( 0, 255, 0, 255 ) );
                        break;

                    case Log::LOG_LEVEL_WARNING:
                        m_colors.push_back( IM_COL32( 255, 255, 0, 255 ) );
                        break;

                    default:
                        m_colors.push_back( IM_COL32( 128, 128, 128, 255 ) );
                        break;
                }
            }

        private:
            std::mutex m_mutex;

            ImGuiTextBuffer m_buffer;
            ImVector< int > m_lineOffsets;
            ImVector< uint32_t > m_colors;
        };

    public:
        LogWindow( void ) noexcept;
        ~LogWindow( void ) noexcept = default;

        void drawContent( void ) noexcept final;
    };
}

#endif
