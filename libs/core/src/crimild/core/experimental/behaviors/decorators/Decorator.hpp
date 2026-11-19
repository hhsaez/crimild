#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_DECORATORS_DECORATOR_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_DECORATORS_DECORATOR_

#include <crimild/core/experimental/behaviors/Behavior.hpp>

namespace crimild::experimental::behaviors::decorators {

   class Decorator : public Behavior {
   protected:
      Decorator( void ) noexcept = default;

   public:
      virtual ~Decorator( void ) noexcept;

      // Automatically reparents the child; null detaches it, and cycles are rejected.
      void attach( std::shared_ptr< Behavior > const &behavior );

      virtual void detach( std::shared_ptr< Behavior > const &behavior ) override;

      inline std::shared_ptr< Behavior > getBehavior( void ) const { return m_behavior; }

   protected:
      // Overrides should chain here to abort the running child.
      virtual void terminate( void ) override;

   private:
      std::shared_ptr< Behavior > m_behavior;
   };

}

#endif
