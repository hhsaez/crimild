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

#ifndef CRIMILD_CORE_RENDERING_SHADER_LIBRARY_
#define CRIMILD_CORE_RENDERING_SHADER_LIBRARY_

#include "Rendering/RenderResource.hpp"

namespace crimild {

    namespace constants {

        static const char *SHADER_UNLIT_VERTEX_P2C3 = "shaders/unlit/vertex_P2C3";
        static const char *SHADER_UNLIT_FRAGMENT_COLOR = "shaders/unlit/frag_color";

        static const char *SHADER_UNLIT_VERTEX_P2C3TC2 = "shaders/unlit/vertex_P2C3TC2";
        static const char *SHADER_UNLIT_FRAGMENT_TEXTURE_COLOR = "shaders/unlit/frag_texture_color";

        static const char *SHADER_UNLIT_TEXTURE_P3N3TC2_VERT = "shaders/unlit/texture_P3N3TC2.vert";
        static const char *SHADER_UNLIT_TEXTURE_P3N3TC2_FRAG = "shaders/unlit/texture_P3N3TC2.frag";

        static const char *SHADER_UNLIT_SKYBOX_P3_VERT = "shaders/unlit/skybox_P3.vert";
        static const char *SHADER_UNLIT_SKYBOX_P3_FRAG = "shaders/unlit/skybox_P3.frag";

        static const char *SHADER_DEBUG_POSITION_P3_VERT = "shaders/debug/position_P3.vert";
        static const char *SHADER_DEBUG_POSITION_P3_FRAG = "shaders/debug/position_P3.frag";
        static const char *SHADER_DEBUG_POSITION_P3N3TC2_VERT = "shaders/debug/position_P3N3TC2.vert";
        static const char *SHADER_DEBUG_POSITION_P3N3TC2_FRAG = "shaders/debug/position_P3N3TC2.frag";

    }

    class Shader;

    // Required for specialization
    // Platform-dependent libraries (i.e. OpenGL, Vulkan, etc) must implement this
    template<> RenderResourceLibrary< Shader >::RenderResourceLibrary( void ) noexcept;

    using ShaderLibrary = RenderResourceLibrary< Shader >;

}

#endif

