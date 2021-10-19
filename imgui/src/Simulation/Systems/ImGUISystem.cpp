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

#include "Simulation/Systems/ImGUISystem.hpp"

#include "Coding/Encoder.hpp"
#include "Concurrency/Async.hpp"
#include "Foundation/Version.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Operations/ImGUIOperations.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Texture.hpp"
#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/Systems/RenderSystem.hpp"
#include "imgui.h"

using namespace crimild;
using namespace crimild::imgui;
using namespace crimild::framegraph::imgui;

namespace crimild {

    namespace coding {

        class ImGuiInspectorEncoder : public coding::Encoder {
        public:
            virtual crimild::Bool encode( SharedPointer< coding::Codable > const &codable ) override
            {
                ImGui::Text( "Class: %s", codable->getClassName() );
                ImGui::Text( "ID: %llu", codable->getUniqueID() );

                codable->encode( *this );

                return true;
            }

            virtual crimild::Bool encode( std::string key, SharedPointer< coding::Codable > const &codable ) override
            {
                if ( codable == nullptr ) {
                    return false;
                }

                encodeKey( key );

                return true;
            }

            virtual crimild::Bool encode( std::string key, std::string value ) override
            {
                encodeKey( key );
                return true;
            }

            virtual crimild::Bool encode( std::string key, crimild::Size value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt8 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt16 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Int16 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Int32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::UInt32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Bool value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Real32 value ) override { return encodeValue( key, value ); }
            virtual crimild::Bool encode( std::string key, crimild::Real64 value ) override { return encodeValue( key, value ); }

            virtual crimild::Bool encode( std::string key, const ColorRGB &value ) override
            {
                ImGui::ColorEdit3( key.c_str(), const_cast< float * >( &value.r ) );
            }
            virtual crimild::Bool encode( std::string key, const ColorRGBA &value ) override
            {
                ImGui::ColorEdit4( key.c_str(), const_cast< float * >( &value.r ) );
            }
            virtual crimild::Bool encode( std::string key, const Vector2f &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector3f &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector4f &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Matrix3f &value ) override { return encodeValues( key, 9, static_cast< const float * >( &value.c0.x ) ); }
            virtual crimild::Bool encode( std::string key, const Matrix4f &value ) override { return encodeValues( key, 16, static_cast< const float * >( &value[ 0 ].x ) ); }
            virtual crimild::Bool encode( std::string key, const Quaternion &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.getRawData().x ) ); }

            virtual crimild::Bool encode( std::string key, const Transformation &value ) override
            {
                return true;
            }

            virtual crimild::Bool encode( std::string key, ByteArray &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< crimild::Real32 > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Vector3f > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Vector4f > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Matrix3f > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Matrix4f > &value ) override { return false; }
            virtual crimild::Bool encode( std::string key, Array< Quaternion > &value ) override { return false; }

        protected:
            virtual void encodeArrayBegin( std::string key, crimild::Size count ) override
            {
                // TODO
            }

            virtual std::string beginEncodingArrayElement( std::string key, crimild::Size index ) override
            {
                return key;
            }

            virtual void endEncodingArrayElement( std::string key, crimild::Size index ) override
            {
                // TODO
            }

            virtual void encodeArrayEnd( std::string key ) override
            {
                // TODO
            }

        private:
            template< typename T >
            crimild::Bool encodeValue( std::string key, const T &value )
            {
                encodeKey( key );
                return true;
            }

            template< typename T >
            crimild::Bool encodeValues( std::string key, crimild::Size count, const T *values )
            {
                encodeKey( key );
                return true;
            }

