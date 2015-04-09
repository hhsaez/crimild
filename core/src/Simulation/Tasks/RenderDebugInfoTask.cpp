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
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( RenderDebugInfoTask::Messages::Enable, RenderDebugInfoTask, enable );
	CRIMILD_BIND_MEMBER_MESSAGE_HANDLER( RenderDebugInfoTask::Messages::Disable, RenderDebugInfoTask, disable );
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
	auto renderer = Simulation::getInstance()->getRenderer();
	
	Simulation::getInstance()->forEachCamera( [&]( CameraPtr const &camera ) {

		Simulation::getInstance()->getScene()->perform( Apply( [&]( NodePtr const &node ) {
			node->foreachComponent( [&]( NodeComponentPtr const &component ) {
				component->renderDebugInfo( renderer, camera );
			});
		}));

	});
}

void RenderDebugInfoTask::stop( void )
{

}

void RenderDebugInfoTask::enable( RenderDebugInfoTask::Messages::Enable const &message )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->resumeTask( getShared< RenderDebugInfoTask >() );
	}
}

void RenderDebugInfoTask::disable( RenderDebugInfoTask::Messages::Disable const &message )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->suspendTask( getShared< RenderDebugInfoTask>() );
	}
}

