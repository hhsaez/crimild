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
#include "Layout/Layout.hpp"
#include "Simulation/Editor.hpp"
#include "Simulation/Project.hpp"
#include "Views/Dialogs/FileDialog.hpp"
#include "Views/Modals/AboutModal.hpp"
#include "Views/Specials/ImGuiDemoWindow.hpp"
#include "Views/Windows/BehaviorsWindow.hpp"
#include "Views/Windows/FileSystemWindow.hpp"
#include "Views/Windows/GraphEditor/GraphEditorWindow.hpp"
#include "Views/Windows/InspectorWindow.hpp"
#include "Views/Windows/LogWindow.hpp"
#include "Views/Windows/PlaybackControlsWindow.hpp"
#include "Views/Windows/Scene3DWindow.hpp"
#include "Views/Windows/SceneRTWindow.hpp"
#include "Views/Windows/SceneWindow.hpp"
#include "Views/Windows/SimulationWindow.hpp"
#include "Views/Windows/TimelineWindow.hpp"
#include "Views/Workspaces/AssemblyWorkspace.hpp"
#include "Views/Workspaces/WorkspaceManager.hpp"

using namespace crimild;
using namespace crimild::editor;

MainMenu::MainMenu( void ) noexcept
   : View( "MainMenu" )
{
   // no-op
}

void MainMenu::draw( void ) noexcept
{
   drawContent();
}

void MainMenu::drawContent( void ) noexcept
{
   if ( ImGui::BeginMainMenuBar() ) {
      renderFileMenu();
      renderEditMenu();
      renderSceneMenu();
      renderLayoutMenu();
      renderHelpMenu();
      ImGui::EndMainMenuBar();
   }
}

void MainMenu::renderEditMenu( void ) noexcept
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
   auto scene = crimild::cast_ptr< Group >( crimild::Simulation::getInstance()->getScene() );
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

void MainMenu::renderSceneMenu( void ) noexcept
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
                  Point3f { -5, 5, 10 },
                  Point3f { 0, 0, 0 },
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
                  Point3f { 10, 10, 0 },
                  Point3f { 0, 0, 0 },
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

