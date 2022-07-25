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

#ifndef CRIMILD_CORE_DEBUG_DRAW_MANAGER_
#define CRIMILD_CORE_DEBUG_DRAW_MANAGER_

#include "Foundation/Memory.hpp"
#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/Point3.hpp"
#include "Mathematics/Transformation.hpp"

#include <string>
#include <vector>

namespace crimild {

    class Primitive;

    class DebugDrawManager {
    public:
        /**
         * @brief Adds a line segment to the debug drawing queue.
         */
        static void addLine(
            const Point3 &from,
            const Point3 &to,
            const ColorRGB &color,
            float width = 1.0f,
            float duration = 0.0f,
            bool depthEnabled = true
        ) noexcept;

        static void addCross(
            const Point3 &position,
            const ColorRGB &color,
            float size,
            float duration = 0.0f,
            bool depthEnabled = true
        ) noexcept;

        static void addSphere(
            const Point3 &center,
            float radius,
            const ColorRGB &color,
            float duration = 0.0f,
            bool depthEnabled = true
        ) noexcept;

        static void addCircle(
            const Point3 &center,
            const Vector3 &normal,
            float radius,
            const ColorRGB &color,
            float duration = 0.0f,
            bool depthEnabled = true
        ) noexcept;

        static void addAxes(
            const Transformation &transformation,
            float duration = 0.0f,
            bool depthEnabled = true
        ) noexcept;

        static void addText(
            const Point3 &position,
            std::string_view text,
            const ColorRGB &color,
            float duration = 0.0f,
            bool depthEnabled = true
        ) noexcept;

    public:
        static void tick( void ) noexcept;

        template< typename Fn >
        static void eachRenderable( Fn fn ) noexcept
        {
            for ( auto &r : s_renderables ) {
                fn( r );
            }

            for ( auto &r : s_persistentRenderables ) {
                if ( r.duration > 0 ) {
                    fn( r );
                }
            }
        }

        static void reset( bool clearAll = false ) noexcept;

    private:
        struct Renderable {
            SharedPointer< Primitive > primitive;
            Transformation world;
            ColorRGB color;
            float duration;
            bool depthEnabled;
        };

        static std::vector< Renderable > s_renderables;
        static std::vector< Renderable > s_persistentRenderables;
    };

}

#endif
