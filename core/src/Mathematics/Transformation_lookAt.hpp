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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_LOOK_AT_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_LOOK_AT_

#include "Mathematics/Matrix4_inverse.hpp"
#include "Mathematics/Point3Ops.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/cross.hpp"
#include "Mathematics/dot.hpp"
#include "Mathematics/normalize.hpp"

namespace crimild {

    /**
       \brief Creates a transformation oriented in the direction of the center and located in the given eye position

       \remarks Crimild uses a right-handed coordinate system, but the forward vector is always pointing
       in the -Z direction.
     */
    [[nodiscard]] static constexpr Transformation lookAt( const Point3 &eye, const Point3 &center, const Vector3 &up ) noexcept
    {
        const auto E = Vector3 { eye.x, eye.y, eye.z };
        const auto C = Vector3 { center.x, center.y, center.z };

        // Compute the view vector. Keep in mind that Forward always point towards the -Z axis.
        const auto V = normalize( E - C );

        // Make sure up vector is normalized. Also, negate the cross produce since we want the R vector
        // The result is a normalized vector too.
        const auto R = -( cross( V, normalize( up ) ) );

        // The new up vector is normalized
        const auto U = cross( V, R );

        // This matrix is in camera space, so we need to invert it later
        const auto cameraToWorld = Matrix4 {
            { R.x, U.x, V.x, 0 },
            { R.y, U.y, V.y, 0 },
            { R.z, U.z, V.z, 0 },
            { dot( -E, R ), dot( -E, U ), dot( -E, V ), 1 },
        };

        return Transformation {
            // TODO: avoid using inverse
            inverse( cameraToWorld ),
            cameraToWorld,
            Transformation::Contents::ROTATION | Transformation::Contents::TRANSLATION,
        };
    }
}

#endif
