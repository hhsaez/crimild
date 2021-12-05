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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_OPERATORS_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_OPERATORS_

#include "Mathematics/Matrix4_operators.hpp"
#include "Mathematics/Transformation.hpp"
#include "Mathematics/Transformation_isIdentity.hpp"

namespace crimild {

    [[nodiscard]] constexpr Transformation operator*( const Transformation &t0, const Transformation &t1 ) noexcept
    {
        if ( isIdentity( t1 ) ) {
            return t0;
        }

        if ( isIdentity( t0 ) ) {
            return t1;
        }

        return crimild::Transformation {
            // Matrices are multiplied as usual...
            .mat = t0.mat * t1.mat,
            // ... but inverses must be multiplied in reverse order since `inv(A * B) = inv(B) * inv(A)`
            .invMat = t1.invMat * t0.invMat,
            .contents = t0.contents | t1.contents,
        };
    }

}

#endif
