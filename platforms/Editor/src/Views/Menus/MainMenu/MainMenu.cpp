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

#include "Views/Menus/MainMenu/MainMenu.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Views/Windows/FileSystemWindow/FileSystemWindow.hpp"
#include "Views/Windows/InspectorWindow/InspectorWindow.hpp"
#include "Views/Windows/LogWindow/LogWindow.hpp"
#include "Views/Windows/Scene3DWindow/Scene3DWindow.hpp"
#include "Views/Windows/SceneWindow/SceneWindow.hpp"
#include "Views/Windows/SimulationWindow/SimulationWindow.hpp"

using namespace crimild::editor;

void renderFileMenu( void ) noexcept
{
    static std::string dialogId;
    static std::function< void( const std::filesystem::path & ) > dialogHandler;

    auto openDialog = [ & ]( std::string id, std::string title, auto handler, const char *filters = ".crimild", std::string pathName = "." ) {
        dialogId = id;
        dialogHandler = handler;
        ImGuiFileDialogFlags flags = ImGuiFileDialogFlags_None;
        ImGuiFileDialog::Instance()->OpenDialog( id, title, filters, pathName, 1, nullptr, flags );
    };

    // auto project = Editor::getInstance()->getProject();
    // const auto hasProject = project != nullptr
    const auto hasProject = false;

    auto enabledWithProject = [ & ]( auto fn ) {
        if ( !hasProject ) {
            ImGui::BeginDisabled();
        }
        fn();
        if ( !hasProject ) {
            ImGui::EndDisabled();
        }
    };

    if ( ImGui::BeginMenu( "File" ) ) {
        if ( ImGui::MenuItem( "New Project..." ) ) {
            openDialog(
                "NewProjectDlgKey",
                "Create Project",
                []( const auto &path ) {
                    // Editor::getInstance()->createProject( path );
                },
                ".crimild"
            );
        }

        enabledWithProject(
            [ & ] {
                if ( ImGui::MenuItem( "New Scene..." ) ) {
                    openDialog(
                        "NewSceneDlgKey",
                        "Create Scene",
                        []( const auto &path ) {
                            // Editor::getInstance()->createNewScene( path );
                        },
                        ".crimild",
                        "" // project->getScenesDirectory().string()
                    );
                }
            }
        );

        ImGui::Separator();

        if ( ImGui::MenuItem( "Open Project..." ) ) {
            openDialog(
                "OpenProjectDlgKey",
                "Open Project",
                []( const auto &path ) {
                    // Editor::getInstance()->loadProject( path );
                },
                ".crimild"
            );
        }

        enabledWithProject(
            [ & ] {
                if ( ImGui::MenuItem( "Open Scene..." ) ) {
                    openDialog(
                        "OpenSceneDlgKey",
                        "Open Scene",
                        []( const auto &path ) {
                            // Editor::getInstance()->loadScene( path );
                        },
                        ".crimild",
                        "" // project->getScenesDirectory().string()
                    );
                }
            }
        );

        ImGui::Separator();

        enabledWithProject(
            [ & ] {
                if ( ImGui::MenuItem( "Save Project" ) ) {
                    // Editor::getInstance()->saveProject();
                }
            }
        );

        enabledWithProject(
            [ & ] {
                if ( ImGui::MenuItem( "Save Scene" ) ) {
                    // Editor::getInstance()->saveScene();
                }
            }
        );

        enabledWithProject(
            [ & ] {
                if ( ImGui::MenuItem( "Save Scene As..." ) ) {
                    openDialog(
                        "SaveSceneAsDlgKey",
                        "Save Scene As...",
                        []( const auto &path ) {
                            // Editor::getInstance()->saveSceneAs( path );
                        },
                        ".crimild",
                        "" // project->getScenesDirectory().string()
                    );
                }
            }
        );

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Recent Projects..." ) ) {
            // const auto &recentProjects = Editor::getInstance()->getRecentProjects();
            // if ( recentProjects.empty() ) {
            ImGui::MenuItem( "No Recent Projects" );
            // } else {
            //     for ( const auto &path : recentProjects ) {
            //         if ( path.empty() ) {
            //             continue;
            //         }
            //         if ( ImGui::MenuItem( path.c_str() ) ) {
            //             crimild::concurrency::sync_frame(
            //                 [ path ] {
            //                     Editor::getInstance()->loadProject( path );
            //                 }
            //             );
            //         }
            //     }
            // }
            ImGui::EndMenu();
        }

        ImGui::Separator();

        enabledWithProject(
            [ & ] {
                if ( ImGui::BeginMenu( "Import..." ) ) {
                    if ( ImGui::MenuItem( "Wavefront OBJ (.obj)..." ) ) {
                        openDialog(
                            "ImportOBJDlgKey",
                            "Import",
                            []( const auto &path ) {
                                // OBJLoader loader( path.string() );
                                // if ( auto model = loader.load() ) {
                                //     Editor::getInstance()->addToScene( model );
                                // } else {
                                //     CRIMILD_LOG_ERROR( "Cannot load model from file ", path.string() );
                                // }
                            },
                            ".obj",
                            "" // project->getAssetsDirectory().string()
                        );
                    }
                    if ( ImGui::MenuItem( "glTF 2.0 (.gltf)..." ) ) {
                        openDialog(
                            "ImportGLTFDlgKey",
                            "Import",
                            []( const auto &path ) {
                                // GLTFImporter importer;
                                // auto model = importer.import( path );
                                // if ( model != nullptr ) {
                                //     Editor::getInstance()->addToScene( model );
                                // } else {
                                //     CRIMILD_LOG_ERROR( "Cannot load model from file ", path.string() );
                                // }
                            },
                            ".gltf",
                            "" // project->getAssetsDirectory().string()
                        );
                    }
                    ImGui::EndMenu();
                }
            }
        );

        ImGui::Separator();

        if ( ImGui::MenuItem( "Quit" ) ) {
            // Editor::getInstance()->terminate();
        }

        ImGui::EndMenu();
    }

    if ( dialogHandler != nullptr && !dialogId.empty() ) {
        ImGui::SetNextWindowSize( ImVec2( 800, 600 ) );
        if ( ImGuiFileDialog::Instance()->Display( dialogId ) ) {
            if ( ImGuiFileDialog::Instance()->IsOk() ) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                // const auto path = std::filesystem::path { filePathName };
                // // Resolve at the beginning of next frame
                // crimild::concurrency::sync_frame(
                //     [ path, handler = dialogHandler ] {
                //         handler( path );
                //     }
                // );
            }
            ImGuiFileDialog::Instance()->Close();
            dialogHandler = nullptr;
            dialogId = "";
        }
    }
}

