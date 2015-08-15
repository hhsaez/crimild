#ifndef CRIMILD_GLES_SIMULATION_
#define CRIMILD_GLES_SIMULATION_

#include <Crimild.hpp>

namespace crimild {
    
    namespace gles {
        
        class GLESSimulation : public crimild::Simulation {
        public:
            GLESSimulation( void );
            virtual ~GLESSimulation( void );
        };
        
    }
    
}

#endif

