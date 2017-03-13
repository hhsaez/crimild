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

#ifndef CRIMILD_PARTICLE_SYSTEM_COMPONENT_
#define CRIMILD_PARTICLE_SYSTEM_COMPONENT_

#include "ParticleData.hpp"

#include "Components/NodeComponent.hpp"

namespace crimild {

    /**
        \remarks Since it's a component, the particle sytem
        can affect the parent node any wait it likes

        For exmaple, it might add a geometry for simple particles

        In addition, if the particles are actual nodes, the "renderer"
        will affect the transform for each node

        MUST BE ATTACHED TO A GROUP NODE (or derived class)
    */
    class ParticleSystemComponent : public NodeComponent {
        CRIMILD_IMPLEMENT_RTTI( crimild::ParticleSystemComponent )

    public:
		/**
		   \brief Deafult constructor
		 */
		ParticleSystemComponent( void );
        explicit ParticleSystemComponent( ParticleDataPtr const &particles );
		explicit ParticleSystemComponent( crimild::Size maxParticles );
        virtual ~ParticleSystemComponent( void );

        inline ParticleData *getParticles( void ) { return crimild::get_ptr( _particles ); }

		virtual void start( void ) override;
		virtual void update( const Clock & ) override;

    private:
        SharedPointer< ParticleData > _particles;

		/**
		   \name Pre-Warming
		*/
		//@{

	public:
		inline void setPreWarmTime( crimild::Real64 time ) { _preWarmTime = time; }
		inline crimild::Real64 getPreWarmTime( void ) const { return _preWarmTime; }

	private:
		/**
		   \brief Indicates if particles should be pre-computed

		   If greater than 0 (zero), this variable tells how many seconds
		   should be pre-simulated when the particle system is being
		   generated
		 */
		crimild::Real64 _preWarmTime = 0;

		//@}

		/**
		   \name Generators
		*/
		//@{
		
	public:
		/**
		   \brief A generator for particle attributes
		 */
        class ParticleGenerator : public SharedObject {
        public:
            virtual ~ParticleGenerator( void ) { }

			/**
			   \brief Configures the particle generator

			   This method is invoked in ParticleEmitterComponent::start()
			 */
			virtual void configure( Node *node, ParticleData *particles ) = 0;

			/**
			   \brief Generates data for one or more attributes
			 */
            virtual void generate( Node *node, crimild::Real64 dt, ParticleData *particles, ParticleId startId, ParticleId endId ) = 0;
        };

        using ParticleGeneratorPtr =  SharedPointer< ParticleGenerator >;


		inline void setEmitRate( crimild::Size value ) { _emitRate = value; }
		inline crimild::Size getEmitRate( void ) const { return _emitRate; }

		inline void addGenerator( ParticleGeneratorPtr const &gen )
		{
			_generators.add( gen );
		}

		inline void setBurst( bool value ) { _burst = value; }
		inline crimild::Bool isBurst( void ) const { return _burst; }

	private:
		void configureGenerators( Node *node, ParticleData *particles );
		void updateGenerators( Node *node, crimild::Real64 dt, ParticleData *particles );

    private:
        crimild::Size _emitRate;
        ThreadSafeArray< ParticleGeneratorPtr > _generators;
		crimild::Bool _burst = false;
		
		//@}

		/**
		   \name Updaters
		*/
		//@{
	public:
		/**
		   \brief Updates the particles of a particle system
		*/
        class ParticleUpdater : public SharedObject {
        public:
            virtual ~ParticleUpdater( void ) { }

			virtual void configure( Node *node, ParticleData *particles ) = 0;
            virtual void update( Node *node, crimild::Real64 dt, ParticleData *particles ) = 0;
        };

        using ParticleUpdaterPtr =  SharedPointer< ParticleUpdater >;

        inline void addUpdater( ParticleUpdaterPtr const &updater )
		{
			_updaters.add( updater );
		}

	private:
		void configureUpdaters( Node *node, ParticleData *particles );
		void updateUpdaters( Node *node, crimild::Real64 dt, ParticleData *particles );
		
    private:
        ThreadSafeArray< ParticleUpdaterPtr > _updaters;

		//@}

		/**
		   \name Renderers
		*/
		//@{
	public:
		/**
		   \brief Generate renderables for a particle system
		*/
        class ParticleRenderer : public SharedObject {
        public:
            virtual ~ParticleRenderer( void ) { }

			virtual void configure( Node *node, ParticleData *particles ) = 0;
            virtual void update( Node *node, crimild::Real64 dt, ParticleData *particles ) = 0;
        };

        using ParticleRendererPtr =  SharedPointer< ParticleRenderer >;

        inline void addRenderer( ParticleRendererPtr const &renderer )
		{
			_renderers.add( renderer );
		}

	private:
		void configureRenderers( Node *node, ParticleData *particles );
		void updateRenderers( Node *node, crimild::Real64 dt, ParticleData *particles );
		
    private:
        ThreadSafeArray< ParticleRendererPtr > _renderers;

		//@}
    };

}

#endif

