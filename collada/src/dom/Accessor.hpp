#ifndef CRIMILD_COLLADA_ACCESSOR_
#define CRIMILD_COLLADA_ACCESSOR_

#include "Entity.hpp"
#include "Param.hpp"

namespace crimild {

	namespace collada {

		class Accessor : public Entity {
		public:
			Accessor( void );

			virtual ~Accessor( void );

			virtual bool parseXML( xmlNode *input ) override;

			unsigned int getCount( void ) const { return _count; }
			unsigned int getStride( void ) const { return _stride; }
			ParamList *getParams( void ) { return &_params; }

		private:
			unsigned int _count;
			unsigned int _stride;
			ParamList _params;
		};

	}

}

#endif

