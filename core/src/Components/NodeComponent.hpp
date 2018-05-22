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

#ifndef CRIMILD_COMPONENTS_NODE_COMPONENT_
#define CRIMILD_COMPONENTS_NODE_COMPONENT_

#include "Coding/Codable.hpp"
#include "Streaming/Stream.hpp"
#include "Mathematics/Clock.hpp"

namespace crimild {

	class Node;
	class Renderer;
	class Camera;

    class NodeComponent : public coding::Codable, public StreamObject {
    	CRIMILD_IMPLEMENT_RTTI( crimild::NodeComponent )
        
	protected:
		NodeComponent( void );

	public:
		virtual ~NodeComponent( void );

    public:
        // internal use only
        void setNode( Node *node ) { _node = node; }
		
        // internal use only
        Node *getNodePointer( void ) { return _node; }
        
    public:
        virtual const char *getComponentName( void ) const { return getClassName(); }
        
        Node *getNode( void ) { return _node; }
        
        const Node *getNode( void ) const { return _node; }
        
		template< class NodeClass >
        NodeClass *getNode( void ) { return static_cast< NodeClass * >( getNode() ); }

		bool isEnabled( void ) const { return _enabled; }
		void setEnabled( bool value ) { _enabled = value; }

	private:
        Node *_node = nullptr;
        bool _enabled = true;
        
    public:
        NodeComponent *getComponentWithName( std::string name );
        
        template< class NODE_COMPONENT_CLASS >
        NODE_COMPONENT_CLASS *getComponent( void )
        {
            return static_cast< NODE_COMPONENT_CLASS * >( getComponentWithName( NODE_COMPONENT_CLASS::__CLASS_NAME ) );
        }

	public:
		/**
		   \brief Invoked once when component is attached to a node
		*/
		virtual void onAttach( void );

		/**
		   \brief Invoked once when scene is loaded
		*/
		virtual void start( void );

		/**
			\brief Invoked once per scene update
		*/
		virtual void update( const Clock & );

		/**
			\brief Invoked only if debug rendering is enabled
		*/
        virtual void renderDebugInfo( Renderer *renderer, Camera *camera );

		/**
		   \brief Invoked once when component is detached from a node
		*/
		virtual void onDetach( void );
        
        /**
            \name Cloning
         */
        //@{
    public:
        virtual SharedPointer< NodeComponent > clone( void ) { return nullptr; }
        
        //@}
        
        /**
            \name Coding
         */
        //@{
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;
        
        //@}

		/**
			\name Streaming
            \deprecated See crimild::coding
		*/
		//@{

	public:
		virtual bool registerInStream( Stream &s ) override;
		virtual void save( Stream &s ) override;
		virtual void load( Stream &s ) override;	

		//@}
		
	};

}

#endif

