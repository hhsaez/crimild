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
*     * Neither the name of the copyright holder nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
* DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
* (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
* LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef CRIMILD_CORE_RENDERING_SHADER_PROGRAM_LIBRARY_
#define CRIMILD_CORE_RENDERING_SHADER_PROGRAM_LIBRARY_

#include "Rendering/RenderResource.hpp"

namespace crimild {

    namespace constants {

        static const char *SHADER_PROGRAM_UNLIT_P2C3_COLOR = "shader_programs/unlit/P2C3_color";
        static const char *SHADER_PROGRAM_UNLIT_P2C3TC2_TEXTURE_COLOR = "shader_programs/unlit/P2C3TC2_texture_color";
        static const char *SHADER_PROGRAM_UNLIT_SKYBOX_P3 = "shader_programs/unlit/skybox_P3";
        static const char *SHADER_PROGRAM_UNLIT_TEXTURE_P3N3TC2 = "shader_programs/unlit/texture_P3N3TC2";

        static const char *SHADER_PROGRAM_DEBUG_POSITION_P3 = "shader_programs/debug/position_P3";
        static const char *SHADER_PROGRAM_DEBUG_POSITION_P3N3TC2 = "shader_programs/debug/position_P3N3TC2";

    }

    class ShaderProgram;

    // Required for specialization
    template<> RenderResourceLibrary< ShaderProgram >::RenderResourceLibrary( void ) noexcept;

    using ShaderProgramLibrary = RenderResourceLibrary< ShaderProgram >;

}

#endif

