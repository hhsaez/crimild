#ifndef CRIMILD_RAYTRACING_RENDERING_MATERIAL_
#define CRIMILD_RAYTRACING_RENDERING_MATERIAL_

#include "Components/NodeComponent.hpp"
#include "Mathematics/ColorRGB.hpp"
#include "Mathematics/min.hpp"

namespace crimild {

    namespace raytracing {

        class RTMaterial : public NodeComponent {
            CRIMILD_IMPLEMENT_RTTI( crimild::RTMaterial )

        public:
            enum class Type {
                LAMBERTIAN,
                METALLIC,
                DIELECTRIC
            };

        public:
            RTMaterial( void );
            virtual ~RTMaterial( void );

            void setType( Type type ) { _type = type; }
            Type getType( void ) const { return _type; }

            const ColorRGB &getAlbedo( void ) const { return _albedo; }
            void setAlbedo( const ColorRGB &albedo ) { _albedo = albedo; }

            float getFuzz( void ) const { return _fuzz; }
            void setFuzz( float value ) { _fuzz = min( value, 1.0f ); }

            float getRefractionIndex( void ) const { return _refractionIndex; }
            void setRefractionIndex( float value ) { _refractionIndex = value; }

        private:
            Type _type;
            ColorRGB _albedo;
            float _fuzz;
            float _refractionIndex;
        };

    }

}

#endif
