#ifndef CRIMILD_COLLADA_LIBRARY_
#define CRIMILD_COLLADA_LIBRARY_

#include "Utils.hpp"

namespace crimild {

	namespace collada {

		template< class ENTITY_TYPE >
		class EntityLibrary {
		private:
			typedef std::shared_ptr< ENTITY_TYPE > EntityPtr;

		public:
			virtual ~EntityLibrary( void )
			{
				_entities.clear();
			}

			void attachEntity( EntityPtr entity )
			{
				_entities.push_back( entity );
			}

			unsigned int getEntityCount( void ) const { return _entities.size(); }

			void foreachEntity( std::function< void( EntityPtr ) > callback )
			{
				for ( auto entity : _entities ) {
					callback( entity );
				}
			}

			ENTITY_TYPE *getEntityWithID( std::string id )
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
							EntityPtr entity( new ENTITY_TYPE() );
							if ( entity->parseXML( childXML) ) {
								attachEntity( entity );
							}
						}
					}
				}

				return true;
			}

		protected:
			explicit EntityLibrary( const char *entityName )
				: _entityName( entityName )
			{ 
			}

		private:
			const char *_entityName;
			std::list< EntityPtr > _entities;
		};

	}

}

#endif

