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
		getRunLoop()->suspendTask( getShared< RenderDebugInfoTask >() );
	}
}

void RenderDebugInfoTask::update( void )
{
	auto renderer = Simulation::getCurrent()->getRenderer();
	
	Simulation::getCurrent()->forEachCamera( [&]( CameraPtr const &camera ) {

		Simulation::getCurrent()->getScene()->perform( Apply( [&]( NodePtr const &node ) {
			node->foreachComponent( [&]( NodeComponentPtr const &component ) {
				component->renderDebugInfo( renderer, camera );
			});
		}));

	});
}

void RenderDebugInfoTask::stop( void )
{

}

void RenderDebugInfoTask::handleMessage( EnableRenderDebugInfoMessagePtr const &message )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->resumeTask( getShared< RenderDebugInfoTask >() );
	}
}

void RenderDebugInfoTask::handleMessage( DisableRenderDebugInfoMessagePtr const &message )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->suspendTask( getShared< RenderDebugInfoTask>() );
	}
}

