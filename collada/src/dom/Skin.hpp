#ifndef CRIMILD_COLLADA_SKIN_
#define CRIMILD_COLLADA_SKIN_

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

			virtual bool parseXML( xmlNode *input ) override;

			inline const char *getSourceID( void ) const { return _sourceID.c_str(); }
			inline Joints *getJoints( void ) { return _joints.get(); }
			inline VertexWeights *getVertexWeights( void ) { return _vertexWeights.get(); }
			inline SourceMap *getSources( void ) { return &_sources; }

		public:
			std::string _sourceID;
			SourceMap _sources;
			JointsPtr _joints;
			VertexWeightsPtr _vertexWeights;
		};

		typedef std::shared_ptr< Skin > SkinPtr;

	}

}

#endif

