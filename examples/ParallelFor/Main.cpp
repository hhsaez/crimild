#include <Crimild.hpp>

using namespace crimild;

SharedPointer< Geometry > createSphere( const Vector3f &center, float radius )
{
    auto geometry = crimild::alloc< Geometry >();
    geometry->setLocalBound( crimild::alloc< SphereBoundingVolume >() );
    geometry->setWorldBound( crimild::alloc< SphereBoundingVolume >() );
    geometry->attachPrimitive( crimild::alloc< SpherePrimitive >( radius ) );
    geometry->local().setTranslate( center );
    return geometry;
}

int main( int argc, char **argv )
{
    for ( int i = 0; i < 10; i++ ) {
        crimild::concurrency::JobScheduler jobScheduler;

        jobScheduler.configure();
        jobScheduler.start();

        auto parent = crimild::concurrency::async();

        std::atomic< int > counter( 0 );
        
        auto scene = crimild::alloc< crimild::Group >();
        for ( int i = 0; i < 1; i++ ) {
            scene->attachNode( createSphere( Vector3f::ZERO, 1.0f ) );
        }

        for ( int i = 0; i < 400; i++ ) {
            auto child = crimild::concurrency::async( parent, [&counter]() {
                counter++;
            });
        }

        crimild::concurrency::wait( parent );
        
        jobScheduler.stop();

        std::cout << "Counter: " << counter << std::endl;
    }

	return 0;
}

