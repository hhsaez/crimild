#ifndef CRIMILD_CORE_EXPERIMENTAL_BLACKBOARD_
#define CRIMILD_CORE_EXPERIMENTAL_BLACKBOARD_

#include <crimild/core/Simulation/Clock.hpp>
#include <crimild/core/experimental/Resource.hpp>
#include <crimild/foundation/common/Key.hpp>
#include <crimild/math/Size2.hpp>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <variant>

namespace crimild {

   class Entity;

}

namespace crimild::experimental {

   class Node;
   class Camera3D;

   namespace detail {

      // Pointer inputs are stored weakly and read back through shared ownership.
      template< typename T >
      struct BlackboardTypes {
         using Stored = T;
         using Read = T;
      };

      template< typename T >
      struct BlackboardTypes< std::weak_ptr< T > > {
         using Stored = std::weak_ptr< T >;
         using Read = std::shared_ptr< T >;
      };

      template< typename T >
      struct BlackboardTypes< std::shared_ptr< T > > : BlackboardTypes< std::weak_ptr< T > > { };

   }

   /**
    * @brief Shared data accessed through typed keys or dynamic names
    *
    * Callers that need to exchange values use the same blackboard instance.
    * As a Resource, a blackboard is non-copyable; share its identity through handles.
    * Key names are owned by the blackboard, and get() returns a copy.
    * Entity references are always stored weakly, even when set() receives a shared
    * pointer. Reads lock them and return an optional shared pointer; expired or
    * empty references return nullopt. The object must have an owner elsewhere.
    */
   class Blackboard : public Resource {
      CRIMILD_IMPLEMENT_RTTI( crimild::experimental::Blackboard )

   public:
      using Value = std::variant<
         bool,
         uint32_t,
         float,
         Size2i,
         std::string,
         Clock,
         std::weak_ptr< Entity >,
         std::weak_ptr< Node >,
         std::weak_ptr< Camera3D > >;

      virtual ~Blackboard( void ) noexcept = default;

      /**
       * @brief Replace the named entry, converting pointer inputs to weak storage
       */
      template< typename T >
      void set( Key< T > key, std::type_identity_t< T > value )
      {
         set< T >( key.name, std::move( value ) );
      }

      // A shared-pointer read key accepts weak inputs, including derived objects.
      template< typename T, typename U >
      void set( Key< std::shared_ptr< T > > key, std::weak_ptr< U > value )
      {
         set( key.name, std::weak_ptr< T > { std::move( value ) } );
      }

      template< typename T >
      void set( std::string_view name, T value )
      {
         using Stored = typename detail::BlackboardTypes< T >::Stored;
         m_values.insert_or_assign(
            std::string { name },
            Value { std::in_place_type< Stored >, Stored { std::move( value ) } }
         );
      }

      /**
       * @brief Return a copy or locked shared pointer; nullopt for missing,
       * mismatched, empty, or expired entries
       */
      template< typename T >
      std::optional< typename detail::BlackboardTypes< T >::Read > get( Key< T > key ) const
      {
         return get< T >( key.name );
      }

      template< typename T >
      std::optional< typename detail::BlackboardTypes< T >::Read > get( std::string_view name ) const
      {
         using Types = detail::BlackboardTypes< T >;
         const auto it = m_values.find( std::string { name } );
         if ( it != m_values.end() ) {
            if ( const auto value = std::get_if< typename Types::Stored >( &it->second ) ) {
               if constexpr ( std::is_same_v< typename Types::Stored, typename Types::Read > ) {
                  return *value;
               } else if ( auto locked = value->lock() ) {
                  return locked;
               }
            }
         }
         return std::nullopt;
      }

      /**
       * @brief Check whether the name exists, regardless of its stored type
       *
       * Use get() to check whether an entry can be read with this key's type.
       */
      template< typename T >
      bool contains( Key< T > key ) const
      {
         return contains( key.name );
      }

      bool contains( std::string_view name ) const
      {
         return m_values.contains( std::string { name } );
      }

      /**
       * @brief Remove the named entry, returning whether it existed
       */
      template< typename T >
      bool erase( Key< T > key )
      {
         return erase( key.name );
      }

      bool erase( std::string_view name )
      {
         return m_values.erase( std::string { name } ) != 0;
      }

      void clear( void )
      {
         m_values.clear();
      }

   private:
      std::unordered_map< std::string, Value > m_values;
   };

}

#endif
