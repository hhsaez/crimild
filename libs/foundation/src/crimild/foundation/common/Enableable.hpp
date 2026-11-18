#ifndef CRIMILD_FOUNDATION_ENABLEABLE_
#define CRIMILD_FOUNDATION_ENABLEABLE_

namespace crimild {

   /**
    * @brief Helper class for class that can be enabled/disabled
    */
   class Enableable {
   protected:
      Enableable( void ) noexcept = default;
      ~Enableable( void ) noexcept = default;

   public:
      inline void setEnabled( bool enabled ) { m_enabled = enabled; }
      inline bool isEnabled( void ) const { return m_enabled; }

      inline bool toggleEnabled( void )
      {
         return m_enabled = !m_enabled;
      }

   private:
      bool m_enabled = true;
   };

}

#endif
