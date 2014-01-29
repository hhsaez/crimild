#ifndef CRIMILD_COLLADA_TECHNIQUE_COMMON_
#define CRIMILD_COLLADA_TECHNIQUE_COMMON_

#include "Entity.hpp"
#include "Accessor.hpp"

namespace crimild {

	namespace collada {

		class TechniqueCommon : public Entity {
		public:
			TechniqueCommon( void );

			virtual ~TechniqueCommon( void );

			virtual bool parseXML( xmlNode *input ) override;

			Accessor *getAccessor( void ) { return _accessor.get(); }

		private:
			Pointer< Accessor > _accessor;
		};

	}

}

#endif

