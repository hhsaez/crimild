#ifndef CRIMILD_COLLADA_SOURCE_
#define CRIMILD_COLLADA_SOURCE_

#include "Entity.hpp"
#include "EntityCatalog.hpp"

namespace crimild {

	namespace collada {

		class Source : public Entity {
		public:
			Source( void );

			virtual ~Source( void );

			bool parseXML( xmlNode *input );

			inline unsigned int getCount( void ) const { return _count; }
			inline const float *getFloatArray( void ) const { return &_floatArray[ 0 ]; }
			//inline const bool *getBoolArray( void ) const { return &_boolArray[ 0 ]; }
			//inline const int *getIntArray( void ) const { return &_intArray[ 0 ]; }
			inline const std::string *getNameArray( void ) const { return &_nameArray[ 0 ]; }

		private:
			unsigned int _count;
			std::vector< float > _floatArray;
			std::vector< bool > _boolArray;
			std::vector< int > _intArray;
			std::vector< std::string > _nameArray;
		};

		typedef std::shared_ptr< Source > SourcePtr;

		class SourceCatalog : public EntityCatalog< Source > {
		public:
			SourceCatalog( void ) : EntityCatalog< Source >( COLLADA_SOURCE ) { }
			virtual ~SourceCatalog( void ) { }
		};

	}

}

#endif

