/*
 * Copyright (c) 2013, Hernan Saez
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

#ifndef CRIMILD_RENDERING_SHADER_PROGRAM_
#define CRIMILD_RENDERING_SHADER_PROGRAM_

#include "Shader.hpp"
#include "ShaderLocation.hpp"
#include "ShaderUniform.hpp"
#include "Catalog.hpp"

#include <functional>
#include <map>
#include <string>
#include <vector>
#include <list>

namespace crimild {

	class ShaderProgram : public Catalog< ShaderProgram >::Resource {
	public:
		class StandardLocation {
		public:
			enum {
				POSITION_ATTRIBUTE = 0,
				NORMAL_ATTRIBUTE,
				TANGENT_ATTRIBUTE,
				TEXTURE_COORD_ATTRIBUTE,
				COLOR_ATTRIBUTE,

				PROJECTION_MATRIX_UNIFORM = 100,
				VIEW_MATRIX_UNIFORM,
				MODEL_MATRIX_UNIFORM,
				NORMAL_MATRIX_UNIFORM,

				MATERIAL_AMBIENT_UNIFORM = 1000,
				MATERIAL_DIFFUSE_UNIFORM,
				MATERIAL_SPECULAR_UNIFORM,
				MATERIAL_SHININESS_UNIFORM,

				MATERIAL_COLOR_MAP_UNIFORM = 1500,
				MATERIAL_USE_COLOR_MAP_UNIFORM,
				MATERIAL_SPECULAR_MAP_UNIFORM,
				MATERIAL_USE_SPECULAR_MAP_UNIFORM,
				MATERIAL_NORMAL_MAP_UNIFORM,
				MATERIAL_USE_NORMAL_MAP_UNIFORM,

				LIGHT_COUNT_UNIFORM = 5000,
				LIGHT_POSITION_UNIFORM = 5100,
				LIGHT_ATTENUATION_UNIFORM = 5200,
				LIGHT_DIRECTION_UNIFORM = 5300,
				LIGHT_COLOR_UNIFORM = 5400,
				LIGHT_OUTER_CUTOFF_UNIFORM = 5500,
				LIGHT_INNER_CUTOFF_UNIFORM = 5600,
				LIGHT_EXPONENT_UNIFORM = 5700,
			};
		};

	public:
		ShaderProgram( VertexShaderPtr vs, FragmentShaderPtr fs );
		virtual ~ShaderProgram( void );

		VertexShader *getVertexShader( void ) { return _vertexShader.get(); }
		FragmentShader *getFragmentShader( void ) { return _fragmentShader.get(); }

	private:
		VertexShaderPtr _vertexShader;
		FragmentShaderPtr _fragmentShader;

	public:
		void registerLocation( ShaderLocationPtr location );
		ShaderLocation *getLocation( std::string name ) { return _locations[ name ].get(); }
		void resetLocations( void );
		void foreachLocation( std::function< void( ShaderLocationPtr & ) > callback );

		void registerStandardLocation( ShaderLocation::Type locationType, unsigned int standardLocationId, std::string name );
		ShaderLocation *getStandardLocation( unsigned int standardLocationId );

	private:
		std::map< std::string, ShaderLocationPtr > _locations;
		std::map< unsigned int, std::string > _standardLocations;

	public:
		void attachUniform( ShaderUniformPtr uniform );
		void detachAllUniforms( void );
		void foreachUniform( std::function< void( ShaderUniformPtr & ) > callback );

	private:
		std::list< ShaderUniformPtr > _uniforms;
	};

	typedef std::shared_ptr< ShaderProgram > ShaderProgramPtr;
	typedef Catalog< ShaderProgram > ShaderProgramCatalog;
	typedef std::shared_ptr< ShaderProgramCatalog > ShaderProgramCatalogPtr;

}

#endif

