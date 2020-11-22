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

#include "Rendering/Uniforms/CameraViewProjectionUniformBuffer.hpp"
#include "SceneGraph/Camera.hpp"
#include "Simulation/Simulation.hpp"

using namespace crimild;

CameraViewProjectionUniform::CameraViewProjectionUniform( Camera *camera ) noexcept
    : UniformBuffer( Props { } ),
      m_camera( camera )
{
    
}

void CameraViewProjectionUniform::onPreRender( void ) noexcept
{
    setValue(
        Props {
            .view = [&] {
                if ( m_camera != nullptr ) {
                    // if a camera has been specified, we use that one to get the view matrix
                    return m_camera->getViewMatrix();
                }
                
                if ( auto camera = Camera::getMainCamera() ) {
                    // if no camera has been set, let's use whatever's the main one
                    return camera->getViewMatrix();
                }
                
                // no camera
                return Matrix4f::IDENTITY;
            }(),
            .proj = [&] {
                auto proj = Matrix4f::IDENTITY;
                if ( m_camera != nullptr ) {
                    proj = m_camera->getProjectionMatrix();
                }
                else if ( auto camera = Camera::getMainCamera() ) {
                    proj = camera->getProjectionMatrix();
                }
                return proj;
            }(),
            .viewport = [] {
				auto settings = Simulation::getInstance()->getSettings();
                auto w = settings->get< float >( "video.width", 1024 );
                auto h = settings->get< float >( "video.height", 1024 );
                return Vector2f( w, h );
            }(),
        }
    );
}


