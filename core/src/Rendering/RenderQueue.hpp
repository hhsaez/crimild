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

#ifndef CRIMILD_CORE_RENDERING_RENDER_QUEUE_
#define CRIMILD_CORE_RENDERING_RENDER_QUEUE_

#include "Foundation/SharedObject.hpp"
#include "Foundation/SharedObjectList.hpp"
#include "Foundation/Pointer.hpp"
#include "SceneGraph/Geometry.hpp"
#include "SceneGraph/Camera.hpp"
#include "SceneGraph/Light.hpp"

#include <functional>
#include <list>

namespace crimild {
    
    class RenderQueue : public SharedObject {
    public:
        RenderQueue( void );
        virtual ~RenderQueue( void );
        
        void reset( void );
        
        void setCamera( CameraPtr const &camera ) { _camera = camera; }
        CameraPtr getCamera( void ) { return _camera; }
        
        SharedObjectList< Light > &getLights( void ) { return _lights; }
        SharedObjectList< Geometry > &getShadowCasters( void ) { return _shadowCasters; }
        SharedObjectList< Geometry > &getOpaqueObjects( void ) { return _opaqueObjects; }
        SharedObjectList< Geometry > &getTranslucentObjects( void ) { return _translucentObjects; }
        SharedObjectList< Geometry > &getScreenObjects( void ) { return _screenObjects; }
        
    private:
        CameraPtr _camera;
        
        SharedObjectList< Light > _lights;
        SharedObjectList< Geometry > _shadowCasters;
        SharedObjectList< Geometry > _opaqueObjects;
        SharedObjectList< Geometry > _translucentObjects;
        SharedObjectList< Geometry > _screenObjects;
    };
    
    using RenderQueuePtr = std::shared_ptr< RenderQueue >;
    
}

#endif

