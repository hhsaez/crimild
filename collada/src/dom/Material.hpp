#ifndef CRIMILD_COLLADA_MATERIAL_
#define CRIMILD_COLLADA_MATERIAL_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "Instance.hpp"

namespace crimild {

	namespace collada {

		class Material : public Entity {
		public:
			Material( void );
			virtual ~Material( void );

			virtual bool parseXML( xmlNode *input ) override;

			Instance *getInstanceEffect( void ) { return _instanceEffect.get(); }
			std::string getName( void ) const { return _name; }

		private:
			Pointer< Instance > _instanceEffect;
			std::string _name;
		};

		class MaterialMap : public EntityMap< collada::Material > {
		public:
			MaterialMap( void ) : EntityMap< collada::Material >( COLLADA_MATERIAL ) { }
			virtual ~MaterialMap( void ) { }
		};

	}

}

#endif

