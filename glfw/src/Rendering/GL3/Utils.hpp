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

#ifndef CRIMILD_GL3_RENDERING_UTILS_
#define CRIMILD_GL3_RENDERING_UTILS_

#include <Crimild.hpp>

namespace crimild {

	namespace gl3 {

		class Utils {
		public:
			static void checkErrors( std::string prefix );

			static Pointer< VertexShader > getVertexShaderInstance( std::string source );
			
			static Pointer< FragmentShader > getFragmentShaderInstance( std::string source );

			static std::string buildArrayShaderLocationName( std::string variable, int index );

			static std::string buildArrayShaderLocationName( std::string variable, int index, std::string member );
		};

	}

}

#define CRIMILD_CHECK_GL_ERRORS_BEFORE_CURRENT_FUNCTION crimild::gl3::Utils::checkErrors( std::string( "Before " ) + CRIMILD_CURRENT_FUNCTION );
#define CRIMILD_CHECK_GL_ERRORS_AFTER_CURRENT_FUNCTION crimild::gl3::Utils::checkErrors( std::string( "After " ) + CRIMILD_CURRENT_FUNCTION );

#endif

