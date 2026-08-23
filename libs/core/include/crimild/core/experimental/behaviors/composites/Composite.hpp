#ifndef CRIMILD_BEHAVIOR_COMPOSITES_COMPOSITE_
#define CRIMILD_BEHAVIOR_COMPOSITES_COMPOSITE_

#include <crimild/core/experimental/behaviors/Behavior.hpp>
#include <memory>
#include <vector>

namespace crimild::experimental::behaviors::composites {

   class Composite : public Behavior {
   protected:
      Composite( void ) noexcept = default;

   public:
      virtual ~Composite( void ) noexcept = default;

      void attach( std::shared_ptr< Behavior > const &behavior );

      inline const std::vector< std::shared_ptr< Behavior > > &getBehaviros( void ) const { return m_behaviors; }

   private:
      std::vector< std::shared_ptr< Behavior > > m_behaviors;
   };
}

#endif
