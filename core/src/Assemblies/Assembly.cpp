#include "Assemblies/Assembly.hpp"

using namespace crimild;

Assembly::Assembly( std::string_view name ) noexcept
   : Named( name )
{
   // no-op
}

void Assembly::addEntity( std::shared_ptr< Entity > const &entity )
{
   auto it = std::find( m_entities.begin(), m_entities.end(), entity );
   if ( it == m_entities.end() ) {
      m_entities.push_back( entity );
   }
}

void Assembly::removeEntity( std::shared_ptr< Entity > const &entity )
{
   auto it = std::find( m_entities.begin(), m_entities.end(), entity );
   if ( it != m_entities.end() ) {
      m_entities.erase( it );
   }
}

void Assembly::encode( coding::Encoder &encoder ) noexcept
{
   Entity::encode( encoder );

   encoder.encode( "name", getName() );

   Array< std::shared_ptr< Entity > > entities;
   for ( auto &entity : m_entities ) {
      entities.add( entity );
   }
   encoder.encode( "entities", entities );
}

void Assembly::decode( coding::Decoder &decoder ) noexcept
{
   Entity::decode( decoder );

   decoder.decode( "name", getNameRefForCoding() );

   Array< std::shared_ptr< Entity > > entities;
   decoder.decode( "entities", entities );
   entities.each(
      [ this ]( auto entity ) {
         addEntity( entity );
      }
   );
}
