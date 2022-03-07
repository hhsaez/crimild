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

#ifndef CRIMILD_CORE_RENDERING_RENDERABLE_SET_
#define CRIMILD_CORE_RENDERING_RENDERABLE_SET_

#include "Foundation/Containers/Array.hpp"
#include "Foundation/NamedObject.hpp"
#include "Foundation/SharedObject.hpp"
#include "Rendering/FrameGraphResource.hpp"

namespace crimild {

    class Camera;
    class Geometry;

    class RenderableSet
        : public SharedObject,
          public NamedObject,
          public FrameGraphResource {

    public:
        FrameGraphResource::Type getType( void ) const noexcept { return FrameGraphResource::Type::RENDERABLE_SET; }

        void reset( void ) noexcept
        {
            m_camera = nullptr;
            m_geometries.clear();
        }

        inline void setCamera( Camera *camera ) noexcept { m_camera = camera; }
        inline Camera *getCamera( void ) noexcept { return m_camera; }

        inline void addGeometry( Geometry *geometry ) noexcept { m_geometries.add( geometry ); }

        Bool hasGeometries( void ) const noexcept { return !m_geometries.empty(); }

        template< typename Fn >
        inline void eachGeometry( Fn fn )
        {
            m_geometries.each( fn );
        }

        template< typename Fn >
        inline void eachGeometry( Fn fn ) const
        {
            m_geometries.each( fn );
        }

    private:
        Camera *m_camera = nullptr;
        Array< Geometry * > m_geometries;
    };

}

#endif
