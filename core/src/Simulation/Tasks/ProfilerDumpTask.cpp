#include "ProfilerDumpTask.hpp"

#include "Foundation/Profiler.hpp"

#include "Simulation/RunLoop.hpp"
#include "Simulation/Simulation.hpp"

#include "Rendering/Renderer.hpp"

using namespace crimild;

ProfilerDumpTask::ProfilerDumpTask( int priority )
	: Task( priority )
{

}

ProfilerDumpTask::~ProfilerDumpTask( void )
{

}

void ProfilerDumpTask::start( void )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->suspendTask( getShared< ProfilerDumpTask >() );
	}
}

void ProfilerDumpTask::update( void )
{
	Profiler::getInstance()->dump();

	static double accum = 0.0;
	
	auto t = Simulation::getInstance()->getSimulationTime();
	accum += t.getDeltaTime() * 100.0;
	if ( accum >= 1.0 ) {
		Profiler::getInstance()->resetAll();
		accum = 0.0;
	}
}

void ProfilerDumpTask::stop( void )
{

}

void ProfilerDumpTask::handleMessage( EnableProfilerDumpMessagePtr const &message )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->resumeTask( getShared< ProfilerDumpTask >() );
	}
}

void ProfilerDumpTask::handleMessage( DisableProfilerDumpMessagePtr const &message )
{
	if ( getRunLoop() != nullptr ) {
		getRunLoop()->suspendTask( getShared< ProfilerDumpTask>() );
	}
}

