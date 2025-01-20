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

#include "Views/Dialogs/FileDialog.hpp"

#include "Concurrency/Async.hpp"
#include "Simulation/Editor.hpp"

#include <filesystem>

using namespace crimild::editor;

FileDialog::FileDialog(
    std::string_view title,
    Handler handler,
    std::string_view filters,
    std::string_view pathName
) noexcept
    : Dialog( title ),
      m_handler( handler ),
      m_filters( filters ),
      m_pathName( pathName )
{
    setMinSize( { 600, 400 } );

    // ImGuiFileDialogFlags flags = ImGuiFileDialogFlags_None;
    IGFD::FileDialogConfig config;
    config.path = m_pathName.c_str();
    ImGuiFileDialog::Instance()->OpenDialog(
        getName().c_str(),
        getName().c_str(),
        m_filters.c_str(),
        config
        // m_pathName.c_str(),
        // 1,
        // nullptr,
        // flags
    );
}

void FileDialog::drawContent( void ) noexcept
{
    if ( ImGuiFileDialog::Instance()->Display( getName(), ImGuiWindowFlags_NoDocking, getMinSize(), getMaxSize() ) ) {
        if ( ImGuiFileDialog::Instance()->IsOk() ) {
            std::string filePathName = ImGuiFileDialog::Instance()->GetFilePathName();
            const auto path = std::filesystem::path { filePathName };
            // Resolve at the beginning of next frame
            crimild::concurrency::sync_frame(
                [ path, handler = m_handler ] {
                    handler( path );
                }
            );
        }
        ImGuiFileDialog::Instance()->Close();
    }
}
