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

#ifndef CRIMILD_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_
#define CRIMILD_EDITOR_VIEWS_WINDOWS_GRAPH_EDITOR_

#include "Foundation/ImGuiUtils.hpp"
#include "Views/Windows/GraphEditor/Editables/Editable.hpp"
#include "Views/Windows/GraphEditor/GraphEditorTypes.hpp"
#include "Views/Windows/Window.hpp"

#define GRAPH_EDITOR_BLUEPRINTS 1

namespace crimild::editor {

#if GRAPH_EDITOR_BLUEPRINTS

   namespace utils {

      class AssemblyNodeBuilder;

   }

#else

   // Holds basic information about connections between
   // pins. Note that connection (aka, link) has its own
   // ID. This is useful later with dealing with selections,
   // deletion and other operations.
   struct LinkInfo {
      ax::NodeEditor::LinkId id;
      ax::NodeEditor::PinId inputPinId;
      ax::NodeEditor::PinId outputPinId;
   };

#endif

   class GraphEditorWindow : public Window {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::GraphEditorWindow )

   public:
      GraphEditorWindow( void ) noexcept;
      ~GraphEditorWindow( void ) noexcept;

      void drawContent( void ) noexcept final;

   private:
      void updateTouch( void ) noexcept;
      void showLeftPanel( float panelWidth ) noexcept;

#if GRAPH_EDITOR_BLUEPRINTS

      GraphNode *findNode( ax::NodeEditor::NodeId id ) noexcept;
      Link *findLink( ax::NodeEditor::LinkId id ) noexcept;
      Pin *findPin( ax::NodeEditor::PinId id ) noexcept;

      //   inline int getNextId( void ) noexcept { return m_nextId++; }
      //   inline ax::NodeEditor::LinkId getNextLinkId( void ) noexcept { return ax::NodeEditor::LinkId( getNextId() ); }

      bool canCreateLink( Pin *a, Pin *b ) const noexcept
      {
         if ( !a || !b || a == b || a->kind == b->kind || a->type != b->type || a->node == b->node ) {
            return false;
         }
         return true;
      }

      bool isPinkLinked( ax::NodeEditor::PinId id ) const noexcept
      {
         if ( !id ) {
            return false;
         }

         for ( auto &link : m_links ) {
            if ( link.startPinId == id || link.endPinId == id ) {
               return true;
            }
         }

         return false;
      }

      void drawPinIcon( const Pin &pin, bool connected, int alpha ) const noexcept;

      GraphNode *spawnInputActionNode( void ) noexcept;
      GraphNode *spawnOutputActionNode( void ) noexcept;
      GraphNode *spawnBranchNode( void ) noexcept;
      GraphNode *spawnDoNNode( void ) noexcept;
      GraphNode *spawnSetTimerNode( void ) noexcept;
      GraphNode *spawnLessNode( void ) noexcept;
      GraphNode *spawnWeirdNode( void ) noexcept;
      GraphNode *spawnTraceByChannelNode( void ) noexcept;
      GraphNode *spawnPrintStringNode( void ) noexcept;
      GraphNode *spawnCommentNode( void ) noexcept;
      GraphNode *spawnTreeSequenceNode( void ) noexcept;
      GraphNode *spawnTreeMoveToNode( void ) noexcept;
      GraphNode *spawnTreeRandomWaitNode( void ) noexcept;
      GraphNode *spawnMessageNode( void ) noexcept;
      GraphNode *spawnHoudiniTransformNode( void ) noexcept;
      GraphNode *spawnHoudiniGroupNode( void ) noexcept;

      void buildNodes( void ) noexcept
      {
         for ( auto &node : m_nodes ) {
            buildNode( &node );
         }
      }

      void buildNode( GraphNode *node ) noexcept
      {
         for ( auto &input : node->inputs ) {
            input.node = node;
            input.kind = PinKind::Input;
         }
         for ( auto &output : node->outputs ) {
            output.node = node;
            output.kind = PinKind::Output;
         }
      }

      ImColor getIconColor( PinType type ) const noexcept;

      void renderBlueprintAndSimpleNodes( utils::AssemblyNodeBuilder &builder ) noexcept;
      void renderTreeNodes( void ) noexcept;
      void renderHoudiniNodes( void ) noexcept;
      void renderCommentNodes( void ) noexcept;
      void renderLinks( void ) noexcept;
      void renderCreateNewNode( void ) noexcept;
      void renderPopups( void ) noexcept;
      void renderNodeContextMenu( void ) noexcept;
      void renderPinContextMenu( void ) noexcept;
      void renderLinkContextMenu( void ) noexcept;
      void renderCreateNewNodeMenu( void ) noexcept;
      void showOrdinals( void ) noexcept;

      inline void touchNode( ax::NodeEditor::NodeId id ) noexcept
      {
         m_nodeTouchTime[ id ] = m_touchTime;
      }

      inline float getTouchProgress( ax::NodeEditor::NodeId id ) const noexcept
      {
         auto it = m_nodeTouchTime.find( id );
         if ( it != m_nodeTouchTime.end() && it->second > 0.0f ) {
            return ( m_touchTime - it->second ) / m_touchTime;
         }
         return 0.0f;
      }

      void showLeftPanel( void );
      void showStyleEditor();
#endif

   private:
      // Editor context
      // Required to trace editor state.
      ax::NodeEditor::EditorContext *m_context = nullptr;

      GraphEditorContext m_ctx;

#if GRAPH_EDITOR_BLUEPRINTS
      int m_nextId = 1;
      const int m_pinIconSize = 24;
      std::vector< GraphNode > m_nodes;
      std::vector< Link > m_links;
      ImTextureID m_headerBackground = nullptr;
      ImTextureID m_saveIcon = nullptr;
      ImTextureID m_restoreIcon = nullptr;
      const float m_touchTime = 1.0f;
      std::map< ax::NodeEditor::NodeId, float, NodeIdLess > m_nodeTouchTime;
      bool m_showOrdinals = false;

      bool m_createNewNode = false;
      Pin *m_newNodeLinkPin = nullptr;
      Pin *m_newLinkPin = nullptr;

      ax::NodeEditor::NodeId m_contextNodeId = 0;
      ax::NodeEditor::LinkId m_contextLinkId = 0;
      ax::NodeEditor::PinId m_contextPinId = 0;

      float m_leftPanelWidth = 400.0f;
      float m_rightPanelWidth = 800.0f;
      bool m_showStyleEditor = false;
      int m_changeCount = 0;

#else
      // Flag set for first frame only
      // This is required for some actions that need to be executed only once
      bool m_firstFrame = true;

      // List of live links
      // It is dynamic unless you want to create a read-only view over nodes
      ImVector< LinkInfo > m_links;

      // Counter to help generate link ids.
      // In a real application this will probably based on pointer to user
      // data structures
      int m_nextLinkId = 100;
#endif
   };
}

#endif
