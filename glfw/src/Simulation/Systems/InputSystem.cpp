#include "InputSystem.hpp"
#include "WindowSystem.hpp"

#include <Concurrency/Async.hpp>
#include <Foundation/Profiler.hpp>
#include <Simulation/Input.hpp>
#include <Simulation/Console/Console.hpp>

using namespace crimild;

InputSystem::InputSystem( void )
	: System( "Input System" )
{
    auto self = this;
    
    registerMessageHandler< messages::WindowSystemDidCreateWindow >( [self]( messages::WindowSystemDidCreateWindow const &message ) {
        self->_window = message.video->getWindowHandler();
        
        Input::getInstance()->reset( GLFW_KEY_LAST, GLFW_MOUSE_BUTTON_LAST );
        
        glfwSetKeyCallback( self->_window, []( GLFWwindow* window, int key, int scancode, int action, int mods ) {
			if ( key == GLFW_KEY_UNKNOWN ) {
				return;
			}
			
            if ( Console::getInstance()->isEnabled() ) {
                if ( action == GLFW_PRESS ) {
                    switch ( key ) {
                        case CRIMILD_INPUT_KEY_BACKSPACE:
                        case CRIMILD_INPUT_KEY_UP:
                        case CRIMILD_INPUT_KEY_DOWN:
                        case CRIMILD_INPUT_KEY_ENTER:
                            Console::getInstance()->handleInput( key, mods );
                            break;

                        default:
                            break;
                    }
                }

                if ( Console::getInstance()->isActive() ) {
                    return;
                }
            }

            if ( action == GLFW_PRESS || action == GLFW_REPEAT ) {
                MessageQueue::getInstance()->pushMessage( messaging::KeyPressed { key } );
            }
            else {
                MessageQueue::getInstance()->pushMessage( messaging::KeyReleased { key } );
            }
        });

        glfwSetCharCallback( self->_window, []( GLFWwindow *window, unsigned int codepoint ) {
            if ( Console::getInstance()->isEnabled() ) {
                Console::getInstance()->handleInput( codepoint, 0 );
            }
        });
        
        glfwSetMouseButtonCallback( self->_window, []( GLFWwindow* window, int button, int action, int mods ) {
            double x, y;
            glfwGetCursorPos( window, &x, &y );
            
            if ( action == GLFW_PRESS ) {
                MessageQueue::getInstance()->pushMessage( messaging::MouseButtonDown { button } );
            }
            else {
                MessageQueue::getInstance()->pushMessage( messaging::MouseButtonUp { button } );
            }
        });
        
        glfwSetScrollCallback( self->_window, []( GLFWwindow* window, double xoffset, double yoffset ) {
            MessageQueue::getInstance()->pushMessage( messaging::MouseScroll { ( float ) xoffset, ( float ) yoffset } );
        });
        
        crimild::concurrency::sync_frame( std::bind( &InputSystem::update, self ) );
    });
    
    registerMessageHandler< messages::WindowSystemWillDestroyWindow >( [&]( messages::WindowSystemWillDestroyWindow const &message ) {
        _window = nullptr;
    });
}

InputSystem::~InputSystem( void )
{

}

bool InputSystem::start( void )
{	
	if ( !System::start() ) {
		return false;
	}
    
    return true;
}

void InputSystem::update( void )
{
    CRIMILD_PROFILE( "Update Input" )
    
    if ( _window == nullptr ) {
        return;
    }
    
    switch ( Input::getInstance()->getMouseCursorMode() ) {
        case Input::MouseCursorMode::NORMAL:
            glfwSetInputMode( _window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
            break;
            
        case Input::MouseCursorMode::HIDDEN:
            glfwSetInputMode( _window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
            break;
            
        case Input::MouseCursorMode::GRAB:
            glfwSetInputMode( _window, GLFW_CURSOR, GLFW_CURSOR_DISABLED );
            break;
            
        default:
            break;
    }

    // trigger MouseMotion in every update to handle cases
    // when the mouse is not moving and the delta pos should
    // be updated 
    int windowWidth, windowHeight;
    double x, y;
    glfwGetWindowSize( _window, &windowWidth, &windowHeight );
    glfwGetCursorPos( _window, &x, &y );
    
    MessageQueue::getInstance()->pushMessage( messaging::MouseMotion {
        ( float ) x,
        ( float ) y,
        ( float ) x / ( float ) windowWidth,
        ( float ) y / ( float ) windowHeight
    });

	std::vector< float > axes;
	auto joyPresent = glfwJoystickPresent(GLFW_JOYSTICK_1);
	if (joyPresent == 1) {
		int axesCount;
		const float *axesData = glfwGetJoystickAxes( GLFW_JOYSTICK_1, &axesCount );
		for ( int i = 0; i < axesCount; i++ ) {
			axes.push_back( axesData[ i ] );
		}
	}
	Input::getInstance()->resetJoystickAxes( axes );
    
    crimild::concurrency::sync_frame( std::bind( &InputSystem::update, this ) );
}

void InputSystem::stop( void )
{
	System::stop();
}

