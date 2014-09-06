#include "RenderDebugInfoTask.hpp"

#include "Simulation/RunLoop.hpp"
#include "Simulation/Simulation.hpp"

#include "Rendering/Renderer.hpp"

#include "Visitors/Apply.hpp"

#include "Debug/DebugRenderHelper.hpp"

using namespace crimild;

RenderDebugInfoTask::RenderDebugInfoTask( int priority )
	: Task( priority )
{

}

RenderDebugInfoTask::~RenderDebugInfoTask( void )
{

}

void RenderDebugInfoTask::start( void )
{
	DebugRenderHelper::init();

	if ( getRunLoop() != nullptr ) {
		getRunLoop()->suspendTask( this );
	}
}

void RenderDebugInfoTask::update( void )
{
	Renderer *renderer = Simulation::getCurrent()->getRenderer();
	
	Simulation::getCurrent()->forEachCamera( [&]( Camera *camera ) {

		Simulation::getCurrent()->getScene()->perform( Apply( [&]( Node *node ) {
			node->foreachComponent( [&]( NodeComponent *component ) {
				component->renderDebugInfo( renderer, camera );
			});
		}));

	});
}

void RenderDebugInfoTask::stop( void )
{

}

void RenderDebugInfoTask::handleMessage( EnableRenderDebugInfoMessage *message )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->resumeTask( this );
	}
}

void RenderDebugInfoTask::handleMessage( DisableRenderDebugInfoMessage *message )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->suspendTask( this );
	}
}

