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
#include "Catalog.hpp"

#include <functional>
#include <map>
#include <string>

namespace Crimild {

	class ShaderProgram : public Catalog< ShaderProgram >::Resource {
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

		void registerPositionAttributeLocation( std::string name );
		ShaderLocation *getPositionAttributeLocation( void ) { return _positionAttributeLocation.get(); }

		void registerColorAttributeLocation( std::string name );
		ShaderLocation *getColorAttributeLocation( void ) { return _colorAttributeLocation.get(); }

		void registerTextureCoordAttributeLocation( std::string name );
		ShaderLocation *getTextureCoordAttributeLocation( void ) { return _textureCoordAttributeLocation.get(); }

		void registerProjectionMatrixUniformLocation( std::string name );
		ShaderLocation *getProjectionMatrixUniformLocation( void ) { return _projectionMatrixUniformLocation.get(); }

		void registerViewMatrixUniformLocation( std::string name );
		ShaderLocation *getViewMatrixUniformLocation( void ) { return _viewMatrixUniformLocation.get(); }

		void registerModelMatrixUniformLocation( std::string name );
		ShaderLocation *getModelMatrixUniformLocation( void ) { return _modelMatrixUniformLocation.get(); }

		void registerMaterialDiffuseUniformLocation( std::string name );
		ShaderLocation *getMaterialDiffuseUniformLocation( void ) { return _materialDiffuseUniformLocation.get(); }

		void registerMaterialColorMapUniformLocation( std::string name );
		ShaderLocation *getMaterialColorMapUniformLocation( void ) { return _materialColorMapUniformLocation.get(); }

	private:
		std::map< std::string, ShaderLocationPtr > _locations;
		ShaderLocationPtr _positionAttributeLocation;
		ShaderLocationPtr _colorAttributeLocation;
		ShaderLocationPtr _textureCoordAttributeLocation;
		ShaderLocationPtr _projectionMatrixUniformLocation;
		ShaderLocationPtr _viewMatrixUniformLocation;
		ShaderLocationPtr _modelMatrixUniformLocation;
		ShaderLocationPtr _materialDiffuseUniformLocation;
		ShaderLocationPtr _materialColorMapUniformLocation;
	};

	typedef std::shared_ptr< ShaderProgram > ShaderProgramPtr;
	typedef Catalog< ShaderProgram > ShaderProgramCatalog;
	typedef std::shared_ptr< ShaderProgramCatalog > ShaderProgramCatalogPtr;

}

#endif

