#ifndef CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_SEQUENCE_
#define CRIMILD_EXPERIMENTAL_BEHAVIORS_COMPOSITES_SEQUENCE_

#include <crimild/core/experimental/behaviors/composites/Composite.hpp>

namespace crimild::experimental::behaviors::composites {

   class Sequence : public Composite {
   public:
      virtual ~Sequence( void ) noexcept = default;

      virtual State onExecute( void ) override;
   };

}

#endif
