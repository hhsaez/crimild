#include "systems/GLFWInputSystem.hpp"

#include <crimild/foundation/log/Log.hpp>
#include <crimild/universal/systems/DebugSystem.hpp>
#include <crimild/universal/systems/GLFWWindow.hpp>

using namespace crimild::universal::systems;
using namespace crimild::experimental::systems;

GLFWInputSystem::State GLFWInputSystem::onUpdate( const Context &ctx )
{
   if ( getState() == State::READY ) {
      CRIMILD_LOG_DEBUG( "Configuring input system" );
      if ( auto window = getParent< GLFWWindow >() ) {
         window->onKeyPressed.bind(
            []( auto keyInfo ) {
               switch ( keyInfo.key ) {
                  case CRIMILD_INPUT_KEY_D:
                  case CRIMILD_INPUT_KEY_RIGHT:
                     crimild::Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, +1.0f );
                     break;

                  case CRIMILD_INPUT_KEY_A:
                  case CRIMILD_INPUT_KEY_LEFT:
                     crimild::Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, -1.0f );
                     break;

                  case CRIMILD_INPUT_KEY_W:
                  case CRIMILD_INPUT_KEY_UP:
                     crimild::Input::getInstance()->setAxis( Input::AXIS_VERTICAL, +1.0f );
                     break;

                  case CRIMILD_INPUT_KEY_S:
                  case CRIMILD_INPUT_KEY_DOWN:
                     crimild::Input::getInstance()->setAxis( Input::AXIS_VERTICAL, -1.0f );
                     break;

                  case CRIMILD_INPUT_KEY_SPACE:
                     crimild::Input::getInstance()->setAxis( "fire", 1.0f );
                     break;

                  default:
                     break;
               }
            }
         );

         window->onKeyReleased.bind(
            []( auto keyInfo ) {
               switch ( keyInfo.key ) {
                  case CRIMILD_INPUT_KEY_ESCAPE:
                     // window.close();
                     break;

                  case CRIMILD_INPUT_KEY_A:
                  case CRIMILD_INPUT_KEY_D:
                  case CRIMILD_INPUT_KEY_LEFT:
                  case CRIMILD_INPUT_KEY_RIGHT:
                     crimild::Input::getInstance()->setAxis( Input::AXIS_HORIZONTAL, 0 );
                     break;

                  case CRIMILD_INPUT_KEY_W:
                  case CRIMILD_INPUT_KEY_S:
                  case CRIMILD_INPUT_KEY_UP:
                  case CRIMILD_INPUT_KEY_DOWN:
                     crimild::Input::getInstance()->setAxis( Input::AXIS_VERTICAL, 0 );
                     break;

                  case CRIMILD_INPUT_KEY_SPACE:
                     crimild::Input::getInstance()->setAxis( "fire", 0.0f );
                     break;

                  case CRIMILD_INPUT_KEY_TAB:
                     if ( auto debugSystem = DebugSystem::getInstance() ) {
                        debugSystem->toggleEnabled();
                     }
                     break;
               }
            }
         );

      } else {
         CRIMILD_LOG_ERROR( "No Window Found" );
         return State::FAILURE;
      }
      CRIMILD_LOG_DEBUG( "Input System ready" );
   }

   return State::SUCCESS;
}
