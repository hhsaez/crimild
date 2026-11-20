#ifndef CRIMILD_CORE_EXPERIMENTAL_BLACKBOARD_
#define CRIMILD_CORE_EXPERIMENTAL_BLACKBOARD_

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

   /**
    * @brief Shared data accessed through typed keys or dynamic names
    *
    * Callers that need to exchange values use the same blackboard instance.
    * Copying a blackboard copies its entries; Entity references remain shared.
    * Key names are owned by the blackboard, and get() returns a copy.
    */
   class Blackboard {
   public:
      using Value = std::variant<
         bool,
         uint32_t,
         float,
         Size2i,
         std::string,
         std::shared_ptr< Entity >,
         std::shared_ptr< Node >,
         std::shared_ptr< Camera3D > >;

      virtual ~Blackboard( void ) noexcept = default;

      /**
       * @brief Store a value of the key's declared type, replacing any existing entry
       */
      template< typename T >
      void set( Key< T > key, std::type_identity_t< T > value )
      {
         set< T >( key.name, std::move( value ) );
      }

      template< typename T >
      void set( std::string_view name, T value )
      {
         m_values.insert_or_assign(
            std::string { name },
            Value { std::in_place_type< T >, std::move( value ) }
         );
      }

      /**
       * @brief Return a copy, or nullopt if the name is absent or the type differs
       */
      template< typename T >
      std::optional< T > get( Key< T > key ) const
      {
         return get< T >( key.name );
      }

      template< typename T >
      std::optional< T > get( std::string_view name ) const
      {
         const auto it = m_values.find( std::string { name } );
         if ( it != m_values.end() ) {
            if ( const auto value = std::get_if< T >( &it->second ) ) {
               return *value;
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
