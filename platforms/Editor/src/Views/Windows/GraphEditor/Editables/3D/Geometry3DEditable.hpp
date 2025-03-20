#ifndef CRIMILD_EDITOR_EDITABLES_3D_GEOMETRY3D_
#define CRIMILD_EDITOR_EDITABLES_3D_GEOMETRY3D_

#include "Views/Windows/GraphEditor/Editables/3D/Node3DEditable.hpp"

namespace crimild::editor::editables {

   class Geometry3DEditable : public Node3DEditable {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::Geoemtry3DEditable)
   
   public:
      explicit Geometry3DEditable( GraphEditorContext &ctx, std::string_view name = "Geometry3D" ) noexcept
         : Node3DEditable( ctx, name )
      {
         // no-op
      }

      virtual ~Geometry3DEditable( void ) noexcept = default;

   };

}

#endif
