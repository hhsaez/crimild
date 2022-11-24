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

#ifndef CRIMILD_DESKTOP_EDITOR_LAYOUT_
#define CRIMILD_DESKTOP_EDITOR_LAYOUT_

#include "Foundation/NamedObject.hpp"
#include "Foundation/RTTI.hpp"
#include "Simulation/Event.hpp"

#include <imgui.h>
#include <sstream>

namespace crimild {

    namespace editor {

        namespace layout {

            class Layout : public NamedObject {
            public:
                using Fraction = float;
                using Pixels = uint32_t;
                
                // Values match the ones in ImGuiDir_
                enum class Direction : int {
                    NONE = -1,
                    LEFT = 0,
                    RIGHT = 1,
                    UP = 2,
                    DOWN = 3,
                };
                
            public:
                explicit Layout( std::string_view name ) noexcept;
                Layout( Direction direction, Fraction fraction ) noexcept;
                Layout( Direction direction, Pixels pixels ) noexcept;
                virtual ~Layout( void ) noexcept = default;

                std::string getUniqueName( void ) const noexcept
                {
                    std::stringstream ss;
                    ss << getName() << "##" << ( size_t ) this;
                    return ss.str();
                }
                
                inline ImGuiID getID( void ) const noexcept { return m_id; }
                
                inline const ImVec2 &getPos( void ) const noexcept { return m_pos; }

                inline const std::shared_ptr< Layout > &getFirst( void ) const noexcept { return m_first; }
                inline std::shared_ptr< Layout > &getFirst( void ) noexcept { return m_first; }
                void setFirst( std::shared_ptr< Layout > const &first ) noexcept;
                
                inline const std::shared_ptr< Layout > &getSecond( void ) const noexcept { return m_second; }
                inline std::shared_ptr< Layout > &getSecond( void ) noexcept { return m_second; }
                void setSecond( std::shared_ptr< Layout > const &second ) noexcept;
                
                inline const Layout *getParent( void ) const noexcept { return m_parent; }
                inline Layout *getParent( void ) noexcept { return m_parent; }
                
                virtual Event handle( const Event &e ) noexcept;
                virtual void build( ImGuiID id ) noexcept;
                virtual void render( void ) noexcept;
                
                virtual void removeFromParent( void ) noexcept;
                
            protected:
                inline void setParent( Layout *parent ) noexcept { m_parent = parent; }
                
                virtual void onLayoutChanged( void ) noexcept;
                
            private:
                Direction m_direction = Direction::NONE;
                std::shared_ptr< Layout > m_first;
                std::shared_ptr< Layout > m_second;
                Layout *m_parent = nullptr;
                ImGuiID m_id;
                Fraction m_fraction = 0.5f;
                Pixels m_pixels = 0;
                ImVec2 m_size;
                ImVec2 m_pos;
            };

            class Dockspace : public Layout {
            public:
                Dockspace( void ) : Layout( "Dockspace" ) { }
                virtual ~Dockspace( void ) = default;
                
                virtual Event handle( const Event &e ) noexcept override;
                virtual void render( void ) noexcept override;

                virtual void onLayoutChanged( void ) noexcept override
                {
                    m_reset = true;
                    Layout::onLayoutChanged();
                }

            private:
                Extent2D m_extent;
                bool m_reset = true;
            };

            class Tab : public Layout {
            public:
                Tab( void ) : Layout( "Tab" ) { }
                virtual ~Tab( void ) = default;
            };
        
            class Panel : public Layout {
            public:
                explicit Panel( std::string_view name ) noexcept;
                virtual ~Panel( void ) = default;
                
                virtual void build( ImGuiID parent ) noexcept override;
            };
        
        }

    }

}

#endif
