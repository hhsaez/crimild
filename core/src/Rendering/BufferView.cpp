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
*     * Neither the name of the copyright holder nor the
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

#include "Rendering/BufferView.hpp"

#include "Coding/Decoder.hpp"
#include "Coding/Encoder.hpp"

using namespace crimild;

void BufferView::encode( coding::Encoder &encoder )
{
    Codable::encode( encoder );

    encoder.encode( "target", Int32( m_target ) );
    encoder.encode( "buffer", m_buffer );
    encoder.encode( "offset", m_offset );
    encoder.encode( "stride", m_stride );
    encoder.encode( "length", m_length );
    encoder.encode( "usage", Int32( m_usage ) );
}

void BufferView::decode( coding::Decoder &decoder )
{
    Codable::decode( decoder );

    Int32 target;
    decoder.decode( "target", target );
    m_target = Target( target );

    decoder.decode( "buffer", m_buffer );
    decoder.decode( "offset", m_offset );
    decoder.decode( "stride", m_stride );
    decoder.decode( "length", m_length );

    Int32 usage;
    decoder.decode( "usage", usage );
    m_usage = Usage( usage );
}
