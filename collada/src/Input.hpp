#ifndef CRIMILD_COLLADA_INPUT_
#define CRIMILD_COLLADA_INPUT_

#include "Entity.hpp"
#include "EntityLibrary.hpp"

namespace crimild {

	namespace collada {

		class Source;

		class Input : public Entity {
		public:
			Input( void );

			virtual ~Input( void );

			bool parseXML( xmlNode *input );

			inline const char *getSemantic( void ) const { return _semantic.c_str(); }
			inline const char *getSourceID( void ) const { return _sourceID.c_str(); }

			inline void setSourceRef( Source *source ) { _source = source; }
			inline Source *getSourceRef( void ) { return _source; }

		private:
			std::string _semantic;
			std::string _sourceID;
			Source *_source;
		};

		typedef std::shared_ptr< Input > InputPtr;

		class InputLibrary : public EntityLibrary< Input > {
		public:
			InputLibrary( void ) : EntityLibrary< Input >( COLLADA_INPUT ) { }
			virtual ~InputLibrary( void ) { }
		};

	}

}

#endif

