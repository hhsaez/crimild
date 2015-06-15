#include "InputSystem.hpp"
#include "WindowSystem.hpp"

using namespace crimild;

InputSystem::InputSystem( void )
	: System( "Input" )
{
	enableUpdater();

    registerMessageHandler< messages::WindowSystemDidCreateWindow >( [&]( messages::WindowSystemDidCreateWindow const &message ) {
        _window = message.video->getWindowHandler();
        glfwGetWindowSize( _window, &_windowWidth, &_windowHeight);
        
        InputState::getCurrentState().reset( GLFW_KEY_LAST, GLFW_MOUSE_BUTTON_LAST );
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
	System::update();
    
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
    
    if ( x >= 0 && x < _windowWidth && y >= 0 && y < _windowHeight ) {
        InputState::getCurrentState().setMousePosition( Vector2i( x, y ) );
        InputState::getCurrentState().setNormalizedMousePosition( Vector2f( ( float ) x / float( _windowWidth - 1.0f ), ( float ) y / float( _windowHeight - 1.0f ) ) );
    }
    
    for ( int i = GLFW_MOUSE_BUTTON_1; i < GLFW_MOUSE_BUTTON_LAST; i++ ) {
        int buttonState = glfwGetMouseButton( _window, i );
        InputState::getCurrentState().setMouseButtonState( i, buttonState == GLFW_PRESS ? InputState::MouseButtonState::PRESSED : InputState::MouseButtonState::RELEASED );
    }
}

void InputSystem::stop( void )
{
	System::stop();
}

