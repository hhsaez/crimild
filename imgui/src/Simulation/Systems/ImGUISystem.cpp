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

#include "Foundation/Version.hpp"
#include "Rendering/ImageView.hpp"
#include "Rendering/Operations/ImGUIOperations.hpp"
#include "Rendering/RenderPass.hpp"
#include "Rendering/Sampler.hpp"
#include "Rendering/Texture.hpp"
#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"
#include "Simulation/Systems/RenderSystem.hpp"
#include "Coding/Encoder.hpp"
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

            //                ImGui::Text( "Name: %s", !node->getName().empty() ? node->getName().c_str() : "<No Name>" );
//                ImGui::Text( "Class: %s", node->getClassName() );

//                _ss << "{ ";
//                _indentLevel++;
//
//                encodeKey( "type" );
//                _ss << "'" << codable->getClassName() << "', ";
//
//                encodeKey( "id" );
//                _ss << codable->getUniqueID() << ", ";
//
                codable->encode( *this );
//
//                _indentLevel--;
//
//                _ss << getIndentSpaces() << "}";
//
//                if ( _arrayKeys.size() == 0 ) {
//                    _ss << "\n";
//                }

                return true;
            }

            virtual crimild::Bool encode( std::string key, SharedPointer< coding::Codable > const &codable ) override
            {
                if ( codable == nullptr ) {
                    return false;
                }

                encodeKey( key );
//                encode( codable );
//                _ss << ", ";

                return true;
            }

            virtual crimild::Bool encode( std::string key, std::string value ) override
            {
                encodeKey( key );
//                _ss << "'" << value << "', ";

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
//            	return encodeValues( key, 3, static_cast< const float * >( &value.r ) );

            }
            virtual crimild::Bool encode( std::string key, const ColorRGBA &value ) override
            {
				ImGui::ColorEdit4( key.c_str(), const_cast< float * >( &value.r ) );
            	//return encodeValues( key, 4, static_cast< const float * >( &value.r ) );
            }
            virtual crimild::Bool encode( std::string key, const Vector2f &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector3f &value ) override { return encodeValues( key, 3, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Vector4f &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.x ) ); }
            virtual crimild::Bool encode( std::string key, const Matrix3f &value ) override { return encodeValues( key, 9, static_cast< const float * >( &value.c0.x ) ); }
            virtual crimild::Bool encode( std::string key, const Matrix4f &value ) override { return encodeValues( key, 16, static_cast< const float * >( &value[ 0 ].x ) ); }
            virtual crimild::Bool encode( std::string key, const Quaternion &value ) override { return encodeValues( key, 4, static_cast< const float * >( &value.getRawData().x ) ); }

            virtual crimild::Bool encode( std::string key, const Transformation &value ) override
            {
                /*
                encodeKey( key );

                _ss << "{ ";
                _indentLevel++;

                encode( "translate", value.getTranslate() );
                encode( "rotate_q", value.getRotate() );
                encode( "scale", value.getScale() );

                _indentLevel--;
                _ss << getIndentSpaces() << "}, ";
                */

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
//                _arrayKeys.push( key );
//
//                _ss << getIndentSpaces() << key << " = { ";
//
//                ++_indentLevel;
            }

            virtual std::string beginEncodingArrayElement( std::string key, crimild::Size index ) override
            {
                return key;
            }

            virtual void endEncodingArrayElement( std::string key, crimild::Size index ) override
            {

            }

            virtual void encodeArrayEnd( std::string key ) override
            {
//                _arrayKeys.pop();
//
//                --_indentLevel;
//                _ss << getIndentSpaces() << "},";
            }

        private:
            template< typename T >
            crimild::Bool encodeValue( std::string key, const T &value )
            {
                encodeKey( key );
//                _ss << value << ", ";

                return true;
            }

            template< typename T >
            crimild::Bool encodeValues( std::string key, crimild::Size count, const T *values )
            {
                encodeKey( key );
//                _ss << "{ ";
//                for ( crimild::Size i = 0; i < count; i++ ) {
//                    if ( i > 0 ) {
//                        _ss << ", ";
//                    }
//                    _ss << values[ i ];
//                }
//                _ss << "}, ";

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
//                ImGui::Text( "Name: %s", !node->getName().empty() ? node->getName().c_str() : "<No Name>" );
//                ImGui::Text( "Class: %s", node->getClassName() );
            }
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
            }

            ImGui::End();

            settings->set( "ui.edit.rendering.show", open );
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
                    ImGui::EndMenu();
                }

                if ( ImGui::BeginMenu( "Tools" ) ) {
                    if ( ImGui::MenuItem( "Scene Tree..." ) ) {
                        settings->set( "ui.tools.scene_tree.show", true );
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

            showToolsSceneTree( settings );
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
