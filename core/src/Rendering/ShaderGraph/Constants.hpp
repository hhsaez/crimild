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

#ifndef CRIMILD_RENDERING_SHADER_GRAPH_CONSTANTS_
#define CRIMILD_RENDERING_SHADER_GRAPH_CONSTANTS_

#include "Foundation/Types.hpp"

#include <string>
#include <sstream>

namespace crimild {

	namespace shadergraph {

		namespace locations {

			const char * const PROJECTION_MATRIX_UNIFORM = "uPMatrix";			
			const char * const VIEW_MATRIX_UNIFORM = "uVMatrix";			
			const char * const MODEL_MATRIX_UNIFORM = "uMMatrix";			
			const char * const NORMAL_MATRIX_UNIFORM = "uNMatrix";

			const char * const COLOR_UNIFORM = "uColor";
			const char * const COLOR_MAP_UNIFORM = "uColorMap";
			const char * const SPECULAR_UNIFORM = "uSpecular";
			const char * const SPECULAR_MAP_UNIFORM = "uSpecularMap";
			const char * const SHININESS_UNIFORM = "uShininess";

			const char * const LIGHT_UNIFORM = "uLight";
			const char * const LIGHT_AMBIENT_UNIFORM = "ambient";
			const char * const LIGHT_DIFFUSE_UNIFORM = "diffuse";
			const char * const LIGHT_HAS_DIRECTION_UNIFORM = "hasDirection";
			const char * const LIGHT_DIRECTION_UNIFORM = "directon";
			const char * const LIGHT_HAS_POSITION_UNIFORM = "hasPosition";
			const char * const LIGHT_POSITION_UNIFORM = "position";
			const char * const LIGHT_HAS_ATTENUATION_UNIFORM = "hasAttenuation";
			const char * const LIGHT_ATTENUATION_UNIFORM = "attenuation";
			const char * const LIGHT_INNER_CUTOFF_UNIFORM = "innerCutOff";
			const char * const LIGHT_OUTER_CUTOFF_UNIFORM = "outerCutOff";
			const char * const LIGHT_ARRAY_COUNT_UNIFORM = "uLightCount";
			const char * const LIGHT_ARRAY_UNIFORM = "uLights";

		}

		namespace variants {

			const char * const MODEL_TEXTURE_COORDS_VARIANT = "vTextureCoord";

			const char * const WORLD_POSITION_VARIANT = "vWorldPosition";
			const char * const WORLD_NORMAL_VARIANT = "vWorldNormal";
			const char * const WORLD_EYE_VARIANT = "vWorldEye";

			const char * const CLIP_POSITION_VARIANT = "vClipPosition";

		}

	}

}

#endif

