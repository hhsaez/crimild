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

#ifndef CRIMILD_MATHEMATICS_
#define CRIMILD_MATHEMATICS_

#include "Bounds3.hpp"
#include "Box.hpp"
#include "ColorRGB.hpp"
#include "ColorRGBA.hpp"
#include "Cylinder.hpp"
#include "Frustum.hpp"
#include "Interpolation.hpp"
#include "Intersection.hpp"
#include "LineSegment3.hpp"
#include "Matrix3.hpp"
#include "Matrix4.hpp"
#include "Normal3.hpp"
#include "Numeric.hpp"
#include "Plane3.hpp"
#include "Point2.hpp"
#include "Point3.hpp"
#include "Quaternion.hpp"
#include "Random.hpp"
#include "Ray3.hpp"
#include "Rect.hpp"
#include "Root.hpp"
#include "Size2.hpp"
#include "Size3.hpp"
#include "Sphere.hpp"
#include "Transformation.hpp"
#include "Triangle.hpp"
#include "Tuple2.hpp"
#include "Tuple3.hpp"
#include "Tuple4.hpp"
#include "Vector2.hpp"
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "abs.hpp"
#include "bisect.hpp"
#include "ceil.hpp"
#include "centroid.hpp"
#include "clamp.hpp"
#include "combine.hpp"
#include "concepts.hpp"
#include "conjugate.hpp"
#include "corner.hpp"
#include "cross.hpp"
#include "decompose.hpp"
#include "determinant.hpp"
#include "diagonal.hpp"
#include "distance.hpp"
#include "dot.hpp"
#include "easing.hpp"
#include "edges.hpp"
#include "euler.hpp"
#include "expand.hpp"
#include "factorial.hpp"
#include "floor.hpp"
#include "get_ptr.hpp"
#include "height.hpp"
#include "inside.hpp"
#include "intersect.hpp"
#include "inverse.hpp"
#include "io.hpp"
#include "isEqual.hpp"
#include "isInfinity.hpp"
#include "isNaN.hpp"
#include "isZero.hpp"
#include "length.hpp"
#include "lookAt.hpp"
#include "max.hpp"
#include "min.hpp"
#include "normal.hpp"
#include "normalize.hpp"
#include "numbers.hpp"
#include "origin.hpp"
#include "ortho.hpp"
#include "orthonormalization.hpp"
#include "overlaps.hpp"
#include "permutation.hpp"
#include "perspective.hpp"
#include "pow.hpp"
#include "project.hpp"
#include "radius.hpp"
#include "reflect.hpp"
#include "refract.hpp"
#include "rotation.hpp"
#include "round.hpp"
#include "scale.hpp"
#include "series.hpp"
#include "sign.hpp"
#include "size.hpp"
#include "sqrt.hpp"
#include "surfaceArea.hpp"
#include "swizzle.hpp"
#include "trace.hpp"
#include "translation.hpp"
#include "transpose.hpp"
#include "trigonometry.hpp"
#include "types.hpp"
#include "volume.hpp"
#include "whichSide.hpp"

#endif
