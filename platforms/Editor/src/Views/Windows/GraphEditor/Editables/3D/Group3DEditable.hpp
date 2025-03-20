#ifndef CRIMILD_EDITOR_EDITABLES_3D_GROUP3D_
#define CRIMILD_EDITOR_EDITABLES_3D_GROUP3D_

#include "Views/Windows/GraphEditor/Editables/3D/Node3DEditable.hpp"

namespace crimild::editor::editables {

   class Group3DEditable : public Node3DEditable {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::Group3DEditable)
   
   public:
      explicit Group3DEditable( GraphEditorContext &ctx, std::string_view name = "Group3D" ) noexcept
         : Node3DEditable( ctx, name )
      {
         getOutputs().emplace_back( ctx, this, "", PinType::Flow );
      }

      virtual ~Group3DEditable( void ) noexcept = default;
   
   };

}

#endif
