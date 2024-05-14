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

#include "Point3.hpp"
#include "Transformation.hpp"
#include "cross.hpp"
#include "dot.hpp"
#include "inverse.hpp"
#include "normalize.hpp"
#include "orthonormalization.hpp"
#include "swizzle.hpp"

namespace crimild {

    /**
       \brief Creates a transformation oriented in the direction of the center and located in the given eye position

       \remarks Crimild uses a right-handed coordinate system, but the forward vector is always pointing
       in the -Z direction.
     */
    [[nodiscard]] static constexpr Transformation lookAt( const Point3 &eye, const Point3 &target, const Vector3 &up ) noexcept
    {
        // Compute the view vector. Keep in mind that Forward always point towards the -Z axis.
        const auto zAxis = normalize( eye - target );

        // Makes sure the up vector is normalized
        const auto xAxis = normalize(
            [ & ] {
                auto ret = cross( normalize( up ), zAxis );
                if ( length2( ret ) > 0 ) {
                    return ret;
                } else {
                    // Pik a different up vector
                    Vector3 right, up;
                    orthonormalBasis( zAxis, right, up );
                    return up;
                }
            }()
        );

        // The new up vector is normalized
        const auto yAxis = normalize( cross( zAxis, xAxis ) );

        // This matrix is in world space. It should only be used by nodes. In order to
        // use it as the view matrix, it will have to be inverted.
        const auto mat = Matrix4 {
            Vector4 { xAxis.x, xAxis.y, xAxis.z, 0 },
            Vector4 { yAxis.x, yAxis.y, yAxis.z, 0 },
            Vector4 { zAxis.x, zAxis.y, zAxis.z, 0 },
            Vector4 { eye.x, eye.y, eye.z, 1 }
        };

        return Transformation {
            mat,
            // TODO: avoid using inverse
            inverse( mat ),
            Transformation::Contents::ROTATION | Transformation::Contents::TRANSLATION,
        };
    }
}

#endif
