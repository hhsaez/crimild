#include "InputSystem.hpp"
#include "WindowSystem.hpp"

using namespace crimild;

InputSystem::InputSystem( void )
	: System( "Input System" )
{
    auto weakSelf = this;
    
    registerMessageHandler< messages::WindowSystemDidCreateWindow >( [weakSelf]( messages::WindowSystemDidCreateWindow const &message ) {
        weakSelf->_window = message.video->getWindowHandler();
        glfwGetWindowSize( weakSelf->_window, &( weakSelf->_windowWidth ), &( weakSelf->_windowHeight ) );
        
        InputState::getCurrentState().reset( GLFW_KEY_LAST, GLFW_MOUSE_BUTTON_LAST );
        
        crimild::async( AsyncDispatchPolicy::MAIN_QUEUE, std::bind( &InputSystem::update, weakSelf ) );
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
    CRIMILD_PROFILE( "Update Input State" )
    
    if ( _window == nullptr ) {
        return;
    }
    
    for ( int i = 0; i < GLFW_KEY_LAST; i++ ) {
        int keyState = glfwGetKey( _window, i );
        InputState::getCurrentState().setKeyState( i, keyState == GLFW_PRESS ? InputState::KeyState::PRESSED : InputState::KeyState::RELEASED );
    }
    
    double x, y;
    glfwGetCursorPos( _window, &x, &y );
    
    if ( InputState::getCurrentState().getMouseCursorMode() == InputState::MouseCursorMode::GRAB || ( x >= 0 && x < _windowWidth && y >= 0 && y < _windowHeight ) ) {
        InputState::getCurrentState().setMousePosition( Vector2i( x, y ) );
        InputState::getCurrentState().setNormalizedMousePosition( Vector2f( ( float ) x / float( _windowWidth - 1.0f ), ( float ) y / float( _windowHeight - 1.0f ) ) );
    }
    
    for ( int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_LAST; i++ ) {
        int buttonState = glfwGetMouseButton( _window, i );
        InputState::getCurrentState().setMouseButtonState( i, buttonState == GLFW_PRESS ? InputState::MouseButtonState::PRESSED : InputState::MouseButtonState::RELEASED );
    }
    
    switch ( InputState::getCurrentState().getMouseCursorMode() ) {
        case InputState::MouseCursorMode::NORMAL:
            glfwSetInputMode( _window, GLFW_CURSOR, GLFW_CURSOR_NORMAL );
            break;
            
        case InputState::MouseCursorMode::HIDDEN:
            glfwSetInputMode( _window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN );
            break;
            
        case InputState::MouseCursorMode::GRAB:
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

