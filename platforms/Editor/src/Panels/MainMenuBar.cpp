#include "Panels/MainMenuBar.hpp"

#include "Foundation/ImGuiUtils.hpp"
#include "Importers/GLTFImporter.hpp"
#include "Simulation/Editor.hpp"
#include "Simulation/Project.hpp"

#include <Crimild.hpp>
#include <filesystem>

using namespace crimild;
using namespace crimild::editor::panels;

void MainMenuBar::render( void ) noexcept
{
    if ( ImGui::BeginMainMenuBar() ) {
        renderFileMenu();
        renderEditMenu();
        renderSceneMenu();
        renderViewMenu();
        renderHelpMenu();
        ImGui::EndMainMenuBar();
    }
}

void MainMenuBar::renderFileMenu( void ) noexcept
{
    static std::string dialogId;
    static std::function< void( const std::filesystem::path & ) > dialogHandler;

    auto openDialog = [ & ]( std::string id, std::string title, auto handler, const char *filters = ".crimild", std::string pathName = "." ) {
        dialogId = id;
        dialogHandler = handler;
        ImGuiFileDialogFlags flags = ImGuiFileDialogFlags_None;
        ImGuiFileDialog::Instance()->OpenDialog( id, title, filters, pathName, 1, nullptr, flags );
    };

    auto project = Editor::getInstance()->getProject();

    auto enabledWithProject = [ hasProject = project != nullptr ]( auto fn ) {
        if ( !hasProject ) {
            ImGui::BeginDisabled();
        }
        fn();
        if ( !hasProject ) {
            ImGui::EndDisabled();
        }
    };

    // const auto baseDirectory = project->getScenesDirectory().string();
    const auto baseDirectory = std::filesystem::current_path().string();

    if ( ImGui::BeginMenu( "File" ) ) {
        if ( ImGui::MenuItem( "New Project..." ) ) {
            openDialog(
                "NewProjectDlgKey",
                "Create Project",
                []( const auto &path ) {
                    Editor::getInstance()->createProject( path );
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
                            Editor::getInstance()->createNewScene( path );
                        },
                        ".crimild",
                        baseDirectory
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
                    Editor::getInstance()->loadProject( path );
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
                            Editor::getInstance()->loadScene( path );
                        },
                        ".crimild",
                        baseDirectory
                    );
                }
            }
        );

        ImGui::Separator();

        enabledWithProject(
            [ & ] {
                if ( ImGui::MenuItem( "Save Project" ) ) {
                    Editor::getInstance()->saveProject();
                }
            }
        );

        enabledWithProject(
            [ & ] {
                if ( ImGui::MenuItem( "Save Scene" ) ) {
                    Editor::getInstance()->saveScene();
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
                            Editor::getInstance()->saveSceneAs( path );
                        },
                        ".crimild",
                        baseDirectory
                    );
                }
            }
        );

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Recent Projects..." ) ) {
            const auto &recentProjects = Editor::getInstance()->getRecentProjects();
            if ( recentProjects.empty() ) {
                ImGui::MenuItem( "No Recent Projects" );
            } else {
                for ( const auto &path : recentProjects ) {
                    if ( path.empty() ) {
                        continue;
                    }
                    if ( ImGui::MenuItem( path.c_str() ) ) {
                        crimild::concurrency::sync_frame(
                            [ path ] {
                                Editor::getInstance()->loadProject( path );
                            }
                        );
                    }
                }
            }
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
                                OBJLoader loader( path.string() );
                                if ( auto model = loader.load() ) {
                                    Editor::getInstance()->addToScene( model );
                                } else {
                                    CRIMILD_LOG_ERROR( "Cannot load model from file ", path.string() );
                                }
                            },
                            ".obj",
                            project->getAssetsDirectory().string()
                        );
                    }
                    if ( ImGui::MenuItem( "glTF 2.0 (.gltf)..." ) ) {
                        openDialog(
                            "ImportGLTFDlgKey",
                            "Import",
                            []( const auto &path ) {
                                GLTFImporter importer;
                                auto model = importer.import( path );
                                if ( model != nullptr ) {
                                    Editor::getInstance()->addToScene( model );
                                } else {
                                    CRIMILD_LOG_ERROR( "Cannot load model from file ", path.string() );
                                }
                            },
                            ".gltf",
                            project->getAssetsDirectory().string()
                        );
                    }
                    ImGui::EndMenu();
                }
            }
        );

        ImGui::Separator();

        if ( ImGui::MenuItem( "Quit" ) ) {
            Editor::getInstance()->terminate();
        }

        ImGui::EndMenu();
    }

    if ( dialogHandler != nullptr && !dialogId.empty() ) {
        ImGui::SetNextWindowSize( ImVec2( 800, 600 ) );
        if ( ImGuiFileDialog::Instance()->Display( dialogId ) ) {
            if ( ImGuiFileDialog::Instance()->IsOk() ) {
                std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
                const auto path = std::filesystem::path { filePathName };
                // Resolve at the beginning of next frame
                crimild::concurrency::sync_frame(
                    [ path, handler = dialogHandler ] {
                        handler( path );
                    }
                );
            }
            ImGuiFileDialog::Instance()->Close();
            dialogHandler = nullptr;
            dialogId = "";
        }
    }
}

