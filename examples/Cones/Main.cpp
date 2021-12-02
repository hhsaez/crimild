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

#include <Crimild.hpp>
#include <Crimild_Vulkan.hpp>
#include <Crimild_GLFW.hpp>

using namespace crimild;
using namespace crimild::glfw;
using namespace crimild::vulkan;

class ExampleVulkanSystem : public GLFWVulkanSystem {
public:
    crimild::Bool start( void ) override
    {
        if ( !GLFWVulkanSystem::start() ) {
            return false;
        }

		m_frameGraph = crimild::alloc< FrameGraph >();

        m_scene = [&] {
            auto scene = crimild::alloc< Group >();

            auto wireframeMaterial = [] {
                auto material = crimild::alloc< UnlitMaterial >();
                material->getPipeline()->primitiveType = Primitive::Type::LINES;
                material->setColor( RGBAColorf::ONE );
                return material;
            }();

            for ( auto i = 0l; i < 25; i++ ) {
                scene->attachNode(
                    [&] {
                        auto group = crimild::alloc< Group >();

                        auto height = Random::generate< Real32 >( 1.0f, 2.0f );
                        auto radius = Random::generate< Real32 >( 0.5f, 1.15f );

                        group->attachNode(
                            [&] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive(
                                    crimild::alloc< ConePrimitive >(
                                        ConePrimitive::Params {
                                            .layout = VertexP3N3TC2::getLayout(),
                                            .height = height,
                                            .radius = radius,
                                        }
                                    )
                                );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial(
                                    [&] {
                                        auto material = crimild::alloc< UnlitMaterial >();
                                        material->setColor( RGBAColorf( 1.0f - 0.2f * ( i % 5 ), 0.2f * ( i % 5 ), ( 1.0f - 0.2f * ( i / 5 ) ), 1.0f ) );
                                        return material;
                                    }()
                                );
                                return geometry;
                            }()
                        );
                        group->attachNode(
                            [&] {
                                auto geometry = crimild::alloc< Geometry >();
                                geometry->attachPrimitive(
                                    crimild::alloc< ConePrimitive >(
                                        ConePrimitive::Params {
                                            .type = Primitive::Type::LINES,
                                            .layout = VertexP3N3TC2::getLayout(),
                                            .height = height,
                                            .radius = radius,
                                        }
                                    )
                                );
                                geometry->attachComponent< MaterialComponent >()->attachMaterial( wireframeMaterial );
                                return geometry;
                            }()
                        );
                        group->local().setTranslate( -5.0f + 2.5f * ( i % 5 ), 0.0f, 5.0f - 2.5f * ( i / 5 ) );
                        return group;
                    }()
                );
            }

            scene->attachNode([] {
                auto camera = crimild::alloc< Camera >();
                camera->local().setTranslate( 5.0f, 10.0f, 10.0f );
                camera->local().lookAt( -2.0f * Vector3f::UNIT_Y );
                camera->attachComponent< FreeLookCameraComponent >();
                Camera::setMainCamera( camera );
                return camera;
            }());

            scene->perform( StartComponents() );

            return scene;
        }();

		m_renderPass = [&] {
            auto renderPass = crimild::alloc< RenderPass >();
            renderPass->attachments = {
                [&] {
                    auto att = crimild::alloc< Attachment >();
                    att->format = Format::COLOR_SWAPCHAIN_OPTIMAL;
                    return att;
                }(),
                [&] {
                    auto att = crimild::alloc< Attachment >();
                    att->format = Format::DEPTH_STENCIL_DEVICE_OPTIMAL;
                    return att;
                }()
            };
            renderPass->setDescriptors(
                [&] {
                    auto descriptorSet = crimild::alloc< DescriptorSet >();
                    descriptorSet->descriptors = {
                        Descriptor {
                            .descriptorType = DescriptorType::UNIFORM_BUFFER,
                            .obj = [&] {
                                FetchCameras fetch;
                                m_scene->perform( fetch );
                                auto camera = fetch.anyCamera();
                                return crimild::alloc< CameraViewProjectionUniform >( camera );
                            }(),
                        },
                    };
                    return descriptorSet;
                }()
            );
            renderPass->commands = [&] {
                auto commandBuffer = crimild::alloc< CommandBuffer >();
                m_scene->perform(
                    ApplyToGeometries(
                        [&]( Geometry *g ) {
                            if ( auto ms = g->getComponent< MaterialComponent >() ) {
                                if ( auto material = ms->first() ) {
                                    commandBuffer->bindGraphicsPipeline( material->getPipeline() );
                                    commandBuffer->bindDescriptorSet( renderPass->getDescriptors() );
                                    commandBuffer->bindDescriptorSet( material->getDescriptors() );
                                    commandBuffer->bindDescriptorSet( g->getDescriptors() );
                                    g->forEachPrimitive(
                                        [&] ( Primitive *p ) {
                                            auto vertices = p->getVertexData()[ 0 ];
                                            auto indices = p->getIndices();
                                            commandBuffer->bindVertexBuffer( get_ptr( vertices ) );
                                            commandBuffer->bindIndexBuffer( indices );
                                            commandBuffer->drawIndexed( indices->getIndexCount() );
                                        }
                                    );
                                }
                            }
						}
					)
				);
				return commandBuffer;
			}();

            renderPass->clearValue = {
                .color = RGBAColorf( 0.95f, 0.9f, 0.25f, 1.0f ),
            };

			return renderPass;
		}();

		m_master = [&] {
            auto master = crimild::alloc< PresentationMaster >();
            master->colorAttachment = m_renderPass->attachments[ 0 ];
			return master;
        }();

        if ( m_frameGraph->compile() ) {
            auto commands = m_frameGraph->recordCommands();
            setCommandBuffers( { commands } );
        }

        return true;
    }

    void update( void ) override
    {
        auto clock = Simulation::getInstance()->getSimulationClock();
        m_scene->perform( UpdateComponents( clock ) );
        m_scene->perform( UpdateWorldState() );

        GLFWVulkanSystem::update();
    }

    void stop( void ) override
    {
        if ( auto renderDevice = getRenderDevice() ) {
            renderDevice->waitIdle();
        }

        m_scene = nullptr;
        m_renderPass = nullptr;
        m_master = nullptr;
        m_frameGraph = nullptr;

        GLFWVulkanSystem::stop();
    }

private:
    SharedPointer< Node > m_scene;
    SharedPointer< FrameGraph > m_frameGraph;
	SharedPointer< RenderPass > m_renderPass;
	SharedPointer< PresentationMaster > m_master;
};

int main( int argc, char **argv )
{
    crimild::init();
    crimild::vulkan::init();

    Log::setLevel( Log::Level::LOG_LEVEL_ALL );

    CRIMILD_SIMULATION_LIFETIME auto sim = crimild::alloc< GLSimulation >( "Cones", crimild::alloc< Settings >( argc, argv ) );
    sim->addSystem( crimild::alloc< ExampleVulkanSystem >() );
    return sim->run();
}
