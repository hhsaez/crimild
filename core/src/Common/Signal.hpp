#ifndef CRIMILD_CORE_COMMON_SIGNAL_
#define CRIMILD_CORE_COMMON_SIGNAL_

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace crimild {

   template< typename... Args >
   class Signal {
   public:
      using HandlerType = std::function< void( Args... ) >;
      using ConnectionId = size_t;

      // Bind a lambda function pointer
      ConnectionId bind( HandlerType handler )
      {
         auto id = m_nextId++;
         m_connections.emplace_back( id, std::move( handler ) );
         return id;
      }

      // Connect a member function with an automatic cleanup when object is destroyed
      template< typename T >
      ConnectionId bind( std::shared_ptr< T > const &object, void ( T::*method )( Args... ) )
      {
         auto id = m_nextId++;
         auto handler = [ object, method ]( Args... args ) {
            if ( auto ptr = object.lock() ) {
               ( ptr.get()->*method )( args... );
            }
         };
         m_connections.emplace_back( id, std::move( handler ), object );
         return id;
      }

      // Connect a member function with a raw pointer (manual lifetime management)
      template< typename T >
      ConnectionId bind( T *object, void ( T::*method )( Args... ) )
      {
         auto id = m_nextId++;
         auto handler = [ object, method ]( Args... args ) {
            ( object->*method )( args... );
         };
         m_connections.emplace_back( id, std::move( handler ) );
         return id;
      }

      // Disconnect by connection id
      bool unbind( ConnectionId id )
      {
         auto it = std::find_if(
            m_connections.begin(),
            m_connections.end(),
            [ id ]( const Connection &conn ) {
               return conn.id == id;
            }
         );
         if ( it != m_connections.end() ) {
            m_connections.erase( it );
            return true;
         }
         return false;
      }

      // Unbind all handlers for a specific object
      template< typename T >
      void unbind( std::shared_ptr< T > const object )
      {
         m_connections.erase(
            std::remove_if(
               m_connections.begin(),
               m_connections.end(),
               [ &object ]( const Connection &conn ) {
                  return !conn.owner.expired() && conn.owner.lock() == object;
               }
            ),
            m_connections.end()
         );
      }

      void operator()( Args... args ) const
      {
         // Cleanup expired connections first
         cleanupExpiredConnections();

         // Call all handlers
         for ( const auto &conn : m_connections ) {
            conn.handler( args... );
         }
      }

      void clear()
      {
         m_connections.clear();
      }

      inline size_t size() const
      {
         return m_connections.size();
      }

   private:
      void cleanupExpiredConnections() const
      {
         m_connections.erase(
            std::remove_if(
               m_connections.begin(),
               m_connections.end(),
               []( const Connection &conn ) {
                  return conn.hasOwner && conn.owner.expired();
               }
            ),
            m_connections.end()
         );
      }

   private:
      struct Connection {
         ConnectionId id;
         HandlerType handler;
         bool hasOwner = false;
         std::weak_ptr< void > owner; // For automatic cleanup when object is destroyed

         Connection( ConnectionId id, HandlerType handler ) noexcept
            : id( id ),
              handler( std::move( handler ) ),
              hasOwner( false )
         {
            // no-op
         }

         Connection( ConnectionId id, HandlerType handler, std::weak_ptr< void > owner ) noexcept
            : id( id ),
              handler( std::move( handler ) ),
              hasOwner( true ),
              owner( std::move( owner ) )
         {
            // no-op
         }
      };

      mutable std::vector< Connection > m_connections;
      mutable ConnectionId m_nextId = 1;
   };

}

#endif
