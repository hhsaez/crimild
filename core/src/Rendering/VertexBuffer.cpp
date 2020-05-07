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

#include "Rendering/VertexBuffer.hpp"

namespace crimild {

    template <> const char *VertexP2Buffer::getClassName( void ) const { return "crimild::VertexP2Buffer"; }
    template <> const char *VertexP2C3Buffer::getClassName( void ) const { return "crimild::VertexP2C3Buffer"; }
    template <> const char *VertexP2C3TC2Buffer::getClassName( void ) const { return "crimild::VertexP2C3TC2Buffer"; }
    template <> const char *VertexP2TC2C4Buffer::getClassName( void ) const { return "crimild::VertexP2TC2C4Buffer"; }
    template <> const char *VertexP3Buffer::getClassName( void ) const { return "crimild::VertexP3Buffer"; }
    template <> const char *VertexP3C3Buffer::getClassName( void ) const { return "crimild::VertexP3C3Buffer"; }
    template <> const char *VertexP3N3TC2Buffer::getClassName( void ) const { return "crimild::VertexP3N3TC2Buffer"; }
    template <> const char *VertexP3C3TC2Buffer::getClassName( void ) const { return "crimild::VertexP3C3TC2Buffer"; }

}

