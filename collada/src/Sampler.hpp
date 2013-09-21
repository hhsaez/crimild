#ifndef CRIMILD_COLLADA_SAMPLER_
#define CRIMILD_COLLADA_SAMPLER_

#include "Entity.hpp"
#include "EntityCatalog.hpp"
#include "Input.hpp"

namespace crimild {

	namespace collada {

		class Sampler : public Entity {
		public:
			Sampler( void );

			virtual ~Sampler( void );

			bool parseXML( xmlNode *input );

			inline InputLibrary *getInputLibrary( void ) { return &_inputLibrary; }

		private:
			InputLibrary _inputLibrary;
		};

		typedef std::shared_ptr< Sampler > SamplerPtr;

		class SamplerCatalog : public EntityCatalog< Sampler > {
		public:
			SamplerCatalog( void ) : EntityCatalog< Sampler >( COLLADA_SAMPLER ) { }
			virtual ~SamplerCatalog( void ) { }
		};

	}

}

#endif

