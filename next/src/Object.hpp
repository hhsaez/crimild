#ifndef CRIMILD_NEXT_OBJECT_
#define CRIMILD_NEXT_OBJECT_

#include <Crimild_Coding.hpp>
#include <Crimild_Foundation.hpp>

namespace crimild::next {

   class Object : public coding::Codable {
      CRIMILD_IMPLEMENT_RTTI( crimild::next::Object )

   public:
      class Extension : public coding::Codable {
         CRIMILD_IMPLEMENT_RTTI( crimild::next::Object::Extension )
      };

      template< typename ExtensionType >
      [[nodiscard]] bool hasExtension( void ) const
      {
         return m_extensions.find( ExtensionType::__CLASS_NAME ) != m_extensions.end();
      }

      void attach( std::shared_ptr< Extension > const &extension )
      {
         m_extensions[ extension->getClassName() ] = extension;
      }

      void attach( std::string name, std::shared_ptr< Extension > const &extension )
      {
         m_extensions[ name ] = extension;
      }

      template< typename ExtensionType, typename... Args >
      std::shared_ptr< ExtensionType > attach( Args &&...args )
      {
         auto extension = crimild::alloc< ExtensionType >( std::forward< Args >( args )... );
         m_extensions[ extension->getClassName() ] = extension;
         return extension;
      }

      template< typename ExtensionType >
      std::shared_ptr< ExtensionType > getExtension( void ) const
      {
         auto it = m_extensions.find( ExtensionType::__CLASS_NAME );
         return it != m_extensions.end() ? crimild::cast_ptr< ExtensionType >( it->second ) : nullptr;
      }

   public:
      std::unordered_map< std::string, std::shared_ptr< Extension > > m_extensions;
   };

}

#endif
