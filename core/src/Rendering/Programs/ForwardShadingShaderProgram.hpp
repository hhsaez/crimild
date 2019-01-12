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

#ifndef CRIMILD_CORE_RENDERING_PROGRAMS_FORWARD_SHADING_
#define CRIMILD_CORE_RENDERING_PROGRAMS_FORWARD_SHADING_

#include "Rendering/ShaderProgram.hpp"

namespace crimild {

	/**
	   \brief Compute lighting for a scene using a forward shading pipeline

	   Conditional expressions (if) are avoided by computing both
	   branches and then adding them togheter using a flag multiplier to discard
	   one result or the other. Since computations are simple, this should be
	   more efficient than conditional expressions. So, this 
	   \code
	   vec3 lightVec = vec3( 0 );
	   if ( lightType == DIRECTIONAL ) {
	       lightVec = -light.direction;
	   } else {
	       lightVec = light.position - vertexPos;
	   }
	   \endcode

	   becomes this (with lightIsPoint == 1 for point lights and 0 for directional lights

	   \code
	   lightVec = lightIsPoint * (light.position - vertexPos) + (1 - lightIsPoint) * -light.direction;
	   \endcode

	 */
	class ForwardShadingShaderProgram : public ShaderProgram {
	public:
		ForwardShadingShaderProgram( void );
		virtual ~ForwardShadingShaderProgram( void );

	private:
		void createVertexShader( void );
		void createFragmentShader( void );
	};

}

#endif

