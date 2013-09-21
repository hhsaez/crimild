#ifndef CRIMILD_COLLADA_SKIN_
#define CRIMILD_COLLADA_SKIN_

#include "Entity.hpp"
#include "EntityLibrary.hpp"
#include "Input.hpp"
#include "Source.hpp"
#include "Joints.hpp"
#include "VertexWeights.hpp"

namespace crimild {

	namespace collada {

		class Skin : public Entity {
		public:
			Skin( void );

			virtual ~Skin( void );

			bool parseXML( xmlNode *input );

			inline const char *getSourceID( void ) const { return _sourceID.c_str(); }
			inline Joints *getJoints( void ) { return _joints.get(); }
			inline VertexWeights *getVertexWeights( void ) { return _vertexWeights.get(); }
			inline SourceCatalog *getSourceCatalog( void ) { return &_sources; }

		public:
			std::string _sourceID;
			SourceCatalog _sources;
			JointsPtr _joints;
			VertexWeightsPtr _vertexWeights;
		};

		typedef std::shared_ptr< Skin > SkinPtr;

	}

}

#endif

