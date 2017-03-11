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

#ifndef CRIMILD_PARTICLE_SYSTEM_DATA_
#define CRIMILD_PARTICLE_SYSTEM_DATA_

#include "ParticleAttribArray.hpp"

namespace crimild {

	/**
	   \brief A container for particles

	   \remarks The type and number of attributes can be customized by the
	   user. 

	   This class is based on the SoA principle, being more cache-friendly.

	   \warning Assumes that all attribute arrays store active elements at 
	   the beginning of the data array. 
	 */
    class ParticleData : public SharedObject {
    public:
        using ParticleAttribs = ThreadSafeMap< ParticleAttribType, ParticleAttribArrayPtr >;

    public:
		/**
		   \brief Default constructor
		 */
        ParticleData( crimild::Size maxParticles );

		/**
		   \brief Destructor
		 */
        virtual ~ParticleData( void );

		inline crimild::Size getParticleCount( void ) const { return _count; }

		inline crimild::Size getAliveCount( void ) const { return _aliveCount; }

		inline crimild::Bool isAlive( ParticleId pid ) { return _alive[ pid ]; }

		/**
		   \brief Set the array for a particular attribute
		 */
        inline void setAttribs( ParticleAttribType attId, ParticleAttribArrayPtr const &attr )
        {
            _attribs[ attId ] = attr;
        }

		/**
		   \brief Get the data for a given attribute
		 */
        inline ParticleAttribArray *getAttrib( ParticleAttribType attId )
        {
            return crimild::get_ptr( _attribs[ attId ] );
        }

		/**
		   \brief Generate new particles

		   \remarks Invoked by ParticleSystemComponent::onAttach()
		 */
        void generate( void );

		/**
		   \brief Kill a particle
		   
		   This method set the alive flag to false for the given particle
		   and swaps the particle so it is stored in the far end of the
		   data array
		 */
        void kill( ParticleId pid );

		/**
		   \brief Activates a particle
		 */
        void wake( ParticleId pid );

		/**
		   \brief Swaps a particle for another

		   This method is usually called when killing or activating particles
		 */
        void swap( ParticleId a, ParticleId b );

		inline void setComputeInWorldSpace( crimild::Bool value ) { _computeInWorldSpace = value; }
		inline crimild::Bool shouldComputeInWorldSpace( void ) const { return _computeInWorldSpace; }

    private:
        ParticleAttribs _attribs;
		
        crimild::Size _count = 0;
        crimild::Size _aliveCount = 0;
		
		std::vector< crimild::Bool > _alive; // replace this for a custom array

		crimild::Bool _computeInWorldSpace = false;

	public:
		/**
		   \brief Get the raw data for an attribute

		   \remarks This method creates the ParticleAttribArray instance
		   if none is found matching the given attribType
		 */
		template< typename T >
		ParticleAttribArray *createAttribArray( ParticleAttribType attribType )
		{
			auto attribs = getAttrib( attribType );
			if ( attribs == nullptr ) {
				setAttribs( attribType, crimild::alloc< ParticleAttribArrayImpl< T >>() );
				attribs = getAttrib( attribType );
			}

			assert( attribs != nullptr );
			return attribs;
		}
    };

	using ParticleDataPtr = SharedPointer< ParticleData >;

}

#endif

