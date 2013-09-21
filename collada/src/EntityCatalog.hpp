#ifndef CRIMILD_COLLADA_ENTITY_CATALOG_
#define CRIMILD_COLLADA_ENTITY_CATALOG_

#include "Utils.hpp"

#include <map>

namespace crimild {

	namespace collada {

		template< class ENTITY_TYPE >
		class EntityCatalog {
		public:
			virtual ~EntityCatalog( void )
			{
				_entities.clear();
			}

			void attachEntity( std::shared_ptr< ENTITY_TYPE > entity )
			{
				_entities[ entity->getID() ] = entity;
			}

			ENTITY_TYPE *getEntityWithID( std::string id )
			{
				return _entities[ id ].get();
			}

			virtual bool parseXML( xmlNode *input )
			{
				Log::Debug << "Parsing " << _entityName << " catalog" << Log::End;
				for ( xmlNode *childXML = input->children; childXML != nullptr; childXML = childXML->next ) {
					if ( childXML->type == XML_ELEMENT_NODE ) {
						if ( XMLUtils::compareXMLNodeName( childXML, _entityName ) ) {
							std::shared_ptr< ENTITY_TYPE > entity( new ENTITY_TYPE() );
							if ( entity->parseXML( childXML ) ) {
								attachEntity( entity );
							}
						}
					}
				}

				return true;
			}

		protected:
			explicit EntityCatalog( const char *entityName )
				: _entityName( entityName )
			{
			}

		private:
			const char *_entityName;
			std::map< std::string, std::shared_ptr< ENTITY_TYPE > > _entities;
		};

	}

}

#endif

