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

#ifndef CRIMILD_UI_FRAME_CONSTRAINT_
#define CRIMILD_UI_FRAME_CONSTRAINT_

#include "Coding/Codable.hpp"
#include "Foundation/Types.hpp"

namespace crimild {

	class Node;

	namespace ui {

		class UIFrame;

		class UIFrameConstraint : public coding::Codable {
			CRIMILD_IMPLEMENT_RTTI( crimild::ui::UIFrameConstraint )
			
		public:
			enum class Type {
				WIDTH,
				WIDTH_TO_PARENT,
				HEIGHT,
				HEIGHT_TO_PARENT,
				LEFT,
				RIGHT,
				TOP,
				BOTTOM,
				CENTER,
				CENTER_X,
				CENTER_Y,
				EDGES,
				AFTER,
				BEFORE,
				ABOVE,
				BELOW,
				MARGIN,
				MARGIN_TOP,
				MARGIN_RIGHT,
				MARGIN_BOTTOM,
				MARGIN_LEFT,
				BEHIND,
				INFRONT,
			};
			
		public:
			UIFrameConstraint( void ) = default;
			UIFrameConstraint( Type type, crimild::Real32 value );
			UIFrameConstraint( Type type, UIFrame *referenceFrame );
			~UIFrameConstraint( void ) = default;

			Type getType( void ) const { return _type; }

			void setValue( crimild::Real32 value ) { _value = value; }
			crimild::Real32 getValue( void ) const { return _value; }

		private:
			Type _type;

		public:
			void apply( UIFrame *frame, UIFrame *parentFrame );

		private:
			crimild::Real32 _value = 0;
			UIFrame *_referenceFrame = nullptr;
            std::string _referenceFrameName;

		public:
			void decode( coding::Decoder &decoder ) override;
		};
		
	}

}

#endif

