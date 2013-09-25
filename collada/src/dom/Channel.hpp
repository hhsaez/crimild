#ifndef CRIMILD_COLLADA_CHANNEL_
#define CRIMILD_COLLADA_CHANNEL_

#include "Entity.hpp"
#include "EntityList.hpp"
#include "Sampler.hpp"

namespace crimild {

	namespace collada {

		class Sampler;

		class Channel : public Entity {
		public:
			Channel( void );

			virtual ~Channel( void );

			bool parseXML( xmlNode *node );

			inline const char *getSource( void ) const { return _source.c_str(); }
			inline const char *getTarget( void ) const { return _target.c_str(); }

			inline void setSamplerRef( Sampler *sampler ) { _sampler = sampler; }
			inline Sampler *getSamplerRef( void ) { return _sampler; }

		private:
			std::string _source;
			std::string _target;
			Sampler *_sampler;
		};

		typedef std::shared_ptr< Channel > ChannelPtr;

		class ChannelList : public EntityList< Channel > {
		public:
			ChannelList( void ) : EntityList< Channel >( COLLADA_CHANNEL ) { }
			virtual ~ChannelList( void ) { }
		};

	}

}

#endif

