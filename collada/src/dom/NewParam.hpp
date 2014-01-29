#ifndef CRIMILD_COLLADA_NEWPARAM_
#define CRIMILD_COLLADA_NEWPARAM_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "Sampler2D.hpp"
#include "Surface.hpp"

namespace crimild {

	namespace collada {

		class NewParam : public Entity {
		public:
			NewParam( void );

			virtual ~NewParam( void );

			virtual bool parseXML( xmlNode *input ) override;

			Sampler2D *getSampler2D( void ) { return _sampler2D.get(); }
			Surface *getSurface( void ) { return _surface.get(); }
			std::string getSID( void ) const { return _sid; }

		private:
			Pointer< Sampler2D > _sampler2D;
			Pointer< Surface > _surface;
			std::string _sid;
		};

		class NewParamMap : public EntityMap< NewParam > {
		public:
			NewParamMap( void ) : EntityMap< NewParam >( COLLADA_NEWPARAM ) { }
			virtual ~NewParamMap( void ) { }
		};

	}

}

#endif

