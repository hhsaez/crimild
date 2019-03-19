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

#ifndef CRIMILD_RENDERING_IMAGE_TGA_
#define CRIMILD_RENDERING_IMAGE_TGA_

#include "Image.hpp"

#include <string>

namespace crimild {

	class ImageTGA : public Image {
	public:
		ImageTGA( void );
		explicit ImageTGA( std::string filePath );
		virtual ~ImageTGA( void );

		virtual void load( void ) override;

		void saveToFile( const std::string &fileName ) const;

	private:
		struct TGAHeader {
            crimild::UInt8 idLength;
            crimild::UInt8 colorMapType;
            crimild::UInt8 imageType;
		    crimild::UInt16 colorMapStart;
		    crimild::UInt16 colorMapLength;
		    crimild::UInt8 colorMapBits;
		    crimild::UInt16 xStart;
		    crimild::UInt16 yStart;
		    crimild::UInt16 width;
		    crimild::UInt16 height;
		    crimild::UInt8 bits;
		    crimild::UInt8 descriptor;
		};

		std::string _filePath;

		/**
            \name Coding support
         */
        //@{
        
    public:
        virtual void encode( coding::Encoder &encoder ) override;
        virtual void decode( coding::Decoder &decoder ) override;
        
        //@}        
	};
    
    using ImageTGAPtr = SharedPointer< ImageTGA >;

}

#endif

