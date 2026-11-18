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

#ifndef CRIMILD_MATHEMATICS_LOOK_AT_
#define CRIMILD_MATHEMATICS_LOOK_AT_

#include "Transformation.hpp"
#include "cross.hpp"
#include "io.hpp"
#include "isZero.hpp"
#include "normalize.hpp"

namespace crimild {

    /**
     * \brief Creates a transformation positioned at the given eye point and oriented in the direction of the target
     *
     * \remarks Crimild uses a right-handed coordinate system, but the forward vector is always pointing
     * in the -Z direction.
     *
     * \todo Make this function `constexpr`
     */
    [[nodiscard]] static auto lookAt( const Point3 &eye, const Point3 &target, const Vector3 &up ) noexcept
    {
        const auto direction = normalize( target - eye );

        const auto axis = [ & ] {
            auto ret = cross( Vector3::Constants::FORWARD, direction );
            return normalize( !isZero( ret ) ? ret : up );
        }();

        const auto angle = [ & ] {
            real_t d = dot( Vector3::Constants::FORWARD, direction );
            real_t angle = std::acos( d );
            return angle;
        }();

        /*
                const auto u = [ & ] {
                    auto ret = cross( Vector3::Constants::FORWARD, direction );
                    if ( isZero( ret ) ) {
                        ret = cross( Vector3::Constants::RIGHT, direction );
                    }
                    return normalize( ret );
                }();

                const auto axis = [ & ] {
                    auto v = cross( u, up );
                    if ( isZero( v ) ) {
                        return u;
                    }
                    v = normalize( v );
                    v = cross( v, u );
                                return Vector3 {
                                    -v.y,
                                    v.x,
                                    v.z,
                                };
                }();
        */

        return Transformation {
            .translate = eye,
            .rotate = [ & ] {
                /*
                                auto d = dot( Vector3::Constants::FORWARD, direction );
                                if ( isZero( d ) ) {
                                    d = dot( Vector3::Constants::RIGHT, direction );
                                }
                                real_t angle = std::acos( d );
                */
                return Quaternion {
                    axis * real_t( std::sin( 0.5 * angle ) ),
                    real_t( std::cos( 0.5 * angle ) ),
                };
            }(),
        };
    }

}

#endif
