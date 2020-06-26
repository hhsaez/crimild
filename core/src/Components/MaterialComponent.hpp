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

#ifndef CRIMILD_COMPONENTS_MATERIAL_
#define CRIMILD_COMPONENTS_MATERIAL_

#include "NodeComponent.hpp"

#include "Rendering/Material.hpp"
#include "Foundation/Containers/Array.hpp"

#include <functional>

namespace crimild {

	class MaterialComponent : public NodeComponent {
		CRIMILD_IMPLEMENT_RTTI( crimild::MaterialComponent )

	public:
		MaterialComponent( void );
		virtual ~MaterialComponent( void );

		bool hasMaterials( void ) const { return _materials.size() > 0; }
        
        void attachMaterial( Material *material ) { attachMaterial( crimild::retain( material ) ); }
		void attachMaterial( SharedPointer< Material > const &material );
        
		void detachAllMaterials( void );

		Material *first( void ) { return crimild::get_ptr( _materials[ 0 ] ); }
        
		void forEachMaterial( std::function< void( Material * ) > callback );

	private:
		Array< SharedPointer< Material >> _materials;
        
        /**
            \name Clonning
         */
        //@{
    public:
        virtual SharedPointer< NodeComponent > clone( void ) override;
        
        //@}

        /**
            \name Coding support
         */
        //@{
        
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;
        
        //@}
        
		/**
			\name Streaming support
			\deprecated see crimild::Coding
		*/
		//@{

	public:
		bool registerInStream( Stream &s ) override;
		void save( Stream &s ) override;
		void load( Stream &s ) override;

		//@}
	};

}

#endif

