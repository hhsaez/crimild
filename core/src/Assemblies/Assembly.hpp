#ifndef CRIMILD_CORE_ASSEMBLIES_ASSEMBLY_
#define CRIMILD_CORE_ASSEMBLIES_ASSEMBLY_

#include "Entity/Entity.hpp"

namespace crimild {

   /**
    * @brief
    */
   class Assembly
      : public Entity,
        public Named {
      CRIMILD_IMPLEMENT_RTTI( crimild::Assembly )
   public:
      Assembly( void ) noexcept = default;
      explicit Assembly( std::string_view name ) noexcept;
      virtual ~Assembly( void ) noexcept = default;

      void addEntity( std::shared_ptr< Entity > const &entity );
      void removeEntity( std::shared_ptr< Entity > const &entity );
      inline const std::vector< std::shared_ptr< Entity > > &getEntities( void ) const { return m_entities; }

   private:
      std::vector< std::shared_ptr< Entity > > m_entities;

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