            crimild::Bool encodeKey( std::string key )
            {
                ImGui::Text( "%s", key.c_str() );
                return true;
            }
        };

    }

    namespace imgui {

        void applyUILayer( void ) noexcept
        {
            auto frameGraph = RenderSystem::getInstance()->getFrameGraph();

            if ( frameGraph == nullptr ) {
                // This should not happen, since we always have something to render
                frameGraph = renderUI();
            } else {
                // We want to render the ImGUI layer on top of whatever we rendered before
                frameGraph = overlayUI( renderUI(), frameGraph );
            }

            RenderSystem::getInstance()->setFrameGraph( frameGraph );
        }

        void showEditRT( Settings *settings ) noexcept
        {
            if ( !settings->get< Bool >( "ui.edit.rt.show" ) ) {
                return;
            }

            auto maxSamples = settings->get< UInt32 >( "rt.samples.max", 5000 );
            auto sampleCount = settings->get< UInt32 >( "rt.samples.count", 1 );
            auto bounces = settings->get< UInt32 >( "rt.bounces", 10 );
            auto focusDist = settings->get< Real >( "rt.focusDist", Real( 10 ) ); // move to camera
            auto aperture = settings->get< Real >( "rt.aperture", Real( 0.1 ) );  // move to camera
            auto backgroundColor = ColorRGB {
                settings->get< Real >( "rt.background_color.r", 0.5f ),
                settings->get< Real >( "rt.background_color.g", 0.7f ),
                settings->get< Real >( "rt.background_color.b", 1.0f ),
            };

            ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_Always );
            ImGui::SetNextWindowSize( ImVec2( 350, 300 ), ImGuiCond_Always );

            auto open = true;
            if ( ImGui::Begin( "RT Settings", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize ) ) {
                ImGui::Text( "Samples %d", UInt32( sampleCount ) );

                {
                    // Use AlignTextToFramePadding() to align text baseline to the baseline of framed elements (otherwise a Text+SameLine+Button sequence will have the text a little too high by default)
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Max Samples: " );
                    ImGui::SameLine();

                    // Arrow buttons with Repeater
                    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                    ImGui::PushButtonRepeat( true );
                    if ( ImGui::ArrowButton( "##max_samples_left", ImGuiDir_Left ) ) {
                        maxSamples--;
                    }
                    ImGui::SameLine( 0.0f, spacing );
                    if ( ImGui::ArrowButton( "##max_samples_right", ImGuiDir_Right ) ) {
                        maxSamples++;
                    }
                    ImGui::PopButtonRepeat();
                    ImGui::SameLine();
                    ImGui::Text( "%d", UInt32( maxSamples ) );
                }

                {
                    // Use AlignTextToFramePadding() to align text baseline to the baseline of framed elements (otherwise a Text+SameLine+Button sequence will have the text a little too high by default)
                    ImGui::AlignTextToFramePadding();
                    ImGui::Text( "Bounces: " );
                    ImGui::SameLine();

                    // Arrow buttons with Repeater
                    float spacing = ImGui::GetStyle().ItemInnerSpacing.x;
                    ImGui::PushButtonRepeat( true );
                    if ( ImGui::ArrowButton( "##bounces_left", ImGuiDir_Left ) ) {
                        bounces--;
                    }
                    ImGui::SameLine( 0.0f, spacing );
                    if ( ImGui::ArrowButton( "##bounces_right", ImGuiDir_Right ) ) {
                        bounces++;
                    }
                    ImGui::PopButtonRepeat();
                    ImGui::SameLine();
                    ImGui::Text( "%d", UInt32( bounces ) );
                }

                ImGui::SliderFloat( "f", &focusDist, 0.0f, 100.0f, "Focus Distance = %.3f" );
                ImGui::SliderFloat( "a", &aperture, 0.0f, 10.0f, "Aperture = %.3f" );
                ImGui::ColorEdit3( "Background Color", ( float * ) &backgroundColor );

                if ( ImGui::Button( "Reset" ) ) {
                    //settings->set( "rendering.samples", UInt32( 1 ) );
                    sampleCount = 0;
                }

                settings->set( "rt.samples.max", maxSamples );
                settings->set( "rt.samples.count", sampleCount );
                settings->set( "rt.bounces", bounces );
                settings->set( "rt.focusDist", focusDist );
                settings->set( "rt.aperture", aperture );
                settings->set( "rt.background_color.r", backgroundColor.r );
                settings->set( "rt.background_color.g", backgroundColor.g );
                settings->set( "rt.background_color.b", backgroundColor.b );
            }

            Simulation::getInstance()->getSettings()->set( "ui.edit.rt.show", open );

            ImGui::End();
        }

        void showHelpAboutDialog( void ) noexcept
        {
            ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_Always );
            ImGui::SetNextWindowSize( ImVec2( 350, 120 ), ImGuiCond_Always );

            auto open = true;
            if ( ImGui::Begin( "About", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize ) ) {
                Version version;
                auto versionStr = version.getDescription();
                ImGui::Text( "%s", versionStr.c_str() );
                ImGui::Text( "http://crimild.hhsaez.com" );
                ImGui::Text( "" );
                ImGui::Text( "Copyright (c) 2002 - present, H. Hernan Saez" );
                ImGui::Text( "All rights reserved." );
            }

            Simulation::getInstance()->getSettings()->set( "ui.help.about.show", open );

            ImGui::End();
        }

        void showStats( void ) noexcept
        {
            auto frameTime = Simulation::getInstance()->getSimulationClock().getDeltaTime();
            ImGui::Begin( "Stats" );
            ImGui::Text( "Frame Time: %.2f ms", 1000.0f * frameTime );
            ImGui::Text( "FPS: %d", frameTime > 0 ? int( 1.0 / frameTime ) : 0 );
            ImGui::End();
        }

        void showNodeInspector( Node *node ) noexcept
        {
            ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_FirstUseEver );
            ImGui::SetNextWindowSize( ImVec2( 200, 300 ), ImGuiCond_FirstUseEver );

            bool open = false;
            if ( ImGui::Begin( "Node Inspector", &open, ImGuiWindowFlags_NoCollapse ) ) {
                auto encoder = crimild::alloc< coding::ImGuiInspectorEncoder >();
                auto nodePtr = crimild::retain( node );
                encoder->encode( nodePtr );
            }
            ImGui::End();
        }

        void showToolsFramegraph( Settings *settings ) noexcept
        {
            static FrameGraphOperation *selected = nullptr;

            if ( !settings->get< Bool >( "ui.tools.framegraph.show" ) ) {
                return;
            }

            ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_FirstUseEver );
            ImGui::SetNextWindowSize( ImVec2( 300, 300 ), ImGuiCond_FirstUseEver );

            class FramegraphTraversal {
            public:
                void operator()( FrameGraphOperation *op ) noexcept
                {
                    traverse( op );
                }

            private:
                void traverse( FrameGraphOperation *op ) noexcept
                {
                    if ( ImGui::TreeNodeEx( op->getName().c_str() ) ) {
                        if ( ImGui::IsItemClicked() ) {
                            selected = op;
                        }

                        op->eachBlockedBy(
                            [ & ]( auto other ) {
                                traverse( get_ptr( other ) );
                            } );

                        ImGui::TreePop();
                    }
                }
            };

            auto open = true;
            if ( ImGui::Begin( "Framegraph", &open, ImGuiWindowFlags_NoCollapse ) ) {
                auto framegraph = RenderSystem::getInstance()->getFrameGraph();
                if ( framegraph != nullptr ) {
                    auto traverse = FramegraphTraversal();
                    traverse( get_ptr( framegraph ) );
                } else {
                    ImGui::Text( "No valid scene" );
                }
                ImGui::Text( "" ); // padding
            }

            if ( !open ) {
                selected = nullptr;
            }

            settings->set( "ui.tools.framegraph.show", open );

            ImGui::End();
        }

        void showToolsSceneTree( Settings *settings ) noexcept
        {
            static Node *selected = nullptr;

            class SceneTreeBuilder : public NodeVisitor {
            public:
                void visitNode( Node *node ) override
                {
                    if ( ImGui::TreeNodeEx( getNodeName( node ).c_str() ) ) {
                        if ( ImGui::IsItemClicked() ) {
                            selected = node;
                        }
                        ImGui::TreePop();
                    }
                }

                void visitGroup( Group *group ) override
                {
                    if ( ImGui::TreeNode( getNodeName( group ).c_str() ) ) {
                        if ( ImGui::IsItemClicked() ) {
                            selected = group;
                        }
                        NodeVisitor::visitGroup( group );
                        ImGui::TreePop();
                    }
                }

            private:
                std::string getNodeName( Node *node ) noexcept
                {
                    return !node->getName().empty()
                               ? node->getName()
                               : node->getClassName();
                }

            private:
                Size m_ptrId = 0;
                ImGuiTreeNodeFlags m_baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
            };

            if ( !settings->get< Bool >( "ui.tools.scene_tree.show" ) ) {
                return;
            }

            ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_FirstUseEver );
            ImGui::SetNextWindowSize( ImVec2( 200, 300 ), ImGuiCond_FirstUseEver );

            auto open = true;
            if ( ImGui::Begin( "Scene", &open, ImGuiWindowFlags_NoCollapse ) ) {
                auto scene = Simulation::getInstance()->getScene();
                if ( scene ) {
                    scene->perform( SceneTreeBuilder() );
                    if ( selected ) {
                        showNodeInspector( selected );
                    }
                } else {
                    ImGui::Text( "No valid scene" );
                    selected = nullptr;
                }
                ImGui::Text( "" ); // padding
            }

            if ( !open ) {
                selected = nullptr;
            }

            settings->set( "ui.tools.scene_tree.show", open );

            ImGui::End();
        }

        void showEditRendering( Settings *settings ) noexcept
        {
            if ( !settings->get< Bool >( "ui.edit.rendering.show" ) ) {
                return;
            }

            ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_FirstUseEver );
            ImGui::SetNextWindowSize( ImVec2( 200, 300 ), ImGuiCond_FirstUseEver );

            auto open = true;
            if ( ImGui::Begin( "Rendering Options", &open, ImGuiWindowFlags_NoCollapse ) ) {
                if ( ImGui::CollapsingHeader( "SSAO" ) ) {
                    auto radius = settings->get< Real32 >( "video.ssao.radius", 5.0f );
                    ImGui::InputFloat( "Radius", &radius, 0.01f, 100.0f, "%.3f" );
                    settings->set( "video.ssao.radius", radius );

                    auto bias = settings->get< Real32 >( "video.ssao.bias", 0.05f );
                    ImGui::InputFloat( "bias", &bias, 0.01f, 100.0f, "%.3f" );
                    settings->set( "video.ssao.bias", bias );
                }

                if ( ImGui::CollapsingHeader( "Tonemapping" ) ) {
                    auto exposure = settings->get< Real32 >( "video.exposure", 1.0f );
                    ImGui::InputFloat( "Exposure", &exposure, 0.01f, 1.0f, "%.3f" );
                    settings->set( "video.exposure", exposure );
                }
            }

            ImGui::End();

            settings->set( "ui.edit.rendering.show", open );
        }

        void showEditSettings( Settings *settings ) noexcept
        {
            if ( !settings->get< Bool >( "ui.edit.settings.show" ) ) {
                return;
            }

            ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_FirstUseEver );
            ImGui::SetNextWindowSize( ImVec2( 400, 300 ), ImGuiCond_FirstUseEver );

            auto open = true;
            if ( ImGui::Begin( "Settings", &open, ImGuiWindowFlags_NoCollapse ) ) {
                settings->each(
                    [ & ]( auto key, auto ) {
                        ImGui::Text( "%s: %s", key.c_str(), settings->get< std::string >( key ).c_str() );
                    } );
            }

            ImGui::End();

            settings->set( "ui.edit.settings.show", open );
        }

        void showMainMenu( Settings *settings ) noexcept
        {
            auto showAbout = false;
            if ( ImGui::BeginMainMenuBar() ) {
                if ( ImGui::BeginMenu( "File" ) ) {
                    ImGui::EndMenu();
                }

                if ( ImGui::BeginMenu( "Edit" ) ) {
                    if ( ImGui::MenuItem( "Rendering..." ) ) {
                        settings->set( "ui.edit.rendering.show", true );
                    }
                    if ( ImGui::MenuItem( "RT..." ) ) {
                        settings->set( "ui.edit.rt.show", true );
                    }
                    if ( ImGui::MenuItem( "Settings..." ) ) {
                        settings->set( "ui.edit.settings.show", true );
                    }
                    ImGui::EndMenu();
                }

                if ( ImGui::BeginMenu( "Rendering" ) ) {
                    if ( ImGui::MenuItem( "Default" ) ) {
                        crimild::concurrency::sync_frame(
                            [] {
                                RenderSystem::getInstance()->useDefaultRenderPath();
                                applyUILayer();
                            } );
                    }

                    ImGui::Separator();

                    // call render passes conditationally again !!!

                    if ( ImGui::MenuItem( "RT (Soft)" ) ) {
                        crimild::concurrency::sync_frame(
                            [] {
                                RenderSystem::getInstance()->useRTSoftRenderPath();
                                applyUILayer();
                            } );
                    }

                    if ( ImGui::MenuItem( "RT (Compute)" ) ) {
                        crimild::concurrency::sync_frame(
                            [] {
                                RenderSystem::getInstance()->useRTComputeRenderPath();
                                applyUILayer();
                            } );
                    }

                    ImGui::Separator();

                    if ( ImGui::MenuItem( "Debug" ) ) {
                        crimild::concurrency::sync_frame(
                            [] {
                                RenderSystem::getInstance()->useDefaultRenderPath( true );
                                applyUILayer();
                            } );
                    }
                    ImGui::EndMenu();
                }

                if ( ImGui::BeginMenu( "Tools" ) ) {
                    if ( ImGui::MenuItem( "Scene Tree..." ) ) {
                        settings->set( "ui.tools.scene_tree.show", true );
                    }
                    if ( ImGui::MenuItem( "Framegraph..." ) ) {
                        settings->set( "ui.tools.framegraph.show", true );
                    }
                    ImGui::EndMenu();
                }

                if ( ImGui::BeginMenu( "Help" ) ) {
                    if ( ImGui::MenuItem( "Stats..." ) ) {
                        showStats();
                    }
                    if ( ImGui::MenuItem( "About..." ) ) {
                        settings->set( "ui.help.about.show", true );
                    }
                    ImGui::EndMenu();
                }

                ImGui::EndMainMenuBar();
            }

            if ( settings->get< Bool >( "ui.help.about.show" ) ) {
                showHelpAboutDialog();
            }

            showEditRendering( settings );
            showEditRT( settings );
            showEditSettings( settings );

            showToolsSceneTree( settings );
            showToolsFramegraph( settings );
        }

    }
}

