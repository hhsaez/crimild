/*
 * Copyright (c) 2013-2018, H. Hernan Saez
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

#ifndef CRIMILD_RENDERING_FRAME_BUFFER_OBJECT_
#define CRIMILD_RENDERING_FRAME_BUFFER_OBJECT_

#include "Catalog.hpp"
#include "Texture.hpp"

#include "Mathematics/Vector.hpp"
#include "Foundation/Containers/Map.hpp"

#include <memory>

namespace crimild {

	class RenderTarget;
    
	class FrameBufferObject : public SharedObject, public Catalog< FrameBufferObject >::Resource {
	private:
		using RenderTargetMap = Map< std::string, SharedPointer< RenderTarget >>;

	public:
		class ClearFlag {
		public:
			enum {
				NONE = 0,

				COLOR = 1 << 0,
				DEPTH = 1 << 1,

				ALL = COLOR | DEPTH,
			};
		};

	public:
        FrameBufferObject( int width, int height );
		virtual ~FrameBufferObject( void );

		int getWidth( void ) const { return _width; }
		int getHeight( void ) const { return _height; }

        void resize( int width, int height );

        float getAspectRatio( void ) const { return ( float ) _width / ( float ) _height; }

		void setClearColor( const RGBAColorf &color ) { _clearColor = color; }
		const RGBAColorf &getClearColor( void ) const { return _clearColor; }

        RenderTargetMap &getRenderTargets( void ) { return _renderTargets; }

		void setClearFlags( crimild::Int8 clearFlags ) { _clearFlags = clearFlags; }
        crimild::Int8 getClearFlags( void ) const { return _clearFlags; }

	private:
		int _width;
		int _height;
		RGBAColorf _clearColor;
        RenderTargetMap _renderTargets;
		crimild::Int8 _clearFlags = ClearFlag::ALL;
	};

	/**
	   \brief A simple FBO with color and depth components
	 */
	class StandardFrameBufferObject : public FrameBufferObject {
	public:
		StandardFrameBufferObject( int width, int height );
		virtual ~StandardFrameBufferObject( void );
	};
		

}

#endif

