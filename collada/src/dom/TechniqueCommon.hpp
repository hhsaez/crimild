#ifndef CRIMILD_COLLADA_TECHNIQUE_COMMON_
#define CRIMILD_COLLADA_TECHNIQUE_COMMON_

#include "Entity.hpp"
#include "EntityList.hpp"

namespace crimild {

	namespace collada {

		// class Param : public Entity {
		// public:
		// 	Param( void );
		// 	virtual ~Param( void );

		// 	bool parseXML( xmlNode *node );

		// private:
		// 	std::string _name;
		// 	std::string _type;
		// };

		// std::shared_ptr< Param > ParamPtr;

		// class ParamLibrary : public EntityList< Param > {
		// public:
		// 	ParamLibrary( void ) : EntityList< Param >( COLLADA_PARAM ) { }
		// 	virtual ~ParamLibrary( void ) { }
		// };

		// class Accessor : public Entity {
		// public:
		// };

		// typedef std::shared_ptr< Accessor > AccessorPtr;

		class TechniqueCommon : public Entity {
		public:
			TechniqueCommon( void );

			virtual ~TechniqueCommon( void );

			bool parseXML( xmlNode *input );

		private:
			unsigned int _count;
			unsigned int _stride;
		};

		typedef std::shared_ptr< TechniqueCommon > TechniqueCommonPtr;

	}

}

#endif