void ImGUISystem::start( void ) noexcept
{
    System::start();

    CRIMILD_LOG_TRACE( "Starting ImGUI System" );

    IMGUI_CHECKVERSION();

    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    auto &io = ImGui::GetIO();
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;

    updateDisplaySize();

    registerMessageHandler< messaging::KeyPressed >( [ this ]( messaging::KeyPressed const &msg ) {
        // int key = msg.key;
        // self->_keys[ key ] = true;
    } );

    registerMessageHandler< messaging::KeyReleased >( [ this ]( messaging::KeyReleased const &msg ) {
        int key = msg.key;
        if ( key == CRIMILD_INPUT_KEY_F12 ) {
            auto sim = Simulation::getInstance();
            auto settings = sim->getSettings();
            settings->set( "ui.show", !settings->get< Bool >( "ui.show" ) );
        }
        // self->_keys[ key ] = false;
    } );

    registerMessageHandler< messaging::MouseButtonDown >( [ this ]( messaging::MouseButtonDown const &msg ) {
        int button = msg.button;
        auto &io = ImGui::GetIO();
        io.MouseDown[ button ] = true;
    } );

    registerMessageHandler< messaging::MouseButtonUp >( [ this ]( messaging::MouseButtonUp const &msg ) {
        int button = msg.button;
        auto &io = ImGui::GetIO();
        io.MouseDown[ button ] = false;
    } );

    registerMessageHandler< messaging::MouseMotion >( [ this ]( messaging::MouseMotion const &msg ) {
        auto &io = ImGui::GetIO();
        io.MousePos = ImVec2( msg.x, msg.y );
        // Vector2f pos( msg.x, msg.y );
        // self->_mouseDelta = pos - self->_mousePos;
        // self->_mousePos = pos;

        // Vector2f npos( msg.nx, msg.ny );
        // self->_normalizedMouseDelta = npos - self->_normalizedMousePos;
        // self->_normalizedMousePos = npos;
    } );

    registerMessageHandler< messaging::MouseScroll >( [ this ]( messaging::MouseScroll const &msg ) {
        // _mouseScrollDelta = Vector2f( msg.dx, msg.dy );
    } );

    setFrameCallback(
        [] {
            auto sim = Simulation::getInstance();
            auto settings = sim->getSettings();

            auto showUI = settings->get< Bool >( "ui.show", false );
            if ( !showUI ) {
                return;
            }

            showMainMenu( settings );

            if ( settings->get< Bool >( "ui.imgui_demo.show" ) ) {
                // Overrides default UI. Just for demo purposes
                static bool open = true;
                ImGui::ShowDemoWindow( &open );
            }
        } );
}

