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

#ifndef CRIMILD_EDITOR_SIMULATION_EDITOR
#define CRIMILD_EDITOR_SIMULATION_EDITOR

#include <Crimild.hpp>

namespace crimild::editor {

    class Project;

    class Editor : public Simulation {
    public:
        // Cannot use DynamicSingleton here since Simulation is already deriving from it.
        // TODO: Maybe consider using virtual inheritance?
        static Editor *getInstance( void ) noexcept { return s_instance; }

    private:
        static Editor *s_instance;

    public:
        class State : public coding::Codable {
            CRIMILD_IMPLEMENT_RTTI( crimild::editor::Editor::State )
        public:
            coding::Codable *selectedObject = nullptr;

            void encode( coding::Encoder &encoder ) override;
            void decode( coding::Decoder &decoder ) override;
        };

    public:
        Editor( void ) noexcept;
        ~Editor( void ) noexcept;

        virtual Event handle( const Event &e ) noexcept override;

        inline void setSelectedObject( coding::Codable *selected ) noexcept
        {
            if ( m_state != nullptr ) {
                m_state->selectedObject = selected;
            }
        }

        template< class SelectedObjectType >
        inline SelectedObjectType *getSelectedObject( void ) noexcept
        {
            return m_state != nullptr
                       ? dynamic_cast< SelectedObjectType * >( m_state->selectedObject )
                       : nullptr;
        }

        void createProject( const std::filesystem::path &path ) noexcept;
        void loadProject( const std::filesystem::path &path ) noexcept;
        void saveProject( void ) noexcept;

        inline Project *getProject( void ) noexcept { return m_project.get(); }
        inline const Project *getProject( void ) const noexcept { return m_project.get(); }

        inline const std::list< std::string > &getRecentProjects( void ) const noexcept { return m_recentProjects; }

        void createNewScene( const std::filesystem::path &path ) noexcept;
        void loadScene( const std::filesystem::path &path ) noexcept;
        void saveScene( void ) noexcept;
        void saveSceneAs( const std::filesystem::path &path ) noexcept;

    private:
        void saveRecentProjects( void ) noexcept;
        void loadRecentProjects( void ) noexcept;

        SharedPointer< Node > createDefaultScene( void ) noexcept;

    private:
        SharedPointer< State > m_state;

        std::shared_ptr< Node > m_edittableScene;

        std::shared_ptr< Project > m_project;

        std::list< std::string > m_recentProjects;
    };

}

#endif
