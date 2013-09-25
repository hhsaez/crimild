#ifndef CRIMILD_COLLADA_ENTITY_CATALOG_
#define CRIMILD_COLLADA_ENTITY_CATALOG_

#include "Utils.hpp"

#include <map>

namespace crimild {

	namespace collada {

		template< class ENTITY_TYPE >
		class EntityMap {
		public:
			virtual ~EntityMap( void )
			{
				_entities.clear();
			}

			void attach( std::shared_ptr< ENTITY_TYPE > entity )
			{
				_entities[ entity->getID() ] = entity;
			}

			ENTITY_TYPE *get( std::string id )
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
								attach( entity );
							}
						}
					}
				}

				return true;
			}

		protected:
			explicit EntityMap( const char *entityName )
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

