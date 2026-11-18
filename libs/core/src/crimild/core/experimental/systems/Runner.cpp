#include <crimild/core/experimental/systems/Runner.hpp>
#include <crimild/core/init.hpp>
#include <crimild/foundation/common/Version.hpp>
#include <crimild/foundation/log/Log.hpp>
#include <crimild/foundation/log/LogOutputHandler.hpp>
#include <filesystem>
#include <memory>

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
   detachAll();

   CRIMILD_LOG_INFO( "Terminated" );
}

System::State Runner::onUpdate( const Context &ctx )
{
   while ( true ) {
      auto ret = System::onUpdate( ctx );
      if ( ret != State::RUNNING ) {
         return ret;
      }
   }
   return State::SUCCESS;
}