void MainMenuBar::renderEditMenu( void ) noexcept
{
    if ( ImGui::BeginMenu( "Edit" ) ) {
        if ( ImGui::MenuItem( "Clone" ) ) {
            Editor::getInstance()->cloneSelectedNode();
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Delete" ) ) {
            Editor::getInstance()->deleteSelectedNode();
        }

        ImGui::EndMenu();
    }
}

auto withName( auto node, std::string name ) noexcept
{
    node->setName( name );
    return node;
}

static void addToScene( SharedPointer< Node > const &node ) noexcept
{
    // TODO(hernan): I'm assuming the root node of a scene is a group, which might not
    // always be the case. Maybe I should check the class type
    auto scene = crimild::cast_ptr< Group >( Simulation::getInstance()->getScene() );
    node->perform( UpdateWorldState() );
    node->perform( StartComponents() );
    scene->attachNode( node );
}

static void addGeometry( std::string name, SharedPointer< Primitive > const &primitive, const Transformation &local = Transformation::Constants::IDENTITY ) noexcept
{
    auto geometry = crimild::alloc< Geometry >( name );
    geometry->attachPrimitive( primitive );
    geometry->attachComponent< MaterialComponent >( crimild::alloc< materials::WorldGrid >() );
    geometry->setLocal( local );
    addToScene( geometry );
}

static void addEmptyNode( void ) noexcept
{
    addToScene( crimild::alloc< Group >() );
}

void MainMenuBar::renderSceneMenu( void ) noexcept
{
    if ( ImGui::BeginMenu( "Scene" ) ) {
        if ( ImGui::MenuItem( "Add Empty" ) ) {
            addEmptyNode();
        }

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Geometry" ) ) {
            if ( ImGui::MenuItem( "Plane" ) ) {
                addGeometry( "Plane", QuadPrimitive::UNIT_QUAD, rotationX( -numbers::PI_DIV_2 ) );
            }

            if ( ImGui::MenuItem( "Box" ) ) {
                addGeometry( "Box", BoxPrimitive::UNIT_BOX );
            }

            if ( ImGui::MenuItem( "Sphere" ) ) {
                addGeometry( "Sphere", SpherePrimitive::UNIT_SPHERE );
            }

            ImGui::EndMenu();
        }

        if ( ImGui::BeginMenu( "Light" ) ) {
            if ( ImGui::MenuItem( "Directional" ) ) {
                auto light = crimild::alloc< Light >( Light::Type::DIRECTIONAL );
                light->setName( "Directional Light" );
                light->setEnergy( 5 );
                light->setCastShadows( true );
                light->setLocal(
                    lookAt(
                        Point3 { -5, 5, 10 },
                        Point3 { 0, 0, 0 },
                        Vector3::Constants::UP
                    )
                );
                addToScene( light );
            }
            if ( ImGui::MenuItem( "Point" ) ) {
                addToScene(
                    withTranslation(
                        withName(
                            crimild::alloc< Light >( Light::Type::POINT ),
                            "Point Light"
                        ),
                        0,
                        1,
                        0
                    )
                );
            }
            if ( ImGui::MenuItem( "Spot" ) ) {
                auto light = crimild::alloc< Light >( Light::Type::SPOT );
                light->setName( "Spot Light" );
                light->setColor( ColorRGB { 1.0f, 1.0f, 1.0f } );
                light->setCastShadows( true );
                light->setEnergy( 1000.0f );
                light->setInnerCutoff( Numericf::DEG_TO_RAD * 20.0f );
                light->setOuterCutoff( Numericf::DEG_TO_RAD * 25.0f );
                light->setLocal(
                    lookAt(
                        Point3 { 10, 10, 0 },
                        Point3 { 0, 0, 0 },
                        Vector3::Constants::UP
                    )
                );

                addToScene( light );
            }

            ImGui::EndMenu();
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Add Camera" ) ) {
            // TODO
        }

        ImGui::EndMenu();
    }
}

