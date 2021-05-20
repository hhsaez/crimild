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

#ifndef CRIMILD_RENDERING_UNIFORMS_CAMERA_VIEW_PROJECTION_
#define CRIMILD_RENDERING_UNIFORMS_CAMERA_VIEW_PROJECTION_

#include "Mathematics/Matrix4.hpp"
#include "Mathematics/Vector2.hpp"
#include "Rendering/UniformBuffer.hpp"

namespace crimild {

    class Camera;

    class CameraViewProjectionUniform : public UniformBuffer {
    private:
        struct Props {
            alignas( 16 ) Matrix4f view;
            alignas( 16 ) Matrix4f proj;
            alignas( 8 ) Vector2f viewport;
        };

    public:
        explicit CameraViewProjectionUniform( Camera *camera ) noexcept;
        ~CameraViewProjectionUniform( void ) = default;

        void onPreRender( void ) noexcept override;

    private:
        Camera *m_camera = nullptr;
    };

}

#endif
