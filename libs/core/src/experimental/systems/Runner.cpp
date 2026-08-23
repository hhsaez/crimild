#include <crimild/core/experimental/systems/Runner.hpp>
#include <crimild/core/init.hpp>
#include <filesystem>

using namespace crimild::experimental::systems;

Runner::Runner( int argc, char **argv ) noexcept
{
   crimild::init();
   CRIMILD_LOG_INFO( crimild::Version().getFullDescription() );

   std::filesystem::current_path( std::filesystem::path( argv[ 0 ] ).parent_path() );

   crimild::Log::setOutputHandlers(
      {
         std::make_shared< ConsoleOutputHandler >( Log::LOG_LEVEL_ALL, true ),
      }
   );
}

Runner::~Runner( void ) noexcept
{
   CRIMILD_LOG_INFO( "Terminated" );
}

Runner::State Runner::onExecute( void )
{
   auto behavior = getBehavior();
   if ( behavior == nullptr ) {
      return State::FAILURE;
   }

   State ret;
   do {
      ret = behavior->execute();
   } while ( ret != State::FAILURE );
   return ret;
}
