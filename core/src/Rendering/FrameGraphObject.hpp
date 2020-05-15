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

#ifndef CRIMILD_CORE_RENDERING_FRAME_GRAPH_OBJECT_
#define CRIMILD_CORE_RENDERING_FRAME_GRAPH_OBJECT_

namespace crimild {

	class FrameGraph;

	/**
	   \brief An object that can be added to a frame graph

	   This is supposed to be a base class for any object that can 
	   be added to a frame graph. In practice, it would be better
	   to inherit from FrameGraphObjectImpl instead, since this class
	   also automatically adds the object to the frame graph.

	   \see FrameGraphObjectImpl
	 */
	class FrameGraphObject {
	protected:
		/**
		   \brief Class destructor

		   Whenever an instance of this class is destroyed, we automatically
		   remove it from the frame graph.

		   \remarks It's protected so we cannot instantiate this class.
		 */
		virtual ~FrameGraphObject( void ) noexcept;

	public:
		/**
		   \brief Gets a pointer to a frame graph

		   As it is right now, this is a helper method to get 
		   the global frame graph. It might be more helpful later
		   if multiple frame graphs are allowed. 
		 */
		FrameGraph *getFrameGraph( void ) const noexcept;
	};


}

#endif

