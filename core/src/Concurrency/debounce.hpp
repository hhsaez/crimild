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

#ifndef CRIMILD_CORE_CONCURRENCY_DEBOUNCE
#define CRIMILD_CORE_CONCURRENCY_DEBOUNCE

#include "Concurrency/Async.hpp"

namespace crimild::concurrency {

    namespace impl {

        struct Debouncer {
            void reset( std::function< void( void ) > caller, uint32_t timeout ) noexcept
            {
                m_caller = caller;
                m_timeout = timeout;
                m_start = std::chrono::duration_cast< std::chrono::milliseconds >(
                    std::chrono::high_resolution_clock::now().time_since_epoch()
                );
                if ( m_needsRestart ) {
                    tick();
                }
            }

            void tick( void )
            {
                m_needsRestart = false;
                sync_frame(
                    [ this ] {
                        const auto now = std::chrono::duration_cast< std::chrono::milliseconds >(
                            std::chrono::high_resolution_clock::now().time_since_epoch()
                        );
                        const auto diff = now - m_start;
                        if ( diff.count() >= m_timeout ) {
                            if ( m_caller != nullptr ) {
                                m_caller();
                            }
                            m_needsRestart = true;
                        } else {
                            tick();
                        }
                    }
                );
            }

        private:
            std::function< void( void ) > m_caller;
            uint32_t m_timeout;
            std::chrono::milliseconds m_start;
            bool m_needsRestart = true;
        };

    }

    auto debounce( auto fn, uint32_t timeout ) noexcept
    {
        // So much black magic here...
        auto ret = [ fn, timeout, debouncer = impl::Debouncer {} ]( auto &&...args ) mutable {
            debouncer.reset(
                [ fn, ... args = std::forward< decltype( args ) >( args ) ] {
                    fn( args... );
                },
                timeout
            );
        };
        return ret;
    }

}

#endif
