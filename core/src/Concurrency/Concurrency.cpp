#include "Concurrency.hpp"
#include "JobScheduler.hpp"

using namespace crimild;
using namespace crimild::concurrency;

SharedPointer< Job > crimild::concurrency::async( SharedPointer< Job > const &job )
{
	JobScheduler::getInstance()->schedule( job );
	return job;
}

SharedPointer< Job > crimild::concurrency::async( JobCallback const &callback )
{
	auto job = crimild::alloc< Job >( callback );
	JobScheduler::getInstance()->schedule( job );
	return job;
}

SharedPointer< Job > crimild::concurrency::async( SharedPointer< Job > const &parent, JobCallback const &callback )
{
	auto child = crimild::alloc< Job >( crimild::get_ptr( parent ), callback );
	JobScheduler::getInstance()->schedule( child );
	return child;
}

SharedPointer< Job > crimild::concurrency::sync_frame( void )
{
	// TODO
	return nullptr;
}

SharedPointer< Job > crimild::concurrency::async_frame( void )
{
	// TODO
	return nullptr;
}

void crimild::concurrency::wait( SharedPointer< Job > const &job )
{
	JobScheduler::getInstance()->wait( job );
}

