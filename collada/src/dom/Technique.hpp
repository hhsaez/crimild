#ifndef CRIMILD_COLLADA_TECHNIQUE_
#define CRIMILD_COLLADA_TECHNIQUE_

#include "Entity.hpp"
#include "EntityMap.hpp"
#include "Phong.hpp"

namespace crimild {

	namespace collada {

		class Technique : public Entity {
		public:
			Technique( void );
			virtual ~Technique( void );

			virtual bool parseXML( xmlNode *input ) override;

			std::string getSid( void ) const { return _sid; }
			Phong *getPhong( void ) { return _phong.get(); }

		private:
			std::string _sid;
			PhongPtr _phong;
		};

		typedef std::shared_ptr< collada::Technique > TechniquePtr;

	}

}

#endif