void ImGUISystem::lateStart( void ) noexcept
{
    applyUILayer();
}

void ImGUISystem::onPreRender( void ) noexcept
{
    auto &io = ImGui::GetIO();

    auto clock = Simulation::getInstance()->getSimulationClock();
    io.DeltaTime = Numericf::max( 1.0f / 60.0f, clock.getDeltaTime() );

    if ( !io.Fonts->IsBuilt() ) {
        CRIMILD_LOG_ERROR( "Font atlas is not built!" );
        return;
    }

    updateDisplaySize();

    ImGui::NewFrame();

    if ( m_frameCallback != nullptr ) {
        m_frameCallback();
    }

    ImGui::Render();
}

void ImGUISystem::onTerminate( void ) noexcept
{
    System::onTerminate();

    ImGui::DestroyContext();
}

void ImGUISystem::updateDisplaySize( void ) noexcept
{
    auto &io = ImGui::GetIO();
    auto width = Simulation::getInstance()->getSettings()->get< float >( "video.width", 0 );
    auto height = Simulation::getInstance()->getSettings()->get< float >( "video.height", 1 );
    auto framebufferScale = Simulation::getInstance()->getSettings()->get< float >( "video.framebufferScale", 1 );
    auto displaySizeScale = 2.0f / framebufferScale;
    io.DisplaySize = ImVec2( width * displaySizeScale, height * displaySizeScale );
    io.DisplayFramebufferScale = ImVec2( 1, 1 );
}
