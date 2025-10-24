#ifndef CRIMILD_EDITOR_VIEWS_WORKSPACES_ASSEMBLY_
#define CRIMILD_EDITOR_VIEWS_WORKSPACES_ASSEMBLY_

#include "Views/View.hpp"

namespace crimild::editor {

   class Assembly;

   class AssemblyWorkspace : public View {
      CRIMILD_IMPLEMENT_RTTI( crimild::editor::AssemblyWorkspace )

   public:
      AssemblyWorkspace( void ) noexcept = default;

      explicit AssemblyWorkspace(
         std::filesystem::path assemblyPath,
         std::shared_ptr< Assembly > const &assembly
      ) noexcept;

      virtual ~AssemblyWorkspace( void ) noexcept = default;

      virtual void draw( void ) noexcept override;
      virtual void drawContent( void ) noexcept override;

   private:
      std::filesystem::path m_assemblyPath;
      std::shared_ptr< Assembly > m_assembly;

      /**
       * @name Coding
       */
      //@{

   public:
      virtual void encode( coding::Encoder &encoder ) noexcept override;
      virtual void decode( coding::Decoder &decoder ) noexcept override;

      //@}
   };
}

#endif