void MainMenuBar::renderViewMenu( void ) noexcept
{
    if ( ImGui::BeginMenu( "View" ) ) {
        if ( ImGui::MenuItem( "Project..." ) ) {
            // addPanel( editor, crimild::alloc< editor::ProjectPanel >() );
        }

        if ( ImGui::MenuItem( "Scene Hierarchy..." ) ) {
            // addPanel( editor, crimild::alloc< SceneHierarchyPanel >() );
        }

        if ( ImGui::MenuItem( "Node Inspector..." ) ) {
            // addPanel( editor, crimild::alloc< NodeInspectorPanel >( editor->getRenderDevice() ) );
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Scene..." ) ) {
            // addPanel( editor, crimild::alloc< ScenePanel >( editor->getRenderDevice() ) );
        }

        if ( ImGui::MenuItem( "Simulation..." ) ) {
            // addPanel( editor, crimild::alloc< SimulationPanel >( editor->getRenderDevice() ) );
        }

        ImGui::Separator();

        if ( ImGui::MenuItem( "Behavior Editor..." ) ) {
            // addPanel( editor, crimild::alloc< BehaviorEditorPanel >() );
        }

        if ( ImGui::MenuItem( "Timeline Editor..." ) ) {
            // addPanel( editor, crimild::alloc< TimelinePanel >() );
        }

        ImGui::Separator();

        ImGui::BeginDisabled();
        if ( ImGui::MenuItem( "Render..." ) ) {
            //            addPanel( editor, crimild::alloc< RenderScenePanel >( editor->getRenderDevice() ) );
        }
        ImGui::EndDisabled();

        ImGui::Separator();

        if ( ImGui::MenuItem( "Toolbar..." ) ) {
            // addPanel( editor, crimild::alloc< editor::ToolbarPanel >() );
        }

        ImGui::Separator();

        if ( ImGui::BeginMenu( "Layout..." ) ) {
            if ( ImGui::MenuItem( "Default" ) ) {
                // editor::layout::LayoutManager::getInstance()->loadDefaultLayout();
            }

            ImGui::Separator();

            if ( ImGui::MenuItem( "Clear" ) ) {
                // editor::layout::LayoutManager::getInstance()->clear();
            }
            ImGui::EndMenu();
        }

        ImGui::EndMenu();
    }
}

void helpAboutDialog( bool &open ) noexcept
{
    if ( !open ) {
        return;
    }

    ImGui::SetNextWindowPos( ImVec2( 200, 200 ), ImGuiCond_Always );
    ImGui::SetNextWindowSize( ImVec2( 350, 120 ), ImGuiCond_Always );

    if ( ImGui::Begin( "About", &open, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize ) ) {
        Version version;
        auto versionStr = version.getDescription();
        ImGui::Text( "%s", versionStr.c_str() );
        ImGui::Text( "http://crimild.hhsaez.com" );
        ImGui::Text( "" );
        ImGui::Text( "Copyright (c) 2002 - present, H. Hernan Saez" );
        ImGui::Text( "All rights reserved." );

        ImGui::End();
    }
}

void MainMenuBar::renderHelpMenu( void ) noexcept
{
    static bool showHelpAboutDialog = false;
    static bool showImGuiDemoWindow = false;

    if ( ImGui::BeginMenu( "Help" ) ) {
        if ( ImGui::MenuItem( "About..." ) ) {
            showHelpAboutDialog = true;
        }
        if ( ImGui::MenuItem( "ImGui Demo Window..." ) ) {
            showImGuiDemoWindow = true;
        }
        ImGui::EndMenu();
    }

    helpAboutDialog( showHelpAboutDialog );

    if ( showImGuiDemoWindow ) {
        ImGui::ShowDemoWindow( &showImGuiDemoWindow );
    }
}
