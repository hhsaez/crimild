#ifndef CRIMILD_COLLADA_EFFECT_
#define CRIMILD_COLLADA_EFFECT_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "ProfileCommon.hpp"

namespace crimild {

	namespace collada {

		class Effect : public Entity {
		public:
			Effect( void );
			virtual ~Effect( void );

			virtual bool parseXML( xmlNode *input ) override;

			std::string getName( void ) const { return _name; }

			ProfileCommon *getProfileCommon( void ) { return _profileCommon.get(); }

		private:
			std::string _name;
			Pointer< ProfileCommon > _profileCommon;
		};

		class EffectMap : public EntityMap< collada::Effect > {
		public:
			EffectMap( void ) : EntityMap< collada::Effect >( COLLADA_EFFECT ) { }
			virtual ~EffectMap( void ) { }
		};

	}

}

#endif

