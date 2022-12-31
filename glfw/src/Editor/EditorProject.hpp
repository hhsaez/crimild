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

#ifndef CRIMILD_DESKTOP_EDITOR_PROJECT_
#define CRIMILD_DESKTOP_EDITOR_PROJECT_

#include "Coding/Codable.hpp"
#include "Foundation/Named.hpp"
#include "Foundation/Versionable.hpp"

#include <filesystem>

namespace crimild {

    namespace editor {

        class Project
            : public coding::Codable,
              public Named,
              public Versionable {
            CRIMILD_IMPLEMENT_RTTI( crimild::editor::Project )
        public:
            Project( void ) noexcept;
            Project( std::string name, const Version &version ) noexcept;
            virtual ~Project( void ) = default;

            /**
             * \brief Get the absolute path to the project.crimild file
             */
            inline const std::filesystem::path &getFilePath( void ) const noexcept { return m_filePath; }

            inline std::filesystem::path getRootDirectory( void ) const noexcept { return m_filePath.parent_path(); }
            inline std::filesystem::path getAssetsDirectory( void ) const noexcept { return getRootDirectory() / "Assets"; }
            inline std::filesystem::path getScenesDirectory( void ) const noexcept { return getAssetsDirectory() / "Scenes"; }

            inline void setCurrentSceneName( std::string_view sceneName ) noexcept { m_currentSceneName = sceneName; }
            inline const std::string &getCurrentSceneName( void ) const noexcept { return m_currentSceneName; }
            inline std::filesystem::path getScenePath( std::string_view sceneName ) const noexcept { return getScenesDirectory() / ( std::string( sceneName ) + ".crimild" ); }

        private:
            mutable std::filesystem::path m_filePath;

            std::string m_currentSceneName = "main";

            /**
                \name Coding support
             */
            //@{

        public:
            virtual void encode( coding::Encoder &encoder ) override;
            virtual void decode( coding::Decoder &decoder ) override;

            //@}

            /**
             * \name Internal use only
             */
            //@{

        public:
            inline void setFilePath( const std::filesystem::path &path ) const noexcept { m_filePath = std::filesystem::absolute( path ); }

            //@}
        };

    }

}

#endif
