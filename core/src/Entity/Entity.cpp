#include "Entity/Entity.hpp"

using namespace crimild;

void Entity::encode( coding::Encoder &encoder )
{
   coding::Codable::encode( encoder );

   Array< std::shared_ptr< Extension > > es;
   for ( auto &it : m_extensions ) {
      if ( it.second != nullptr ) {
         es.add( it.second );
      }
   }
   encoder.encode( "extensions", es );
}

void Entity::decode( coding::Decoder &decoder )
{
   coding::Codable::decode( decoder );

   Array< std::shared_ptr< Extension > > es;
   decoder.decode( "extensions", es );
   es.each(
      [ this ]( auto &e ) {
         attach( e );
      }
   );
}
