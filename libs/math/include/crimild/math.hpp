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

#ifndef CRIMILD_MATH_
#define CRIMILD_MATH_

#include "crimild/math/Bounds3.hpp"
#include "crimild/math/Box.hpp"
#include "crimild/math/ColorRGB.hpp"
#include "crimild/math/ColorRGBA.hpp"
#include "crimild/math/Cylinder.hpp"
#include "crimild/math/Frustum.hpp"
#include "crimild/math/Interpolation.hpp"
#include "crimild/math/Intersection.hpp"
#include "crimild/math/LineSegment3.hpp"
#include "crimild/math/Matrix3.hpp"
#include "crimild/math/Matrix4.hpp"
#include "crimild/math/Normal3.hpp"
#include "crimild/math/Numeric.hpp"
#include "crimild/math/Plane3.hpp"
#include "crimild/math/Point2.hpp"
#include "crimild/math/Point3.hpp"
#include "crimild/math/Quaternion.hpp"
#include "crimild/math/Random.hpp"
#include "crimild/math/Ray3.hpp"
#include "crimild/math/Rect.hpp"
#include "crimild/math/Root.hpp"
#include "crimild/math/Size2.hpp"
#include "crimild/math/Size3.hpp"
#include "crimild/math/Sphere.hpp"
#include "crimild/math/Transformation.hpp"
#include "crimild/math/Triangle.hpp"
#include "crimild/math/Tuple2.hpp"
#include "crimild/math/Tuple3.hpp"
#include "crimild/math/Tuple4.hpp"
#include "crimild/math/Vector2.hpp"
#include "crimild/math/Vector3.hpp"
#include "crimild/math/Vector4.hpp"
#include "crimild/math/abs.hpp"
#include "crimild/math/bisect.hpp"
#include "crimild/math/ceil.hpp"
#include "crimild/math/centroid.hpp"
#include "crimild/math/clamp.hpp"
#include "crimild/math/combine.hpp"
#include "crimild/math/concepts.hpp"
#include "crimild/math/conjugate.hpp"
#include "crimild/math/corner.hpp"
#include "crimild/math/cross.hpp"
#include "crimild/math/decompose.hpp"
#include "crimild/math/determinant.hpp"
#include "crimild/math/diagonal.hpp"
#include "crimild/math/distance.hpp"
#include "crimild/math/dot.hpp"
#include "crimild/math/easing.hpp"
#include "crimild/math/edges.hpp"
#include "crimild/math/euler.hpp"
#include "crimild/math/expand.hpp"
#include "crimild/math/factorial.hpp"
#include "crimild/math/floor.hpp"
#include "crimild/math/get_ptr.hpp"
#include "crimild/math/height.hpp"
#include "crimild/math/inside.hpp"
#include "crimild/math/intersect.hpp"
#include "crimild/math/inverse.hpp"
#include "crimild/math/io.hpp"
#include "crimild/math/isEqual.hpp"
#include "crimild/math/isInfinity.hpp"
#include "crimild/math/isNaN.hpp"
#include "crimild/math/isZero.hpp"
#include "crimild/math/length.hpp"
#include "crimild/math/lookAt.hpp"
#include "crimild/math/max.hpp"
#include "crimild/math/min.hpp"
#include "crimild/math/normal.hpp"
#include "crimild/math/normalize.hpp"
#include "crimild/math/numbers.hpp"
#include "crimild/math/origin.hpp"
#include "crimild/math/ortho.hpp"
#include "crimild/math/orthonormalization.hpp"
#include "crimild/math/overlaps.hpp"
#include "crimild/math/permutation.hpp"
#include "crimild/math/perspective.hpp"
#include "crimild/math/pow.hpp"
#include "crimild/math/project.hpp"
#include "crimild/math/radius.hpp"
#include "crimild/math/reflect.hpp"
#include "crimild/math/refract.hpp"
#include "crimild/math/rotation.hpp"
#include "crimild/math/round.hpp"
#include "crimild/math/scale.hpp"
#include "crimild/math/series.hpp"
#include "crimild/math/sign.hpp"
#include "crimild/math/size.hpp"
#include "crimild/math/sqrt.hpp"
#include "crimild/math/surfaceArea.hpp"
#include "crimild/math/swizzle.hpp"
#include "crimild/math/trace.hpp"
#include "crimild/math/translation.hpp"
#include "crimild/math/transpose.hpp"
#include "crimild/math/trigonometry.hpp"
#include "crimild/math/types.hpp"
#include "crimild/math/volume.hpp"
#include "crimild/math/whichSide.hpp"

#endif
