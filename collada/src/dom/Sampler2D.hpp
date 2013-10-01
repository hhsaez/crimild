#ifndef CRIMILD_COLLADA_SAMPLER2D_
#define CRIMILD_COLLADA_SAMPLER2D_

#include "Entity.hpp"
#include "EntityList.hpp"

namespace crimild {

	namespace collada {

		class Sampler2D : public Entity {
		public:
			Sampler2D( void );

			virtual ~Sampler2D( void );

			virtual bool parseXML( xmlNode *input ) override;

			std::string getSourceName( void ) const { return _sourceName; }

		private:
			std::string _sourceName;
		};

		typedef std::shared_ptr< Sampler2D > Sampler2DPtr;

	}

}

#endif

