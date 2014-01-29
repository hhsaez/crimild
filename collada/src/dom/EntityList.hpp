#ifndef CRIMILD_COLLADA_LIBRARY_
#define CRIMILD_COLLADA_LIBRARY_

#include "Utils.hpp"
#include "Entity.hpp"

namespace crimild {

	namespace collada {

		template< class ENTITY_TYPE >
		class EntityList {
		public:
			virtual ~EntityList( void )
			{
				_entities.clear();
			}

			void attach( ENTITY_TYPE *entity )
			{
				_entities.push_back( entity );
			}

			unsigned int getCount( void ) const { return _entities.size(); }

			void foreach( std::function< void( ENTITY_TYPE * ) > callback )
			{
				for ( auto entity : _entities ) {
					callback( entity.get() );
				}
			}

			ENTITY_TYPE *get( std::string id )
			{
				for ( auto entity : _entities ) {
					if ( id == entity->getID() ) {
						return entity.get();
					}
				}

				return nullptr;
			}

			virtual bool parseXML( xmlNode *input )
			{
				Log::Debug << "Parsing " << _entityName << " library" << Log::End;
				for ( xmlNode *childXML = input->children; childXML != nullptr; childXML = childXML->next ) {
					if ( childXML->type == XML_ELEMENT_NODE ) {
						if ( XMLUtils::compareXMLNodeName( childXML, _entityName ) ) {
							Pointer< ENTITY_TYPE > entity( new ENTITY_TYPE() );
							if ( entity->parseXML( childXML) ) {
								attach( entity );
							}
						}
					}
				}

				return true;
			}

		protected:
			explicit EntityList( const char *entityName )
				: _entityName( entityName )
			{ 
			}

		private:
			const char *_entityName;
			std::list< Pointer< ENTITY_TYPE > > _entities;
		};

	}

}

#endif

