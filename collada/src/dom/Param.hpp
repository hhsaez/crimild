#ifndef CRIMILD_COLLADA_PARAM_
#define CRIMILD_COLLADA_PARAM_

#include "Entity.hpp"
#include "EntityList.hpp"

namespace crimild {

	namespace collada {

		class Param : public Entity {
		public:
			Param( void );

			virtual ~Param( void );

			bool parseXML( xmlNode *input );

			std::string getName( void ) const { return _name; }
			std::string getType( void ) const { return _type; }

		private:
			std::string _name;
			std::string _type;
		};

		typedef std::shared_ptr< collada::Param > ParamPtr;

		class ParamList : public EntityList< Param > {
		public:
			ParamList( void ) : EntityList< Param >( COLLADA_PARAM ) { }
			virtual ~ParamList( void ) { }
		};


	}

}

#endif

