/*
 * Copyright (c) 2002 - present, H. Hernan Saez
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

#ifndef CRIMILD_OPENGL_LIGHT_CATALOG_
#define CRIMILD_OPENGL_LIGHT_CATALOG_

#include <Rendering/Catalog.hpp>
#include <Mathematics/Vector.hpp>
#include <Mathematics/Matrix.hpp>
#include <Foundation/Containers/Array.hpp>

namespace crimild {
    
    class Light;

	namespace opengl {

		class LightCatalog : public Catalog< Light > {
		public:
			LightCatalog( void );
			virtual ~LightCatalog( void );

			virtual void configure( void ) override;

			virtual int getNextResourceId( Light * ) override;

			virtual void bind( Light *light ) override;

		private:
			struct AmbientLightData {
                crimild::Vector4f ambient;
			};

			crimild::UInt32 _ambientLightBlockId = 0;
			containers::Array< crimild::Bool > _usedAmbientLightIds;
			containers::Array< AmbientLightData > _ambientLights;
			
			struct DirectionalLightData {
				Vector4f ambient;
				Vector4f diffuse;
				Vector4f direction;
				Matrix4f lightSpaceMatrix;
				Vector4f shadowMapViewport;
				Vector2f shadowMinMaxBias;
				crimild::UInt32 castShadows;
				crimild::Real32 padding;
			};

			crimild::UInt32 _directionalLightBlockId = 0;
			containers::Array< crimild::Bool > _usedDirectionalLightIds;
			containers::Array< DirectionalLightData > _directionalLights;

            struct PointLightData {
                Vector4f ambient;
                Vector4f diffuse;
                Vector4f position;
                Vector4f attenuation;
            };

            crimild::UInt32 _pointLightBlockId = 0;
            containers::Array< crimild::Bool > _usedPointLightIds;
            containers::Array< PointLightData > _pointLights;

            struct SpotLightData {
                Vector4f ambient;
                Vector4f diffuse;
                Vector4f direction;
                Vector4f position;
                Vector4f attenuation;
                Matrix4f lightSpaceMatrix;
                Vector4f shadowMapViewport;
                Vector2f shadowMinMaxBias;
                crimild::Real32 innerCutOff;
                crimild::Real32 outerCutOff;
                crimild::UInt32 castShadows;
                crimild::Real32 padding[ 3 ];
            };

            crimild::UInt32 _spotLightBlockId = 0;
            containers::Array< crimild::Bool > _usedSpotLightIds;
            containers::Array< SpotLightData > _spotLights;
		};

	}

}

#endif

