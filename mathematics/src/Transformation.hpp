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

#ifndef CRIMILD_MATHEMATICS_TRANSFORMATION_
#define CRIMILD_MATHEMATICS_TRANSFORMATION_

#include "Matrix4.hpp"
#include "Normal3.hpp"
#include "Point3.hpp"
#include "Ray3.hpp"
#include "Vector3.hpp"

namespace crimild {

    /**
     * @brief A function from points to points and vectors to vectors
     *
     * There are many ways to represent transformations.
     * In the end, I chose the SRT (actually SQT) one because
     * it's more suited for interpolations and compositions.
     */
    struct Transformation {
        struct Constants;

        class Contents {
        public:
            enum {
                IDENTITY = 0,

                TRANSLATION = 1 << 0,
                ROTATION = 1 << 1,
                SCALING = 1 << 2,

                USER_DEFINED = 1 << 9,
            };
        };

        Matrix4 mat;
        Matrix4 invMat;

        /**
           \brief The contents of the transformation matrices

           This is used as an optimization, in order to avoid doing too many computations
           for basic matrices (i.e., if the transformation is a translation, there's no
           need to compute the matrix multiplication. We can just extract the translation
           vector and add it to the input.

           \remarks It's set as Contents::USER_DEFINED by default, so you need to be careful
           when creating a custom transformation.
         */
        uint32_t contents = Contents::USER_DEFINED;

        [[nodiscard]] inline constexpr bool operator==( const Transformation &other ) const noexcept
        {
            return contents == other.contents && mat == other.mat && invMat == other.invMat;
        }

        [[nodiscard]] inline constexpr bool operator!=( const Transformation &other ) const noexcept
        {
            return !( *this == other );
        }

        [[nodiscard]] constexpr Point3f operator()( const Point3f &p ) const noexcept;
        [[nodiscard]] constexpr Vector3 operator()( const Vector3 &v ) const noexcept;
        [[nodiscard]] constexpr Normal3 operator()( const Normal3 &N ) const noexcept;
        [[nodiscard]] constexpr Ray3 operator()( const Ray3 &R ) const noexcept;
    };

}

#endif