void MainMenu::renderFileMenu( void ) noexcept
{
   auto project = Editor::getInstance()->getProject();
   const auto hasProject = project != nullptr;

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
         getLayout()->addView(
            std::make_shared< FileDialog >(
               "Create Project",
               []( const auto &path ) {
                  Editor::getInstance()->createProject( path );
               },
               ".crimild"
            )
         );
      }

      // enabledWithProject(
      //    [ & ] {
      //       if ( ImGui::MenuItem( "New Scene..." ) ) {
      //          getLayout()->addView(
      //             std::make_shared< FileDialog >(
      //                "Create Project",
      //                []( const auto &path ) {
      //                   Editor::getInstance()->createNewScene( path );
      //                },
      //                ".crimild",
      //                Editor::getInstance()->getProject()->getScenesDirectory().string()
      //             )
      //          );
      //       }
      //    }
      // );

      enabledWithProject(
         [ & ] {
            if ( ImGui::MenuItem( "New Assembly" ) ) {
               getLayout()->addView(
                  crimild::alloc< FileDialog >(
                     "New Assembly",
                     []( const auto &path ) {
                        if ( auto project = Project::getInstance() ) {
                           // Always work with relative paths!
                           auto assemblyPath = project->toRelativePath( path );
                           auto assembly = crimild::alloc< Assembly >( path.stem().string() );
                           if ( auto workspaces = WorkspaceManager::getInstance() ) {
                              workspaces->createWorkspace< AssemblyWorkspace >( assemblyPath, assembly );
                           }
                        }
                     },
                     ".crimild",
                     Editor::getInstance()->getProject()->getRootDirectory().string()
                  )
               );
            }
         }
      );

      ImGui::Separator();

      if ( ImGui::MenuItem( "Open Project..." ) ) {
         getLayout()->addView(
            std::make_shared< FileDialog >(
               "Create Project",
               []( const auto &path ) {
                  Editor::getInstance()->loadProject( path );
               },
               ".crimild"
            )
         );
      }

      enabledWithProject(
         [ & ] {
            if ( ImGui::MenuItem( "Open Assembly..." ) ) {
               getLayout()->addView(
                  std::make_shared< FileDialog >(
                     "Open Assembly",
                     []( const auto &path ) {
                        if ( auto project = Project::getInstance() ) {
                           // Always work with relative paths!
                           auto assemblyPath = project->toRelativePath( path );
                           if ( auto assembly = project->load< Assembly >( assemblyPath ) ) {
                              if ( auto workspaces = WorkspaceManager::getInstance() ) {
                                 workspaces->createWorkspace< AssemblyWorkspace >( assemblyPath, assembly );
                              }
                           } else {
                              CRIMILD_LOG_ERROR( "Cannot load assembly from path ", assemblyPath );
                           }
                        }
                     },
                     ".crimild",
                     Editor::getInstance()->getProject()->getRootDirectory().string()
                  )
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

      // enabledWithProject(
      //    [ & ] {
      //       if ( ImGui::MenuItem( "Save Scene" ) ) {
      //          Editor::getInstance()->saveScene();
      //       }
      //    }
      // );

      // enabledWithProject(
      //    [ & ] {
      //       if ( ImGui::MenuItem( "Save Scene As..." ) ) {
      //          getLayout()->addView(
      //             std::make_shared< FileDialog >(
      //                "Create Project",
      //                []( const auto &path ) {
      //                   Editor::getInstance()->saveSceneAs( path );
      //                },
      //                ".crimild",
      //                Editor::getInstance()->getProject()->getScenesDirectory().string()
      //             )
      //          );
      //       }
      //    }
      // );

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
            if ( ImGui::MenuItem( "Close Project" ) ) {
               crimild::concurrency::sync_frame(
                  [] {
                     Editor::getInstance()->closeProject();
                  }
               );
            }
         }
      );

      ImGui::Separator();

      enabledWithProject(
         [ & ] {
            if ( ImGui::BeginMenu( "Import..." ) ) {
               if ( ImGui::MenuItem( "Wavefront OBJ (.obj)..." ) ) {
                  // openDialog(
                  //     "ImportOBJDlgKey",
                  //     "Import",
                  //     []( const auto &path ) {
                  //         // OBJLoader loader( path.string() );
                  //         // if ( auto model = loader.load() ) {
                  //         //     Editor::getInstance()->addToScene( model );
                  //         // } else {
                  //         //     CRIMILD_LOG_ERROR( "Cannot load model from file ", path.string() );
                  //         // }
                  //     },
                  //     ".obj",
                  //     "" // project->getAssetsDirectory().string()
                  // );
               }
               if ( ImGui::MenuItem( "glTF 2.0 (.gltf)..." ) ) {
                  // openDialog(
                  //     "ImportGLTFDlgKey",
                  //     "Import",
                  //     []( const auto &path ) {
                  //         // GLTFImporter importer;
                  //         // auto model = importer.import( path );
                  //         // if ( model != nullptr ) {
                  //         //     Editor::getInstance()->addToScene( model );
                  //         // } else {
                  //         //     CRIMILD_LOG_ERROR( "Cannot load model from file ", path.string() );
                  //         // }
                  //     },
                  //     ".gltf",
                  //     "" // project->getAssetsDirectory().string()
                  // );
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
}

template< typename WindowType >
void renderLayoutMenuItem( std::shared_ptr< Layout > const &layout, const char *title ) noexcept
{
   const bool existing = layout->hasViewWithTitle( title );
   if ( existing ) {
      ImGui::BeginDisabled();
   }
   if ( ImGui::MenuItem( title ) ) {
      layout->addView( crimild::alloc< WindowType >() );
   }
   if ( existing ) {
      ImGui::EndDisabled();
   }
}

void MainMenu::renderLayoutMenu( void ) noexcept
{
   if ( ImGui::BeginMenu( "Layout" ) ) {
      renderLayoutMenuItem< SceneWindow >( getLayout(), "Scene" );
      renderLayoutMenuItem< FileSystemWindow >( getLayout(), "File System" );
      renderLayoutMenuItem< InspectorWindow >( getLayout(), "Inspector" );
      ImGui::Separator();
      renderLayoutMenuItem< Scene3DWindow >( getLayout(), "Scene 3D" );
      renderLayoutMenuItem< SimulationWindow >( getLayout(), "Simulation" );
      renderLayoutMenuItem< SceneRTWindow >( getLayout(), "Scene RT" );
      ImGui::Separator();
      renderLayoutMenuItem< LogWindow >( getLayout(), "Log" );
      renderLayoutMenuItem< TimelineWindow >( getLayout(), "Timeline" );
      renderLayoutMenuItem< BehaviorsWindow >( getLayout(), "Behaviors" );
      renderLayoutMenuItem< GraphEditorWindow >( getLayout(), "Graph Editor" );
      renderLayoutMenuItem< PlaybackControlsWindow >( getLayout(), "Playback" );
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

void MainMenu::renderHelpMenu( void ) noexcept
{
   if ( ImGui::BeginMenu( "Help" ) ) {
      if ( ImGui::MenuItem( "About..." ) ) {
         getLayout()->addView( std::make_shared< AboutModal >() );
      }
      if ( ImGui::MenuItem( "ImGui Demo Window..." ) ) {
         getLayout()->addView( crimild::alloc< ImGuiDemoWindow >() );
      }
      ImGui::EndMenu();
   }
}
