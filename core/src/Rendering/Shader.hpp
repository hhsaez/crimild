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

#ifndef CRIMILD_RENDERING_SHADER_
#define CRIMILD_RENDERING_SHADER_

#include "Coding/Codable.hpp"
#include "Crimild_Foundation.hpp"

#include <string>

namespace crimild {

    /**
           \todo There's no point in keeping the data after the shader has been used
           in a rendering pipeline. We should clean it.
         */
    class Shader : public coding::Codable {
        CRIMILD_IMPLEMENT_RTTI( crimild::Shader )

    public:
        enum class Stage {
            VERTEX,
            TESSELLATION_CONTROL,
            TESSELLATION_EVALUATION,
            GEOMETRY,
            FRAGMENT,
            COMPUTE,
            ALL_GRAPHICS,
            ALL,
        };

        static std::string getStageDescription( const Stage &stage ) noexcept;

        using Data = std::vector< std::byte >;

        enum class DataType {
            INLINE,
            BINARY,
        };

        static SharedPointer< Shader > withSource( Stage stage, const FilePath &filePath ) noexcept;
        static SharedPointer< Shader > withBinary( Stage stage, const FilePath &filePath ) noexcept;

    public:
        /**
         * \brief Default constructor
         *
         * Only used by coding system
         */
        Shader( void ) = default;
        explicit Shader( Stage stage, const std::string &source, const std::string &entryPointName = "main" ) noexcept;
        explicit Shader( Stage stage, const Data &data = Data(), DataType dataType = DataType::BINARY, std::string entryPointName = "main" ) noexcept;
        virtual ~Shader( void ) = default;

        inline const Stage &getStage( void ) const noexcept { return m_stage; }

        /**
                   \brief Returns a printable version of the shader stage

                   This is mostly for debugging purposes.
                 */
        inline std::string getStageDescription( void ) const noexcept
        {
            return getStageDescription( getStage() );
        }

        inline DataType getDataType( void ) const noexcept { return m_dataType; }
        inline const Data &getData( void ) const noexcept { return m_data; }

        inline const std::string &getEntryPointName( void ) const noexcept { return m_entryPointName; }

    private:
        Stage m_stage;
        DataType m_dataType;
        Data m_data;
        std::string m_entryPointName;

        /**
            \name Coding support
        */
        //@{

    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;

        //@}
        /**
                   \deprecated
                 */
        //@{

    public:
        explicit Shader( std::string source );

        inline void setSource( std::string source ) noexcept { m_source = source; }
        inline const std::string &getSource( void ) const noexcept { return m_source; }

    private:
        std::string m_source;

        //@}
    };

    using VertexShader = Shader;
    using FragmentShader = Shader;

}

#endif
