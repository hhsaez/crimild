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
#include <Crimild_STB.hpp>

using namespace crimild;
using namespace crimild::glfw;

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

            scene->attachNode(
                [] {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( crimild::alloc< QuadPrimitive >() );
                    geometry->setLocal(
                        [] {
                            Transformation t;
                            t.rotate().fromAxisAngle( -Vector3f::UNIT_X, Numericf::HALF_PI );
                            t.setScale( 10.0f );
                            return t;
                        }()
                    );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [] {
                            auto material = crimild::alloc< UnlitMaterial >();
                            material->setColor( RGBAColorf( 0.166, 0.5f, 0.032f, 1.0f ) );
                            return material;
                        }()
                    );
                    return geometry;
                }()
            );

            auto material = [] {
                auto material = crimild::alloc< UnlitMaterial >();
                material->setTexture(
                    [] {
                        auto texture = crimild::alloc< Texture >();
                        texture->imageView = [] {
                            auto imageView = crimild::alloc< ImageView >();
                            imageView->image = ImageManager::getInstance()->loadImage(
                                {
                                    .filePath = {
                                        .path = "assets/textures/grass.png"
                                    },
                                }
                            );
                            return imageView;
                        }();
                        texture->sampler = [] {
                            auto sampler = crimild::alloc< Sampler >();
                            sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_EDGE );
                            return sampler;
                        }();
                        return texture;
                    }()
                );
                return material;
            }();

            auto grassPatch = crimild::alloc< QuadPrimitive >(
                QuadPrimitive::Params { }
            );

            auto rnd = Random::Generator( 1982 );

            Array< SharedPointer< Geometry >>( 20 ).fill(
                [&] ( auto index ) {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( grassPatch );
                    geometry->local().setTranslate(
                        rnd.generate( -10.0f, 5.0f ),
                        1.0f,
                        rnd.generate( -5.0f, 5.0f )
                    );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial( material );
                    return geometry;
                }
            ).sort(
                []( const auto &lhs, const auto &rhs ) {
                    return lhs->getLocal().getTranslate().z() < rhs->getLocal().getTranslate().z();
                }
            ).each(
                [&] ( auto &geometry ) {
                    scene->attachNode( geometry );
                }
            );

            scene->attachNode(
                [] {
                    auto geometry = crimild::alloc< Geometry >();
                    geometry->attachPrimitive( crimild::alloc< QuadPrimitive >() );
                    geometry->attachComponent< MaterialComponent >()->attachMaterial(
                        [] {
                            auto material = crimild::alloc< UnlitMaterial >();
                            material->setTexture(
                                [] {
                                    auto texture = crimild::alloc< Texture >();
                                    texture->imageView = [] {
                                        auto imageView = crimild::alloc< ImageView >();
                                        imageView->image = ImageManager::getInstance()->loadImage(
                                            {
                                                .filePath = {
                                                    .path = "assets/textures/window.png"
                                                },
                                            }
                                        );
                                        return imageView;
                                    }();
                                    texture->sampler = [] {
                                        auto sampler = crimild::alloc< Sampler >();
                                        sampler->setWrapMode( Sampler::WrapMode::CLAMP_TO_EDGE );
                                        return sampler;
                                    }();
                                    return texture;
                                }()
                            );
                            material->getPipeline()->colorBlendState = ColorBlendState {
                                .enable = true,
                            };
                            return material;
                        }()
                    );
                    geometry->setLocal(
                        [] {
                            Transformation t;
                            t.setTranslate( 2.0f, 3.5f, 7.0f );
                            return t;
                        }()
                    );
                    return geometry;
                }()
            );

            scene->attachNode([] {
                auto camera = crimild::alloc< Camera >();
                camera->local().setTranslate( 3.0f, 5.0f, 10.0f );
                camera->local().lookAt( Vector3f::ZERO );
                return camera;
            }());

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
                                    commandBuffer->drawPrimitive( g->anyPrimitive() );
                                }
                            }
						}
					)
				);
				return commandBuffer;
			}();

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

    CRIMILD_SIMULATION_LIFETIME auto sim = crimild::alloc< GLSimulation >( "Alpha Blending", crimild::alloc< Settings >( argc, argv ) );

    SharedPointer< ImageManager > imageManager = crimild::alloc< crimild::stb::ImageManager >();

    sim->addSystem( crimild::alloc< ExampleVulkanSystem >() );

    return sim->run();
}
