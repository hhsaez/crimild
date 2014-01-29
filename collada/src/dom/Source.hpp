#ifndef CRIMILD_COLLADA_SOURCE_
#define CRIMILD_COLLADA_SOURCE_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "TechniqueCommon.hpp"

namespace crimild {

	namespace collada {

		class Source : public Entity {
		public:
			Source( void );

			virtual ~Source( void );

			bool parseXML( xmlNode *input );

			unsigned int getCount( void ) const { return _count; }
			const float *getFloatArray( void ) const { return &_floatArray[ 0 ]; }
			//const bool *getBoolArray( void ) const { return &_boolArray[ 0 ]; }
			//const int *getIntArray( void ) const { return &_intArray[ 0 ]; }
			const std::string *getNameArray( void ) const { return &_nameArray[ 0 ]; }

			TechniqueCommon *getTechniqueCommon( void ) { return _techniqueCommon.get(); }

		private:
			unsigned int _count;
			std::vector< float > _floatArray;
			std::vector< bool > _boolArray;
			std::vector< int > _intArray;
			std::vector< std::string > _nameArray;
			Pointer< TechniqueCommon > _techniqueCommon;
		};

		class SourceMap : public EntityMap< Source > {
		public:
			SourceMap( void ) : EntityMap< Source >( COLLADA_SOURCE ) { }
			virtual ~SourceMap( void ) { }
		};

	}

}

#endif

