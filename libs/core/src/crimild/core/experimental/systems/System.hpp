#ifndef CRIMILD_EXPERIMENTAL_SYSTEMS_SYSTEM_
#define CRIMILD_EXPERIMENTAL_SYSTEMS_SYSTEM_

#include <crimild/core/Entity/Entity.hpp>
#include <crimild/core/Simulation/Clock.hpp>
#include <crimild/foundation/common/Enableable.hpp>
#include <crimild/foundation/memory/Memory.hpp>
#include <crimild/math/Size2.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace crimild {

   class Entity;

   namespace experimental {

      class Node;
      class Camera3D;

   }

}

namespace crimild {

   /**
    * @brief Typed key for key/value containers
    */
   template< typename T >
   struct Key {
      std::string_view name;
   };

}

namespace crimild::experimental::systems {

   namespace keys {

      inline constexpr Key< Size2i > WindowSize { "crimild.systems.window.size" };
      inline constexpr Key< Size2i > FramebufferSize { "crimild.systems.framebuffer.size" };
      inline constexpr Key< Size2i > RenderSize { "crimild.systems.render.size" };
      inline constexpr Key< std::shared_ptr< Node > > Scene { "crimild.systems.scene" };
      inline constexpr Key< std::shared_ptr< Camera3D > > Camera { "crimild.systems.camera" };

   }

   class Context {
      using Value = std::variant<
         bool,
         uint32_t,
         float,
         Size2i,
         std::string,
         std::shared_ptr< Entity >,
         std::shared_ptr< Node >,
         std::shared_ptr< Camera3D > >;

   public:
      virtual ~Context( void ) noexcept = default;

      template< typename T >
      void set( Key< T > key, T value )
      {
         m_values.insert_or_assign(
            std::string { key.name },
            Value { value }
         );
      }

      /**
       * @brief Get a value from the context, if it exists
       *
       * @remarks Always returns a copy
       */
      template< typename T >
      std::optional< T > get( Key< T > key ) const
      {
         auto it = m_values.find( std::string { key.name } );
         if ( it == m_values.end() ) {
            if ( auto parent = m_parent ) {
               return parent->get< T >( key );
            } else {
               return std::nullopt;
            }
         }

         if ( auto value = std::get_if< T >( &it->second ) ) {
            return *value;
         }

         // type mismatch?
         return std::nullopt;
      }

      template< typename T >
      bool contains( Key< T > key ) const
      {
         return m_values.contains( std::string { key.name } ) || ( m_parent != nullptr && m_parent->contains( key ) );
      }

      void clear( void )
      {
         m_values.clear();
      }

   private:
      Context *m_parent = nullptr;
      std::unordered_map< std::string, Value > m_values;
   };

   class System;

   template< class T >
   concept IsSystem = std::is_base_of_v< System, T >;

   class System
      : public Entity,
        public Enableable {
   public:
      virtual ~System( void ) noexcept;

      inline bool hasParent( void ) const
      {
         return m_parent != nullptr;
      }

      std::shared_ptr< System > getParent( void ) const
      {
         return retain( m_parent );
      }

      template< IsSystem ParentSystemType >
      std::shared_ptr< ParentSystemType > getParent( void ) const
      {
         return std::static_pointer_cast< ParentSystemType >( retain( m_parent ) );
      }

      std::shared_ptr< System > detachFromParent( void );

   private:
      void setParent( System *parent );

   private:
      /**
       * @brief Parent system
       *
       * Use a raw pointer so we can attach child systems during
       * construction.
       */
      System *m_parent = nullptr;

   public:
      inline bool hasChildren( void ) const { return !m_children.empty(); }

      template< typename SystemType >
      std::shared_ptr< SystemType > attach( void )
      {
         auto system = std::make_shared< SystemType >();
         attach( system );
         return system;
      }

      void attach( std::shared_ptr< System > const &system );

      void detach( std::shared_ptr< System > const &child );

      void detachAll( void );

      inline std::unordered_set< std::shared_ptr< System > > &getChildren( void ) { return m_children; }

   private:
      std::unordered_set< std::shared_ptr< System > > m_children;

   public:
      enum class State {
         READY,
         RUNNING,
         SUCCESS,
         FAILURE,
      };

      inline State getState( void ) const { return m_state; }

      State update( const Context &ctx );

   protected:
      inline void setState( State state ) { m_state = state; }

      virtual State onUpdate( const Context &ctx );

   private:
      State m_state = State::READY;
   };

}

#endif
