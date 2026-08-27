#ifndef CRIMILD_EXPERIMENTAL_SYSTEMS_SYSTEM_
#define CRIMILD_EXPERIMENTAL_SYSTEMS_SYSTEM_

#include <crimild/core/Simulation/Clock.hpp>
#include <crimild/math/Size2.hpp>
#include <unordered_set>

namespace crimild {

   class Entity;

   namespace experimental {

      class Node;

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
      inline constexpr Key< Size2i > RenderSize { "crimild.systems.render.size" };
      inline constexpr Key< std::shared_ptr< Node > > Scene { "crimild.systems.scene" };

   }

   class Context {
      using Value = std::variant<
         bool,
         uint32_t,
         float,
         Size2i,
         std::string,
         std::shared_ptr< Entity >,
         std::shared_ptr< Node > >;

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
         return m_values.contains( key.name ) || ( m_parent != nullptr && m_parent->contains( key ) );
      }

      void clear( void )
      {
         m_values.clear();
      }

   private:
      Context *m_parent = nullptr;
      std::unordered_map< std::string, Value > m_values;
   };

   class System {
   public:
      virtual ~System( void ) noexcept;

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

      void detachAll( void );

      inline std::unordered_set< std::shared_ptr< System > > &getChildren( void ) { return m_children; }

   private:
      std::unordered_set< std::shared_ptr< System > > m_children;

   public:
      enum class State {
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
      State m_state = State::SUCCESS;
   };

}

#endif
