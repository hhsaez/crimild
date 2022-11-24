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

#include "Foundation/RTTI.hpp"
#include "Foundation/NamedObject.hpp"
#include "Simulation/Event.hpp"

#include <sstream>
#include <imgui.h>

namespace crimild {

    namespace editor {

        namespace layout {

            class Node : public NamedObject {
            public:
                virtual ~Node( void ) = default;
                
                std::string getUniqueName( void ) const noexcept
                {
                    std::stringstream ss;
                    ss << getName() << "##" << ( size_t ) this;
                    return ss.str();
                }

                virtual Event handle( const Event &e ) noexcept;
                virtual void build( ImGuiID parent ) noexcept = 0;
                virtual void render( void ) noexcept = 0;
            };

            class Dockspace : public Node {
            public:
                virtual Event handle( const Event &e ) noexcept;
                virtual void build( ImGuiID parent ) noexcept;
                virtual void render( void ) noexcept;
                
                std::shared_ptr< Node > root;
                
            private:
                Extent2D m_extent;
                bool m_reset = true;
            };

            class VerticalSplit : public Node {
            public:
                virtual Event handle( const Event &e ) noexcept;
                virtual void build( ImGuiID parent ) noexcept;
                virtual void render( void ) noexcept;
                
                std::shared_ptr< Node > left;
                std::shared_ptr< Node > right;
                
                float fraction = 0.5f;
                float pixels = 0;
            };

            class HorizontalSplit : public Node {
            public:
                virtual Event handle( const Event &e ) noexcept;
                virtual void build( ImGuiID parent ) noexcept;
                virtual void render( void ) noexcept;
                
                std::shared_ptr< Node > top;
                std::shared_ptr< Node > bottom;
                
                float fraction = 0.5f;
            };

            class Panel : public Node {
            public:
                virtual Event handle( const Event &e ) noexcept;
                virtual void build( ImGuiID parent ) noexcept;
            };

            class LayoutNode : public RTTI {
            protected:
                LayoutNode( void ) = default;

            public:
                virtual ~LayoutNode( void ) = default;

                virtual void handle( const Event &e ) noexcept;
                virtual void render( void ) noexcept;

                std::shared_ptr< LayoutNode > first;
                std::shared_ptr< LayoutNode > second;

                enum class Direction {
                    HORIZONTAL,
                    VERTICAL,
                };
                Direction direction = Direction::HORIZONTAL;

                enum class SplitUnits {
                    PERCENTAGE,
                    PIXELS,
                };
                SplitUnits splitUnits = SplitUnits::PERCENTAGE;
                float split = 0.5;
            };

        }

    }

}

#endif
