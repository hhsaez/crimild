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

#ifndef CRIMILD_RENDERING_MATERIAL_
#define CRIMILD_RENDERING_MATERIAL_

#include "AlphaState.hpp"
#include "DepthState.hpp"
#include "Texture.hpp"
#include "ShaderProgram.hpp"

#include "Mathematics/Vector.hpp"

namespace crimild {

	class Light;
    
	class Material : public SharedObject {
		CRIMILD_DISALLOW_COPY_AND_ASSIGN( Material );

	public:
		Material( void );
		virtual ~Material( void );

		void setProgram( ShaderProgramPtr const &program ) { _program = program; }
        ShaderProgramPtr &getProgram( void ) { return _program; }

		void setAmbient( const RGBAColorf &ambient ) { _ambient = ambient; }
		const RGBAColorf &getAmbient( void ) const { return _ambient; }

		void setDiffuse( const RGBAColorf &color ) { _diffuse = color; }
		const RGBAColorf &getDiffuse( void ) const { return _diffuse; }

		void setSpecular( const RGBAColorf &color ) { _specular = color; }
		const RGBAColorf &getSpecular( void ) const { return _specular; }

		void setShininess( float value ) { _shininess = value; }
		float getShininess( void ) const { return _shininess; }

		void setColorMap( TexturePtr const &texture ) { _colorMap = texture; }
        TexturePtr &getColorMap( void ) { return _colorMap; }

		void setNormalMap( TexturePtr const &texture ) { _normalMap = texture; }
        TexturePtr &getNormalMap( void ) { return _normalMap; }

		void setSpecularMap( TexturePtr const &texture ) { _specularMap = texture; }
        TexturePtr &getSpecularMap( void ) { return _specularMap; }
        
        void setEmissiveMap( TexturePtr const &texture ) { _emissiveMap = texture; }
        TexturePtr &getEmissiveMap( void ) { return _emissiveMap; }

		void setDepthState( DepthStatePtr const &state ) { _depthState = state; }
        DepthStatePtr &getDepthState( void ) { return _depthState; }

		void setAlphaState( AlphaStatePtr const &alphaState ) { _alphaState = alphaState; }
        AlphaStatePtr &getAlphaState( void ) { return _alphaState; }

	private:
		ShaderProgramPtr _program;

		RGBAColorf _ambient;
		RGBAColorf _diffuse;
		RGBAColorf _specular;
		float _shininess;

		TexturePtr _colorMap;
		TexturePtr _normalMap;
		TexturePtr _specularMap;
        TexturePtr _emissiveMap;

		DepthStatePtr _depthState;
		AlphaStatePtr _alphaState;
	};

    using MaterialPtr = std::shared_ptr< Material >;
    
}

#endif

	