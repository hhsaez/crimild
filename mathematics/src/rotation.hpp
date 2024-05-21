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

#ifndef CRIMILD_MATHEMATICS_ROTATION_
#define CRIMILD_MATHEMATICS_ROTATION_

#include "Transformation.hpp"
#include "dot.hpp"
#include "euler.hpp"

namespace crimild {

    /**
     * \brief Creates a rotation transformation from an arbitrary axis and angle
     *
     * \warning It is assumed the input axis is already normalized
     */
    [[nodiscard]] static auto rotation( const Vector3 &axis, radians_t angle ) noexcept
    {
        return Transformation {
            .rotate = Quaternion {
                axis * real_t( std::sin( 0.5 * angle ) ),
                real_t( std::cos( 0.5 * angle ) ),
            },
        };
    }

    [[nodiscard, maybe_unused]] static constexpr auto rotationX( radians_t angle ) noexcept
    {
        return rotation( Vector3::Constants::UNIT_X, angle );
    }

    // TODO: make this function constexpr
    [[nodiscard, maybe_unused]] static constexpr auto rotationY( radians_t angle ) noexcept
    {
        return rotation( Vector3::Constants::UNIT_Y, angle );
    }

    [[nodiscard, maybe_unused]] static constexpr auto rotationZ( radians_t angle ) noexcept
    {
        return rotation( Vector3::Constants::UNIT_Z, angle );
    }

}

#endif
