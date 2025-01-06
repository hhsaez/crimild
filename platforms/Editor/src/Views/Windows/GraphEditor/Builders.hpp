#ifndef CRIMILD_PLATFORM_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_BUILDERS_
#define CRIMILD_PLATFORM_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_BUILDERS_

#include "Foundation/ImGuiUtils.hpp"

namespace crimild::editor::utils {

    /**
     * @class AssemblyNodeBuilder
     * @brief A utility class for building assembly nodes in a graphical editor.
     *
     * This class provides methods to construct and manage the layout of assembly nodes,
     * including headers, inputs, outputs, and middle sections.
     */
    class AssemblyNodeBuilder {
    public:
        /**
         * @brief Constructor for AssemblyNodeBuilder.
         * @param texture The texture ID for the node header.
         * @param textureWidth The width of the texture.
         * @param textureHeight The height of the texture.
         */
        AssemblyNodeBuilder( ImTextureID texture = nullptr, int textureWidth = 0, int textureHeight = 0 ) noexcept;

        /**
         * @brief Begins the construction of a new node.
         * @param id The unique identifier for the node.
         */
        void begin( ax::NodeEditor::NodeId id ) noexcept;

        /**
         * @brief Ends the construction of the current node.
         */
        void end( void ) noexcept;

        /**
         * @brief Begins the header section of the node.
         * @param color The color of the header.
         */
        void header( const ImVec4 &color = ImVec4( 1, 1, 1, 1 ) ) noexcept;

        /**
         * @brief Ends the header section of the node.
         */
        void endHeader( void ) noexcept;

        /**
         * @brief Begins an input pin section.
         * @param id The unique identifier for the input pin.
         */
        void input( ax::NodeEditor::PinId id ) noexcept;

        /**
         * @brief Ends the input pin section.
         */
        void endInput( void ) noexcept;

        /**
         * @brief Begins the middle section of the node.
         */
        void middle( void ) noexcept;

        /**
         * @brief Begins an output pin section.
         * @param id The unique identifier for the output pin.
         */
        void output( ax::NodeEditor::PinId id ) noexcept;

        /**
         * @brief Ends the output pin section.
         */
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

        /**
         * @brief Sets the current stage of node construction.
         * @param stage The stage to set.
         * @return True if the stage was set successfully, false otherwise.
         */
        bool setStage( Stage stage ) noexcept;

        /**
         * @brief Begins a pin section.
         * @param id The unique identifier for the pin.
         * @param kind The kind of pin (input or output).
         */
        void pin( ax::NodeEditor::PinId id, ax::NodeEditor::PinKind kind ) noexcept;

        /**
         * @brief Ends the current pin section.
         */
        void endPin( void ) noexcept;

        // Member variables documentation
        ImTextureID m_headerTextureId;          ///< The texture ID for the node header.
        int m_headerTextureWidth;               ///< The width of the header texture.
        int m_headerTextureHeight;              ///< The height of the header texture.
        ax::NodeEditor::NodeId m_currentNodeId; ///< The unique identifier for the current node.
        Stage m_currentStage = Stage::Invalid;  ///< The current stage of node construction.
        ImU32 m_headerColor = 0;                ///< The color of the header.
        ImVec2 m_nodeMin;                       ///< The minimum coordinates of the node.
        ImVec2 m_nodeMax;                       ///< The maximum coordinates of the node.
        ImVec2 m_headerMin;                     ///< The minimum coordinates of the header.
        ImVec2 m_headerMax;                     ///< The maximum coordinates of the header.
        ImVec2 m_contentMin;                    ///< The minimum coordinates of the content.
        ImVec2 m_contentMax;                    ///< The maximum coordinates of the content.
        bool m_hasHeader = false;               ///< Flag indicating whether the node has a header.
    };

}

#endif
