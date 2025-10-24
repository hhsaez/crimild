#ifndef CRIMILD_CORE_ENTITY_
#define CRIMILD_CORE_ENTITY_

#include "Entity/Extension.hpp"

namespace crimild {

   class Entity : public coding::Codable {
      CRIMILD_IMPLEMENT_RTTI( crimild::Entity )

   public:
      virtual ~Entity( void ) noexcept = default;

      template< typename ExtensionType >
      [[nodiscard]] bool hasExtension( void ) const
      {
         return m_extensions.find( ExtensionType::__CLASS_NAME ) != m_extensions.end();
      }

      template< typename ExtensionType, typename... Args >
      std::shared_ptr< ExtensionType > attach( Args &&...args )
      {
         auto extension = crimild::alloc< ExtensionType >( std::forward< Args >( args )... );
         attach( extension );
         return extension;
      }

      template< typename ExtensionType >
      std::shared_ptr< ExtensionType > getExtension( void ) const
      {
         auto it = m_extensions.find( ExtensionType::__CLASS_NAME );
         return it != m_extensions.end() ? crimild::cast_ptr< ExtensionType >( it->second ) : nullptr;
      }

      template< typename ExtensionType >
      std::shared_ptr< ExtensionType > getOrCreateExtension( void )
      {
         auto it = m_extensions.find( ExtensionType::__CLASS_NAME );
         if ( it != m_extensions.end() ) {
            return crimild::cast_ptr< ExtensionType >( it->second );
         }
         return attach< ExtensionType >();
      }

      template< typename ExtensionType >
      std::shared_ptr< ExtensionType > detach( void )
      {
         auto it = m_extensions.find( ExtensionType::__CLASS_NAME );
         if ( it == m_extensions.end() ) {
            return nullptr;
         }
         if ( it->second ) {
            it->second->setOwner( nullptr );
         }
         m_extensions.erase( it );
         return crimild::cast_ptr< ExtensionType >( it->second );
      }

      bool detach( std::shared_ptr< Extension > const &extension )
      {
         if ( !extension ) {
            return false;
         }

         auto it = m_extensions.find( extension->getClassName() );
         if ( it != m_extensions.end() && it->second == extension ) {
            extension->setOwner( nullptr );
            m_extensions.erase( it );
            return true;
         }
         return false;
      }

   private:
      void attach( std::shared_ptr< Extension > const &e )
      {
         m_extensions[ e->getClassName() ] = e;
         e->setOwner( retain( this ) );
      }

   private:
      std::unordered_map< std::string, std::shared_ptr< Extension > > m_extensions;

      /**
       * @name Coding
       *
       */
      //@{

   public:
      virtual void encode( coding::Encoder &encoder ) override;
      virtual void decode( coding::Decoder &decoder ) override;

      //@}
   };
}

#endif
