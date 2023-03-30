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

#ifndef CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_SCENE_RT
#define CRIMILD_VULKAN_RENDERING_FRAME_GRAPH_RENDER_SCENE_RT

#include "Mathematics/Normal3Ops.hpp"
#include "Mathematics/dot.hpp"
#include "Rendering/FrameGraph/VulkanRenderBase.hpp"
#include "Visitors/RTAcceleration.hpp"

#include <future>

namespace crimild {

    class BufferAccessor;
    class BufferView;
    class Camera;
    class Node;

    namespace vulkan {

        class Buffer;
        class RenderTarget;

        namespace framegraph {

            class RenderSceneRT
                : public RenderBase,
                  public WithCommandBuffer {
            public:
                enum State {
                    PAUSED,
                    RUNNING,
                    TERMINATED,
                };

            public:
                RenderSceneRT( RenderDevice *device, std::string name, const VkExtent2D &extent ) noexcept;
                virtual ~RenderSceneRT( void ) noexcept;

                void render( crimild::Node *scene, Camera *camera ) noexcept;

                inline const std::shared_ptr< RenderTarget > &getOutput( void ) const noexcept
                {
                    return m_outputTarget;
                }

                void reset( void ) noexcept;

                inline void setState( State state ) noexcept { m_state = state; }
                inline State getState( void ) const noexcept { return m_state; }

                inline size_t getSampleCount( void ) const noexcept { return m_sampleCount; }
                inline float getProgress( void ) const noexcept { return m_progress; }

                inline void setBackgroundColor( const ColorRGB &color ) noexcept { m_backgroundColor = color; }
                inline const ColorRGB &getBackgroundColor( void ) const noexcept { return m_backgroundColor; }

            protected:
                virtual void onResize( void ) noexcept override;

            private:
                struct IntersectionResult {
                    Int32 materialId = -1;
                    Real t = numbers::POSITIVE_INFINITY;
                    Point3 point;
                    Normal3 normal;
                    Bool frontFace;

                    inline void setFaceNormal( const Ray3 &R, const Normal3 &N ) noexcept
                    {
                        frontFace = dot( direction( R ), N ) < 0;
                        normal = frontFace ? N : -N;
                    }
                };

                void setup( crimild::Node *scene, Camera *camera ) noexcept;

                Ray3 getRay( Vector2f uv ) noexcept;
                ColorRGB getRayColor( const Ray3 &R, const ColorRGB &backgroundColor, int depth ) noexcept;
                bool scatter( const Ray3 &R, const IntersectionResult &result, Ray3 &scattered, ColorRGB &attenuation, Bool &shouldBounce ) noexcept;
                bool intersect( const Ray3 &R, IntersectionResult &result ) noexcept;
                bool intersectPrim( const Ray3 &R, Int32 nodeId, Real minT, Real maxT, IntersectionResult &result ) noexcept;
                void updateImage( void ) noexcept;

            private:
                std::shared_ptr< RenderTarget > m_outputTarget;

                std::shared_ptr< BufferView > m_imageData;
                std::shared_ptr< BufferAccessor > m_imageAccessor;
                std::shared_ptr< vulkan::Buffer > m_stagingBuffer;

                SharedPointer< crimild::Node > m_scene;
                SharedPointer< Camera > m_camera;
                RTAcceleration::Result m_acceleratedScene;

                ColorRGB m_backgroundColor = { 0.5, 0.5, 0.5 };

                uint32_t m_sampleCount = 0;
                float m_progress = 0;

                State m_state = State::PAUSED;
                std::future< void > m_done;
            };

        }

    }

}

#endif
