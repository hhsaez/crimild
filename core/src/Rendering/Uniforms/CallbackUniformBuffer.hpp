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

#include "Rendering/UniformBuffer.hpp"

namespace crimild {

    /**
       \brief A uniform that can be updated with a callback
    */
    template< typename T >
    class CallbackUniformBuffer : public UniformBuffer {
    private:
        using PreRenderCallback = std::function< T( void ) >;

    public:
        explicit CallbackUniformBuffer( const T &value ) noexcept : UniformBuffer( value ) { }
        explicit CallbackUniformBuffer( PreRenderCallback callback ) noexcept : UniformBuffer( T() ) { onPreRender( callback ); }
        virtual ~CallbackUniformBuffer( void ) = default;

        inline void onPreRender( PreRenderCallback callback ) noexcept { m_callback = callback; }

        virtual void onPreRender( void ) noexcept override
        {
            if ( m_callback != nullptr ) {
                setValue( m_callback() );
            }
        }

    private:
        std::function< T( void ) > m_callback;
    };

}
