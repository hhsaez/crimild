#ifndef CRIMILD_CORE_ENTITY_EXTENSION_
#define CRIMILD_CORE_ENTITY_EXTENSION_

#include <Crimild_Coding.hpp>

namespace crimild {

   class Extension : public coding::Codable {
      CRIMILD_IMPLEMENT_RTTI( crimild::Extension )
      friend class Entity;

   public:
      virtual ~Extension( void ) = default;

      [[nodiscard]] inline std::shared_ptr< Entity > getOwner( void ) { return m_owner.lock(); }

      template< class OwnerType >
      [[nodiscard]] inline std::shared_ptr< OwnerType > getOwner( void )
      {
         return std::static_pointer_cast< OwnerType >( m_owner.lock() );
      }

   private:
      inline void setOwner( std::shared_ptr< Entity > const &owner ) { m_owner = owner; }

   private:
      std::weak_ptr< Entity > m_owner;
   };

}

#endif
