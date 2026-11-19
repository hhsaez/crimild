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
      virtual ~Composite( void ) noexcept;

      // Automatically detaches from the previous owner; cycles are logged and rejected.
      void attach( std::shared_ptr< Behavior > const &behavior );

      virtual void detach( std::shared_ptr< Behavior > const &behavior ) override;

      inline const std::vector< std::shared_ptr< Behavior > > &getBehaviors( void ) const { return m_behaviors; }

   protected:
      // Overrides should chain here to abort running children.
      virtual void terminate( void ) override;

   private:
      std::vector< std::shared_ptr< Behavior > > m_behaviors;
   };

}

#endif
