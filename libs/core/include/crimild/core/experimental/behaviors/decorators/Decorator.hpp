#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_DECORATORS_DECORATOR_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_DECORATORS_DECORATOR_

#include <crimild/core/experimental/behaviors/Behavior.hpp>

namespace crimild::experimental::behaviors::decorators {

   class Decorator : public Behavior {
   protected:
      Decorator( void ) noexcept = default;

   public:
      virtual ~Decorator( void ) noexcept = default;

      void attach( std::shared_ptr< Behavior > const &behavior );

      inline std::shared_ptr< Behavior > getBehavior( void ) const { return m_behavior; }

   private:
      std::shared_ptr< Behavior > m_behavior;
   };

}

#endif
