#ifndef CRIMILD_COLLADA_SAMPLER_
#define CRIMILD_COLLADA_SAMPLER_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "Input.hpp"

namespace crimild {

	namespace collada {

		class Sampler : public Entity {
		public:
			Sampler( void );

			virtual ~Sampler( void );

			bool parseXML( xmlNode *input );

			inline InputList *getInputs( void ) { return &_inputs; }

		private:
			InputList _inputs;
		};

		typedef std::shared_ptr< Sampler > SamplerPtr;

		class SamplerMap : public EntityMap< Sampler > {
		public:
			SamplerMap( void ) : EntityMap< Sampler >( COLLADA_SAMPLER ) { }
			virtual ~SamplerMap( void ) { }
		};

	}

}

#endif

