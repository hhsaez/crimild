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

#ifndef CRIMILD_RENDERING_SHADER_GRAPH_LANGUAGE_
#define CRIMILD_RENDERING_SHADER_GRAPH_LANGUAGE_

#include "Foundation/Types.hpp"
#include "Foundation/Containers/Array.hpp"
#include "Foundation/Memory.hpp"
#include "Mathematics/Vector.hpp"

#include <string>

namespace crimild {

	class ShaderUniform;
	class Texture;

	namespace shadergraph {

		class Variable;

		namespace csl {

			Variable *scalar( crimild::Real32 value, std::string name = "" );
			Variable *scalar_uniform( std::string name );
			Variable *scalar_uniform( std::string name, crimild::Real32 defaultValue );
			Variable *scalar_uniform( SharedPointer< ShaderUniform > const &uniform );
			Variable *scalar_constant( crimild::Real32 value );
			Variable *scalar_one( void );
			Variable *scalar_two( void );
			Variable *scalar_zero( void );

			Variable *vec2_in( std::string name );
			Variable *vec2( Variable *vector );
			Variable *vec2_uniform( std::string name );
			Variable *vec2_uniform( SharedPointer< ShaderUniform > const &uniform );
			
			Variable *vec3( Variable *vector );
			Variable *vec3( const Vector3f &v );
			Variable *vec3_in( std::string name );
			Variable *vec3_uniform( std::string name );
			Variable *vec3_uniform( SharedPointer< ShaderUniform > const &uniform );
			Variable *vec3_uniform( ShaderUniform *uniform );
			
			Variable *vec4_in( std::string name );
			Variable *vec4( Variable *scalar );
			Variable *vec4( Variable *vector, Variable *scalar );
			Variable *vec4( Variable *x, Variable *y, Variable *z, Variable *w );
			Variable *vec4( const Vector4f &value );
			Variable *vec4_uniform( std::string name );
			Variable *vec4_uniform( std::string name, const Vector4f &defaultValue );
			Variable *vec4_uniform( SharedPointer< ShaderUniform > const &uniform );
			Variable *vec4_uniform( ShaderUniform *uniform );
			
			Variable *vec_x( Variable *vector );
			Variable *vec_y( Variable *vector );
			Variable *vec_z( Variable *vector );
			Variable *vec_w( Variable *vector );
			Variable *vec_yzww( Variable *vector );

			Variable *mat3( Variable *matrix );
			Variable *mat3_uniform( std::string name );
			Variable *mat3_uniform( SharedPointer< ShaderUniform > const &uniform );

			Variable *mat4( Variable *matrix );
			Variable *mat4_uniform( std::string name );
			Variable *mat4_uniform( SharedPointer< ShaderUniform > const &uniform );

			Variable *red( Variable *color );
			Variable *green( Variable *color );
			Variable *blue( Variable *color );
			Variable *alpha( Variable *color );

			/**
			   \name Math ops
			*/
			//@{
			
			Variable *add( Variable *a, Variable *b );
			Variable *add( containers::Array< Variable * > const &inputs );
			template< class ... Args >
			Variable *add( Args &&... args ) { return add( { args... } ); }

			Variable *sub( Variable *a, Variable *b );

			Variable *mult( Variable *a, Variable *b );
			Variable *mult( containers::Array< Variable * > const &inputs );
			template< class ... Args >
			Variable *mult( Args &&... args ) { return mult( { args... } ); }

			Variable *div( Variable *a, Variable *b );

			Variable *pow( Variable *base, Variable *exp );

			Variable *max( Variable *a, Variable *b );
			Variable *neg( Variable *input );

			Variable *dot( Variable *a, Variable *b );
			Variable *normalize( Variable *input );
			Variable *reflect( Variable *incident, Variable *normal );
			Variable *refract( Variable *incident, Variable *normal, Variable *ratio );
			Variable *length( Variable *input );
			Variable *inverse( Variable *matrix );
			Variable *clamp( Variable *value, Variable *lowerBound, Variable *upperBound );
			Variable *fract( Variable *value );

			//@}

			void vertexPosition( Variable *position );
			void vertexPointSize( Variable *pointSize );
			void vertexOutput( std::string name, Variable *value );

			/**
			   \name Fragment inputs
			 */
			//@{

			Variable *fragCoord( void );
			Variable *pointCoord( void );

			//@}

			/**
			   \name Fragment outputs
			 */
			//@{

			void alphaClip( Variable *alpha, Variable *threshold );
			void fragColor( Variable *color );

			//@}

			/**
			   \name Position
			 */
			//@{

			Variable *modelPosition( void );
			Variable *worldPosition( void );
			Variable *viewPosition( void );
			Variable *clipPosition( void );
			Variable *screenPosition( void );

			//@}

            /**
                \name Color
             */
            //@{
            Variable *modelColor( void );

            //@}

			/**
			   \name Texture Coordinates
			*/
			//@{

			Variable *modelTextureCoords( void );

			//@}

			/**
			   \name Normals
			*/
			//@{

			Variable *modelNormal( void );
			Variable *worldNormal( void );
			Variable *worldNormal( Variable *worldMatrix, Variable *normal );
			Variable *viewNormal( void );


			//@}

			/**
			   \name Encoding
			*/
			//@{
			
			Variable *encodeFloatToRGBA( Variable *value );
			Variable *decodeFloatFromRGBA( Variable *rgba );

			//@}

			/**
			   \name Camera vectors
			 */
			//@{

			Variable *worldCameraPos( void );
			Variable *worldEyeVector( void );
			Variable *viewEyeVector( void );
			
			Variable *viewVector( void );
			Variable *viewVector( Variable *viewPosition );

			//@}

			Variable *linearizeDepth( Variable *input, Variable *near, Variable *far );

			/**
			   \name Texture
			 */
			//@{
			
			Variable *texture2D_uniform( std::string name );
			Variable *texture2D_uniform( std::string name, SharedPointer< Texture > const &defaultValue );
			Variable *texture2D_uniform( SharedPointer< ShaderUniform > const &uniform );

			Variable *textureCube_uniform( std::string name );
			Variable *textureCube_uniform( std::string name, SharedPointer< Texture > const &defaultValue );
			Variable *textureCube_uniform( SharedPointer< ShaderUniform > const &uniform );

			Variable *textureColor( Variable *texture, Variable *uvs );

			Variable *textureUnitVector( Variable *texture, Variable *uvs );

			//@}

			/**
			   \name Standard uniforms
			 */
			//@{

			Variable *projectionMatrix( void );
			Variable *viewMatrix( void );
			Variable *modelMatrix( void );
			Variable *normalMatrix( void );

			Variable *colorUniform( void );
			Variable *colorMapUniform( void );
			Variable *specularUniform( void );
			Variable *specularMapUniform( void );

			//@}
		}

	}

}

#endif

