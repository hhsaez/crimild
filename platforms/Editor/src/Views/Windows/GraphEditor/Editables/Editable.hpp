#ifndef CRIMILD_EDITOR_EDITABLE_
#define CRIMILD_EDITOR_EDITABLE_

#include "Views/Windows/GraphEditor/GraphEditorContext.hpp"
#include "Views/Windows/GraphEditor/GraphEditorTypes.hpp"

#include <Crimild.hpp>

namespace crimild::editor::editables {

   class Editable : public crimild::Extension {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::editables::Editable )

   public:
      static std::shared_ptr< Editable > getOrCreate( std::shared_ptr< Entity > const &entity );

   public:
      class Renderer : public crimild::Entity {
      public:
         virtual ~Renderer( void ) noexcept = default;

         virtual void render( GraphEditorContext &ctx, Editable *editable ) = 0;
         virtual void renderLinks( GraphEditorContext &ctx, Editable *editable ) = 0;
      };

   public:
      Editable( void ) noexcept = default;

      explicit Editable( std::shared_ptr< Renderer > const &renderer ) noexcept
         : m_renderer( renderer )
      {
         // no-op
      }

      virtual ~Editable( void ) = default;

      [[nodiscard]] bool hasInputPin( std::string pinName ) const { return m_inputs.contains( pinName ); }
      [[nodiscard]] inline Pin &getInputPin( std::string pinName ) { return m_inputs.at( pinName ); }
      void inline setInputPin( std::string pinName, Pin pin ) { m_inputs[ pinName ] = pin; }

      [[nodiscard]] bool hasOutputPin( std::string pinName ) const { return m_outputs.contains( pinName ); }
      [[nodiscard]] inline Pin &getOutputPin( std::string pinName ) { return m_outputs.at( pinName ); }
      void inline setOutputPin( std::string pinName, Pin pin ) { m_outputs[ pinName ] = pin; }

      [[nodiscard]] Pin *findPin( ax::NodeEditor::PinId id )
      {
         for ( auto &it : m_inputs ) {
            if ( it.second.id == id ) {
               return &it.second;
            }
         }
         for ( auto &it : m_outputs ) {
            if ( it.second.id == id ) {
               return &it.second;
            }
         }
         return nullptr;
      }

      // inline ax::NodeEditor::NodeId getId( void ) const { return m_id; }

      // inline const std::string &getName( void ) const { return m_name; }

      inline const Vector2 &getPosition( void ) const { return m_position; }
      inline void setPosition( const Vector2 &position ) { m_position = position; }

      void render( GraphEditorContext &ctx );
      void renderLinks( GraphEditorContext &ctx );

   private:
      bool m_initialized = false;
      Vector2 m_position = Vector2::Constants::ZERO;

      std::unordered_map< std::string, Pin > m_inputs;
      std::unordered_map< std::string, Pin > m_outputs;

      std::shared_ptr< Renderer > m_renderer;
      // std::string m_name;
      // NodeType m_type;

      // ImColor color;
      // ImVec2 size;

      // std::string state;
      // std::string savedState;

      /**
       * @name Coding
       */
      //@{

   public:
      virtual void encode( coding::Encoder &encoder ) override;
      virtual void decode( coding::Decoder &decoder ) override;

      //@}
   };

}

#endif
