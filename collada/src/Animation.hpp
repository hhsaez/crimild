#ifndef CRIMILD_COLLADA_ANIMATION_
#define CRIMILD_COLLADA_ANIMATION_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Source.hpp"
#include "Sampler.hpp"
#include "Channel.hpp"

namespace crimild {

	namespace collada {

		class Animation : public Entity {
		public:
			Animation( void );

			virtual ~Animation( void );

			bool parseXML( xmlNode *input );

			inline ChannelLibrary *getChannelLibrary( void ) { return &_channels; }

		private:
			SourceCatalog _sources;
			SamplerCatalog _samplers;
			ChannelLibrary _channels;
		};

		typedef std::shared_ptr< Animation > AnimationPtr;

		class AnimationLibrary : public EntityLibrary< Animation > {
		public:
			AnimationLibrary( void ) : EntityLibrary< Animation >( COLLADA_ANIMATION ) { }
			virtual ~AnimationLibrary( void ) { }
		};

	}

}

#endif

