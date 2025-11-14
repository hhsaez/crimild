#ifndef CRIMILD_CORE_COMMON_SIGNAL_
#define CRIMILD_CORE_COMMON_SIGNAL_

#include <algorithm>
#include <functional>
#include <memory>
#include <vector>

namespace crimild {

   /**
    * @brief A lightweight, single-threaded publish/subscribe helper class.
    *
    * Stores callable "connections" with unique ids, lets clients bind/unbind
    * handlers and dispatches the handlers when invoked.
    *
    * - Handlers live in a mutable collection of Connection objects;
    *   each connection tracks an id, callable, and optional owner weak
    *   pointer.
    * - Overloaded `bind()` variants cover lambdas/functors, `shared_ptr`
    *   owners (auto cleanup), and raw pointers (manual lifetime
    *   responsibility).
    * - `operator()` is const and begins by calling `cleanupExpiredConnections()`;
    *   each dispatch then scans for the smallest id greater than the last
    *   executed one so handlers can clear, unbind, or bind safely during
    *   iteration (O(N²) worst case, but deterministic).
    * - Connection ids increase monotonically via `m_nextId`; `clear()` does
    *   not reset the counter, so id uniqueness must hold even when handlers
    *   churn.
    * - `cleanupExpiredConnections()` is const but mutates the vector thanks
    *   to the mutable storage.
    *
    * @warning: This class is not thread-safe.
    */
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
         auto handler = [ object = std::weak_ptr< T >( object ), method ]( Args... args ) {
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

         // This is far from optimal, but allows mutating the connections
         // when we're triggering signals.
         // Keep traversal state using connection ids rather than iterators.
         // After each handler runs, find the smallest id greater than the one
         // just executed. This accepts an O(N^2) upper bound but simplifies
         // bookkepping and still meets the desired behavior.
         ConnectionId lastId = 0;
         while ( true ) {
            Connection *next = nullptr;
            ConnectionId nextId = std::numeric_limits< ConnectionId >::max();

            for ( auto &conn : m_connections ) {
               if ( conn.id > lastId && conn.id < nextId ) {
                  next = &conn;
                  nextId = conn.id;
               }
            }

            if ( next == nullptr ) {
               break; // no more handlers left with id > lastId
            }

            auto handler = next->handler; // copy callable to survive erasure
            lastId = nextId;
            handler( args... );
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
