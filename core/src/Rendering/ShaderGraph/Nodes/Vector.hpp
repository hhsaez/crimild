/*
 * Copyright (c) 2002-present, H. Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_RENDERING_SHADER_GRAPH_NODES_VECTOR_
#define CRIMILD_RENDERING_SHADER_GRAPH_NODES_VECTOR_

#include "Rendering/ShaderGraph/ShaderGraphOperation.hpp"

#include "Mathematics/Vector.hpp"

namespace crimild {

	namespace shadergraph {

		class ShaderGraphVariable;

		class VectorToScalars : public ShaderGraphOperation {
			CRIMILD_IMPLEMENT_RTTI( crimild::shadergraph::VectorToScalars )
			
		public:
			VectorToScalars( ShaderGraph *, ShaderGraphVariable *vector );
			virtual ~VectorToScalars( void );

			ShaderGraphVariable *getVector( void ) { return _vector; }

			ShaderGraphVariable *getX( void ) { return _x; }
			ShaderGraphVariable *getY( void ) { return _y; }
			ShaderGraphVariable *getZ( void ) { return _z; }
			ShaderGraphVariable *getW( void ) { return _w; }

		private:
			ShaderGraphVariable *_vector = nullptr;
			ShaderGraphVariable *_x = nullptr;
			ShaderGraphVariable *_y = nullptr;
			ShaderGraphVariable *_z = nullptr;
			ShaderGraphVariable *_w = nullptr;

		private:
			virtual void setup( ShaderGraph *graph ) override;
		};

		class ScalarsToVector : public ShaderGraphOperation {
			CRIMILD_IMPLEMENT_RTTI( crimild::shadergraph::ScalarsToVector )
			
		public:
			ScalarsToVector( ShaderGraph *, ShaderGraphVariable *x, ShaderGraphVariable *y );
			ScalarsToVector( ShaderGraph *, ShaderGraphVariable *x, ShaderGraphVariable *y, ShaderGraphVariable *z );
			ScalarsToVector( ShaderGraph *, ShaderGraphVariable *x, ShaderGraphVariable *y, ShaderGraphVariable *z, ShaderGraphVariable *w );
			virtual ~ScalarsToVector( void );

			ShaderGraphVariable *getVector( void ) { return _vector; }

			ShaderGraphVariable *getX( void ) { return _x; }
			ShaderGraphVariable *getY( void ) { return _y; }
			ShaderGraphVariable *getZ( void ) { return _z; }
			ShaderGraphVariable *getW( void ) { return _w; }

		private:
			ShaderGraphVariable *_vector = nullptr;
			ShaderGraphVariable *_x = nullptr;
			ShaderGraphVariable *_y = nullptr;
			ShaderGraphVariable *_z = nullptr;
			ShaderGraphVariable *_w = nullptr;
			
		private:
			virtual void setup( ShaderGraph *graph ) override;
		};

		class VectorConstant : public ShaderGraphOperation {
			CRIMILD_IMPLEMENT_RTTI( crimild::scenegraph::VectorConstant )

		public:
			VectorConstant( ShaderGraph *, const Vector2f &v );
			VectorConstant( ShaderGraph *, const Vector3f &v );
			VectorConstant( ShaderGraph *, const Vector4f &v );
			virtual ~VectorConstant( void );

			const Vector4f &getValue( void ) { return _value; }
			ShaderGraphVariable *getVector( void ) { return _vector; }

		private:
			Vector4f _value;
			ShaderGraphVariable *_vector = nullptr;

		public:
			virtual void setup( ShaderGraph *graph ) override;
		};

	}

}

#endif

