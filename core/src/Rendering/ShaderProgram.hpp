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

#ifndef CRIMILD_RENDERING_SHADER_PROGRAM_
#define CRIMILD_RENDERING_SHADER_PROGRAM_

#include "Foundation/Containers/Array.hpp"
#include "Foundation/Containers/Map.hpp"
#include "Foundation/RTTI.hpp"
#include "Rendering/Catalog.hpp"
#include "Rendering/Shader.hpp"
#include "Rendering/ShaderLocation.hpp"
#include "Rendering/ShaderUniform.hpp"
#include "Rendering/VertexBuffer.hpp"

#include <functional>
#include <list>
#include <map>
#include <string>
#include <vector>

namespace crimild {

    class DescriptorSetLayout;
    class Light;
    class Texture;

    namespace shadergraph {

        class ShaderGraph;

    }

    class ShaderProgram : public SharedObject,
                          public RTTI,
                          public Catalog< ShaderProgram >::Resource {
        CRIMILD_IMPLEMENT_RTTI( crimild::ShaderProgram )

    public:
        using ShaderArray = Array< SharedPointer< Shader > >;

    public:
        ShaderProgram( void ) = default;
        explicit ShaderProgram( const ShaderArray &shaders ) noexcept;
        virtual ~ShaderProgram( void ) = default;

        inline void setShaders( ShaderArray const &shaders ) noexcept { m_shaders = shaders; }
        ShaderArray &getShaders( void ) noexcept { return m_shaders; }

        Array< VertexLayout > vertexLayouts;
        Array< VertexLayout > instanceLayouts;
        Array< SharedPointer< DescriptorSetLayout > > descriptorSetLayouts;

    private:
        ShaderArray m_shaders;
    };

}

#endif
