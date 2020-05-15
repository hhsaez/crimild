/*
 * Copyright (c) 2002 - present, H. Hernan Saez
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the copyright holders nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDER BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef CRIMILD_CORE_RENDERING_FRAME_GRAPH_OBJECT_IMPL_
#define CRIMILD_CORE_RENDERING_FRAME_GRAPH_OBJECT_IMPL_

#include "Rendering/FrameGraphObject.hpp"
#include "Rendering/FrameGraph.hpp"

namespace crimild {

	/**
	   \brief Implementes automatic registration for objects 

	   Classes should inherit from this one in order to support
	   automatic registration of instances into a frame graph. 

	   \remarks Right now there can be only one frame graph instance since
	   we're using singletons, but that might change in the future.
	 */
	template< typename T >
	class FrameGraphObjectImpl : public FrameGraphObject {
	protected:
		/**
		   \brief Default constructor

		   New objects will automatically register themselves to a frame
		   graph, if one is available.
		 */
		FrameGraphObjectImpl( void ) noexcept
		{
			if ( auto frameGraph = FrameGraph::getInstance() ) {
				frameGraph->add( static_cast< T * >( this ) );
			}
		}

		/**
		   \brief Destructor

		   Remove the object from the frame graph is done by the base
		   class' destructor.
		 */
		virtual ~FrameGraphObjectImpl( void ) = default;
	};

}

#endif

