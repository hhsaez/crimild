#ifndef CRIMILD_EXPERIMENTAL_SYSTEMS_SYSTEM_
#define CRIMILD_EXPERIMENTAL_SYSTEMS_SYSTEM_

#include <unordered_set>

namespace crimild::experimental::systems {

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

      State update( void );

   protected:
      inline void setState( State state ) { m_state = state; }

      virtual State onUpdate( void );

   private:
      State m_state = State::SUCCESS;
   };

}

#endif
