#ifndef CRIMILD_NEXT_OBJECT_
#define CRIMILD_NEXT_OBJECT_

#include <Crimild_Foundation.hpp>
#include <Crimild_Coding.hpp>

namespace crimild::next {

    class Object
        : public RTTI,
          public coding::Codable {
        CRIMILD_IMPLEMENT_RTTI( crimild::next::Object )

    public:
        class Extension
            : public RTTI,
              public coding::Codable {
            CRIMILD_IMPLEMENT_RTTI( crimild::next::Object::Extension )
        };

        template< typename ExtensionType >
        [[nodiscard]] bool hasExtension( void ) const
        {
            return m_extensions.find( ExtensionType::__CLASS_NAME ) != m_extensions.end();
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

