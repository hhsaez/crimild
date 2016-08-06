/*
 * Copyright (c) 2013, Hernan Saez
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the <organization> nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_SCENE_GRAPH_PARTICLE_SYSTEM_
#define CRIMILD_SCENE_GRAPH_PARTICLE_SYSTEM_

#include "Geometry.hpp"

namespace crimild {

	class Texture;
	class Material;

    class ParticleEmitter : public NonCopyable {
    protected:
        ParticleEmitter( void ) { }

    public:
        virtual ~ParticleEmitter( void ) { }

        void generate( Vector3f &position, Vector3f &velocity ) const;

    protected:
        virtual void evaluate( Vector3f &position, Vector3f &velocity ) const = 0;

    public:
        void setTransformation( const Transformation &t ) { _transformation = t; }
        const Transformation &getTransformation( void ) const { return _transformation; }

    private:
        Transformation _transformation;
    };

    class PointParticleEmitter : public ParticleEmitter {
    public:
        PointParticleEmitter( void ) { }
        virtual ~PointParticleEmitter( void ) { }

    protected:
        virtual void evaluate( Vector3f &position, Vector3f &velocity ) const override;
    };

    class ConeParticleEmitter : public ParticleEmitter {
    public:
        ConeParticleEmitter( float h, float r ) : _height( h ), _radius( r ) { }

        virtual ~ConeParticleEmitter( void ) { }

    protected:
        virtual void evaluate( Vector3f &position, Vector3f &velocity ) const override;

    private:
        float _height;
        float _radius;
    };

    class CylinderParticleEmitter : public ParticleEmitter {
    public:
        CylinderParticleEmitter( float h, float r ) : _height( h ), _radius( r ) { }

        virtual ~CylinderParticleEmitter( void ) { }

    protected:
        virtual void evaluate( Vector3f &position, Vector3f &velocity ) const override;

    private:
        float _height;
        float _radius;
    };

    class SphereParticleEmitter : public ParticleEmitter {
    public:
        SphereParticleEmitter( float r ) : _radius( r ) { }

        virtual ~SphereParticleEmitter( void ) { }

    protected:
        virtual void evaluate( Vector3f &position, Vector3f &velocity ) const override;

    private:
        float _radius;
    };

    /*
        TODO:
        - looping/no-looping
    */
    class ParticleSystem : public Geometry {
    public:
        ParticleSystem( void );
        virtual ~ParticleSystem( void );

    private:
        struct Particle {
            float time;
            Vector3f position;
            Vector3f velocity;
        };

        std::vector< Particle > _particles;

    public:
        void setMaxParticles( unsigned int value ) { _maxParticles = value; }
        unsigned int getMaxParticles( void ) const { return _maxParticles; }

        void setPreComputeParticles( bool value ) { _preComputeParticles = value; }
        bool shouldPreComputeParticles( void ) const { return _preComputeParticles; }

        void setUseWorldSpace( bool value ) { _useWorldSpace = value; }
        bool useWorldSpace( void ) const { return _useWorldSpace; }

    private:
        unsigned int _maxParticles = 10;
        bool _preComputeParticles = false;
        bool _useWorldSpace = false;

    public:
        void setParticleLifetime( float value ) { _particleLifetime = value; }
        float getParticleLifetime( void ) const { return _particleLifetime; }

        void setParticleSpeed( float value ) { _particleSpeed = value; }
        float getParticleSpeed( void ) const { return _particleSpeed; }

        void setParticleStartSize( float value ) { _particleStartSize = value; }
        float getParticleStartSize( void ) const { return _particleStartSize; }

        void setParticleEndSize( float value ) { _particleEndSize = value; }
        float getParticleEndSize( void ) const { return _particleEndSize; }

        void setParticleSize( float value )
        {
            setParticleStartSize( value );
            setParticleEndSize( value );
        }

        void setParticleStartColor( const RGBAColorf &color ) { _particleStartColor = color; }
        const RGBAColorf &getParticleStartColor( void ) const { return _particleStartColor; }

        void setParticleEndColor( const RGBAColorf &color ) { _particleEndColor = color; }
        const RGBAColorf &getParticleEndColor( void ) const { return _particleEndColor; }

        void setParticleColor( const RGBAColorf &color )
        {
            setParticleStartColor( color );
            setParticleEndColor( color );
        }

    private:
        float _particleLifetime = 1.0f;
        float _particleSpeed = 1.0f;
        float _particleStartSize = 1.0f;
        float _particleEndSize = 1.0f;
        RGBAColorf _particleStartColor;
        RGBAColorf _particleEndColor;

    public:
        void setTexture( SharedPointer< Texture > const &texture ) { _texture = texture; }
        Texture *getTexture( void ) { return crimild::get_ptr( _texture ); }

    private:
        SharedPointer< Texture > _texture;

    public:
        void setEmitter( SharedPointer< ParticleEmitter > const &emitter ) { _emitter = emitter; }
        ParticleEmitter *getEmitter( void ) { return crimild::get_ptr( _emitter ); }

    private:
        SharedPointer< ParticleEmitter > _emitter;

    public:
        void generate( void );

    private:
        void resetParticle( Particle &p );
        void updateParticles( const Clock & );        

    private:
        SharedPointer< Material > _material;
        SharedPointer< Primitive > _primitive;
        SharedPointer< ShaderProgram > _program;
    };

}

#endif