template< class PanelType >
static void renderLayoutMenuItem( void ) noexcept
{
    auto panel = PanelType::getInstance();
    if ( ImGui::MenuItem( panel->getName().c_str() ) ) {
        panel->setActive( true );
    }
}

void renderLayoutMenu( void ) noexcept
{
    if ( ImGui::BeginMenu( "Layout" ) ) {
        renderLayoutMenuItem< SceneWindow >();
        renderLayoutMenuItem< FileSystemWindow >();
        renderLayoutMenuItem< InspectorWindow >();

        ImGui::Separator();

        renderLayoutMenuItem< Scene3DWindow >();
        renderLayoutMenuItem< SimulationWindow >();

        ImGui::Separator();

        renderLayoutMenuItem< LogWindow >();

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Layout..." ) ) {
            ImGui::BeginDisabled();
            if ( ImGui::MenuItem( "Default" ) ) {
                // TODO
            }
            ImGui::EndDisabled();

            ImGui::Separator();

            if ( ImGui::MenuItem( "Clear" ) ) {
                // auto panels = panels::Panel::getAllPanels();
                // for ( auto &p : panels ) {
                //     p->setOpen( false );
                // }
            }

            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void renderHelpMenu( void ) noexcept
{
    // static bool showHelpAboutDialog = false;
    // static bool showImGuiDemoWindow = false;

    if ( ImGui::BeginMenu( "Help" ) ) {
        if ( ImGui::MenuItem( "About..." ) ) {
            // showHelpAboutDialog = true;
        }
        if ( ImGui::MenuItem( "ImGui Demo Window..." ) ) {
            // showImGuiDemoWindow = true;
        }
        ImGui::EndMenu();
    }

    // helpAboutDialog( showHelpAboutDialog );

    // if ( showImGuiDemoWindow ) {
    //     ImGui::ShowDemoWindow( &showImGuiDemoWindow );
    // }
}

MainMenu::MainMenu( void ) noexcept
    : View( "MainMenu" )
{
    // no-op
}

void MainMenu::draw( void ) noexcept
{
    if ( isActive() ) {
        drawContent();
    }
}

void MainMenu::drawContent( void ) noexcept
{
    if ( ImGui::BeginMainMenuBar() ) {
        renderFileMenu();
        // renderEditMenu();
        // renderSceneMenu();
        renderLayoutMenu();
        renderHelpMenu();
        ImGui::EndMainMenuBar();
    }
}