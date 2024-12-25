#ifndef CRIMILD_PLATFORM_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_BUILDERS_
#define CRIMILD_PLATFORM_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_BUILDERS_

#include "Foundation/ImGuiUtils.hpp"

namespace crimild::editor::utils {

    class BlueprintNodeBuilder {
    public:
        BlueprintNodeBuilder( ImTextureID texture = nullptr, int textureWidth = 0, int textureHeight = 0 ) noexcept;
        ~BlueprintNodeBuilder( void ) = default;

        void begin( ax::NodeEditor::NodeId id ) noexcept;
        void End( void ) noexcept;

        void header( const ImVec4 &color = ImVec4( 1, 1, 1, 1 ) ) noexcept;
        void endHeader( void ) noexcept;

        void input( ax::NodeEditor::PinId id ) noexcept;
        void endInput( void ) noexcept;

        void middle( void ) noexcept;

        void output( ax::NodeEditor::PinId id ) noexcept;
        void endOutput( void ) noexcept;

    private:
        enum class Stage {
            Invalid,
            Begin,
            Header,
            Content,
            Input,
            Output,
            Middle,
            End
        };

        bool setStage( Stage stage ) noexcept;

        void pin( ax::NodeEditor::PinId id, ax::NodeEditor::PinKind kind ) noexcept;
        void endPin( void ) noexcept;

        ImTextureID m_headerTextureId;
        int m_headerTextureWidth;
        int m_headerTextureHeight;
        ax::NodeEditor::NodeId m_currentNodeId;
        Stage m_currentStage = Stage::Invalid;
        ImU32 m_headerColor = 0;
        ImVec2 m_nodeMin;
        ImVec2 m_nodeMax;
        ImVec2 m_headerMin;
        ImVec2 m_headerMax;
        ImVec2 m_contentMin;
        ImVec2 m_contentMax;
        bool m_hasHeader = false;
    };

}

#endif
