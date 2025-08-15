#ifndef CRIMILD_UNIVERSAL_FOUNDATION_GLFW_SYSTEM_
#define CRIMILD_UNIVERSAL_FOUNDATION_GLFW_SYSTEM_

namespace crimild::universal {

   class GLFWSystem {
   public:
      GLFWSystem( void ) noexcept;
      ~GLFWSystem( void ) noexcept;

      inline bool isInitialized( void ) const
      {
         return m_initialized;
      }

      void update( void );

   private:
      bool m_initialized = false;
   };

}

#endif
