#ifndef CRIMILD_COLLADA_PROFILE_COMMON_
#define CRIMILD_COLLADA_PROFILE_COMMON_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "Technique.hpp"
#include "NewParam.hpp"

namespace crimild {

	namespace collada {

		class ProfileCommon : public Entity {
		public:
			ProfileCommon( void );
			virtual ~ProfileCommon( void );

			virtual bool parseXML( xmlNode *input ) override;

			Technique *getTechnique( void ) { return _technique.get(); }
			NewParamMap *getNewParams( void ) { return &_newparams; }

		private:
			Pointer< Technique > _technique;
			NewParamMap _newparams;
		};

	}

}

#endif

