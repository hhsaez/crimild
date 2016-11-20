#include "UISystem.hpp"

#include "Simulation/Input.hpp"
#include "Simulation/Simulation.hpp"

#include "Visitors/Apply.hpp"

#include "Components/UIResponder.hpp"

using namespace crimild;
using namespace crimild::messaging;

UISystem::UISystem( void )
    : System( "UI System" )
{

}

UISystem::~UISystem( void )
{

}

bool UISystem::start( void )
{
    return System::start();

    // TODO: This should be a click, not just a mouse button up   
    registerMessageHandler< MouseButtonUp >( []( MouseButtonUp const &msg ) {
        if ( msg.button == CRIMILD_INPUT_MOUSE_BUTTON_LEFT ) {
            Vector2f mousePos = Input::getInstance()->getNormalizedMousePosition();
            Ray3f ray;
            
            auto scene = Simulation::getInstance()->getScene();
            auto camera = Simulation::getInstance()->getMainCamera();
            
            if ( camera->getPickRay( mousePos[ 0 ], mousePos[ 1 ], ray ) ) {
                float minJ = -1.0f;
                Node *result = nullptr;
                scene->perform( Apply( [&]( Node *node ) {
                    auto responder = node->getComponent< UIResponder >();
                    if ( responder == nullptr ) {
                        return;
                    }
                    
                    if ( !responder->isEnabled() ) {
                        return ;
                    }
                    
                    if ( responder->getBoundingVolume()->testIntersection( ray ) ) {
                        float j = Distance::computeSquared( node->getWorldBound()->getCenter(), camera->getWorld().getTranslate() );
                        if ( minJ < 0.0f || j < minJ ) {
                            minJ = j;
                            result = node;
                        }
                    }
                }));
                
                if ( result != nullptr ) {
                    result->getComponent< UIResponder >()->invoke();
                }

                // TODO: cancel message broadcasting
            }
        }
    });
}

void UISystem::stop( void )
{
    System::stop();

    unregisterMessageHandler< MouseButtonUp >();
}

