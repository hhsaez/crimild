#include "InputSystem.hpp"
#include "WindowSystem.hpp"

using namespace crimild;

InputSystem::InputSystem( void )
	: System( "Input System" )
{
    auto self = this;
    
    registerMessageHandler< messages::WindowSystemDidCreateWindow >( [self]( messages::WindowSystemDidCreateWindow const &message ) {
        self->_window = message.video->getWindowHandler();
        
        Input::getInstance()->reset( GLFW_KEY_LAST, GLFW_MOUSE_BUTTON_LAST );
        
        glfwSetKeyCallback( self->_window, []( GLFWwindow* window, int key, int scancode, int action, int mods ) {
            if ( action == GLFW_PRESS || action == GLFW_REPEAT ) {
                MessageQueue::getInstance()->pushMessage( messaging::KeyPressed { key } );
            }
            else {
                MessageQueue::getInstance()->pushMessage( messaging::KeyReleased { key } );
            }
        });
        
        glfwSetCursorPosCallback( self->_window, []( GLFWwindow* window, double xpos, double ypos ) {
            int windowWidth, windowHeight;
            glfwGetWindowSize( window, &windowWidth, &windowHeight );
            
            MessageQueue::getInstance()->pushMessage( messaging::MouseMotion {
                ( float ) xpos,
                ( float ) ypos,
                ( float ) xpos / ( float ) windowWidth,
                ( float ) ypos / ( float ) windowHeight
            });
        });
        
        glfwSetMouseButtonCallback( self->_window, []( GLFWwindow* window, int button, int action, int mods ) {
            double x, y;
            glfwGetCursorPos( window, &x, &y );
            
            if ( action == GLFW_PRESS ) {
                MessageQueue::getInstance()->pushMessage( messaging::MouseButtonDown { button, ( float ) x, ( float ) y } );
            }
            else {
                MessageQueue::getInstance()->pushMessage( messaging::MouseButtonUp { button, ( float ) x, ( float ) y } );
            }
        });
        
        glfwSetScrollCallback( self->_window, []( GLFWwindow* window, double xoffset, double yoffset ) {
            MessageQueue::getInstance()->pushMessage( messaging::MouseScroll { ( float ) xoffset, ( float ) yoffset } );
        });
        
        crimild::async( AsyncDispatchPolicy::MAIN_QUEUE, std::bind( &InputSystem::update, self ) );
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
    
    crimild::async( AsyncDispatchPolicy::MAIN_QUEUE, std::bind( &InputSystem::update, this ) );
}

void InputSystem::stop( void )
{
	System::stop();
}

