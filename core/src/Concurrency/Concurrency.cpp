#include "Concurrency.hpp"
#include "JobScheduler.hpp"

using namespace crimild;
using namespace crimild::concurrency;

JobPtr crimild::concurrency::async( void )
{
//	auto job = JobAllocator::getInstance()->allocate();
    auto job = crimild::alloc< Job >();
	job->reset();
	return job;
}

JobPtr crimild::concurrency::async( JobCallback const &callback )
{
//	auto job = JobAllocator::getInstance()->allocate();
    auto job = crimild::alloc< Job >();
	job->reset( callback );
	JobScheduler::getInstance()->schedule( job );
	return job;
}

JobPtr crimild::concurrency::async( JobPtr const &parent, JobCallback const &callback )
{
//	auto child = JobAllocator::getInstance()->allocate();
    auto child = crimild::alloc< Job >();
	child->reset( parent, callback );
	JobScheduler::getInstance()->schedule( child );
	return child;
}

JobPtr crimild::concurrency::sync_frame( void )
{
	// TODO
	return nullptr;
}

JobPtr crimild::concurrency::async_frame( void )
{
	// TODO
	return nullptr;
}

void crimild::concurrency::wait( JobPtr const &job )
{
	JobScheduler::getInstance()->wait( job );
}

