/*
 * Copyright (c) 2002-present, H. Hernán Saez
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

#ifndef CRIMILD_UI_FRAME_CONSTRAINT_MAKER_
#define CRIMILD_UI_FRAME_CONSTRAINT_MAKER_

#include "UIFrameConstraint.hpp"

namespace crimild {

	class Node;

	namespace ui {

		class UIFrame;

		class UIFrameConstraintMaker {
		public:
			UIFrameConstraintMaker( UIFrame *frame );
			virtual ~UIFrameConstraintMaker( void );

			UIFrameConstraintMaker *top( crimild::Real32 offset = 0 );
			UIFrameConstraintMaker *top( SharedPointer< Node > const &other );
			
			UIFrameConstraintMaker *left( crimild::Real32 offset = 0 );
			UIFrameConstraintMaker *left( SharedPointer< Node > const &other );
			
			UIFrameConstraintMaker *right( crimild::Real32 offset = 0 );
			UIFrameConstraintMaker *right( SharedPointer< Node > const &other );

			UIFrameConstraintMaker *bottom( crimild::Real32 offset = 0 );
			UIFrameConstraintMaker *bottom( SharedPointer< Node > const &other );

			UIFrameConstraintMaker *width( crimild::Real32 constant = 0 );
			UIFrameConstraintMaker *height( crimild::Real32 constant = 0 );
			UIFrameConstraintMaker *size( crimild::Real32 width, crimild::Real32 height );

			UIFrameConstraintMaker *center( crimild::Real32 offset = 0 );
			UIFrameConstraintMaker *center( SharedPointer< Node > const &other );
			UIFrameConstraintMaker *centerX( crimild::Real32 offset = 0 );
			UIFrameConstraintMaker *centerX( SharedPointer< Node > const &other );
			UIFrameConstraintMaker *centerY( crimild::Real32 offset = 0 );
			UIFrameConstraintMaker *centerY( SharedPointer< Node > const &other );

			UIFrameConstraintMaker *fillParent( void );
			
			UIFrameConstraintMaker *after( SharedPointer< Node > const &other );
			UIFrameConstraintMaker *before( SharedPointer< Node > const &other );
			UIFrameConstraintMaker *above( SharedPointer< Node > const &other );
			UIFrameConstraintMaker *below( SharedPointer< Node > const &other );

			UIFrameConstraintMaker *margin( crimild::Real32 value );
			UIFrameConstraintMaker *marginLeft( crimild::Real32 value );
			UIFrameConstraintMaker *marginRight( crimild::Real32 value );
			UIFrameConstraintMaker *marginTop( crimild::Real32 value );
			UIFrameConstraintMaker *marginBottom( crimild::Real32 value );
			UIFrameConstraintMaker *margin( crimild::Real32 top, crimild::Real32 right, crimild::Real32 bottom, crimild::Real32 left );

		private:
			UIFrameConstraintMaker *addConstraint( UIFrameConstraint::Type type, crimild::Real32 value );
			UIFrameConstraintMaker *addConstraint( UIFrameConstraint::Type type, SharedPointer< Node > const &other );

		private:
			UIFrame *_frame = nullptr;
		};
		
	}

}

#endif

