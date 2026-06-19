crimild_trace()

add_library( crimild_math STATIC )

target_sources(
  crimild_math

  INTERFACE include/crimild/math.hpp

  PUBLIC include/crimild/math/Bounds3.hpp
  PUBLIC include/crimild/math/Box.hpp
  PUBLIC include/crimild/math/ColorRGB.hpp
  PUBLIC include/crimild/math/ColorRGBA.hpp
  PUBLIC include/crimild/math/Cylinder.hpp
  PUBLIC include/crimild/math/Frustum.hpp
  PUBLIC include/crimild/math/Interpolation.hpp
  PUBLIC include/crimild/math/Intersection.hpp
  PUBLIC include/crimild/math/LineSegment3.hpp
  PUBLIC include/crimild/math/Matrix3.hpp
  PUBLIC include/crimild/math/Matrix4.hpp
  PUBLIC include/crimild/math/Normal3.hpp
  PUBLIC include/crimild/math/Numeric.hpp
  PUBLIC include/crimild/math/Plane3.hpp
  PUBLIC include/crimild/math/Point2.hpp
  PUBLIC include/crimild/math/Point3.hpp
  PUBLIC include/crimild/math/Quaternion.hpp
  PUBLIC include/crimild/math/Random.hpp
  PUBLIC include/crimild/math/Ray3.hpp
  PUBLIC include/crimild/math/Rect.hpp
  PUBLIC include/crimild/math/Root.hpp
  PUBLIC include/crimild/math/Size2.hpp
  PUBLIC include/crimild/math/Size3.hpp
  PUBLIC include/crimild/math/Sphere.hpp
  PUBLIC include/crimild/math/Transformation.hpp
  PUBLIC include/crimild/math/Triangle.hpp
  PUBLIC include/crimild/math/Tuple2.hpp
  PUBLIC include/crimild/math/Tuple3.hpp
  PUBLIC include/crimild/math/Tuple4.hpp
  PUBLIC include/crimild/math/Vector2.hpp
  PUBLIC include/crimild/math/Vector3.hpp
  PUBLIC include/crimild/math/Vector4.hpp

  PUBLIC include/crimild/math/abs.hpp
  PUBLIC include/crimild/math/bisect.hpp
  PUBLIC include/crimild/math/ceil.hpp
  PUBLIC include/crimild/math/centroid.hpp
  PUBLIC include/crimild/math/clamp.hpp
  PUBLIC include/crimild/math/combine.hpp
  PUBLIC include/crimild/math/concepts.hpp
  PUBLIC include/crimild/math/conjugate.hpp
  PUBLIC include/crimild/math/corner.hpp
  PUBLIC include/crimild/math/cross.hpp
  PUBLIC include/crimild/math/determinant.hpp
  PUBLIC include/crimild/math/diagonal.hpp
  PUBLIC include/crimild/math/distance.hpp
  PUBLIC include/crimild/math/dot.hpp
  PUBLIC include/crimild/math/easing.hpp
  PUBLIC include/crimild/math/edges.hpp
  PUBLIC include/crimild/math/euler.hpp
  PUBLIC include/crimild/math/expand.hpp
  PUBLIC include/crimild/math/factorial.hpp
  PUBLIC include/crimild/math/floor.hpp
  PUBLIC include/crimild/math/get_ptr.hpp
  PUBLIC include/crimild/math/inside.hpp
  PUBLIC include/crimild/math/intersect.hpp
  PUBLIC include/crimild/math/io.hpp
  PUBLIC include/crimild/math/isEqual.hpp
  PUBLIC include/crimild/math/isInfinity.hpp
  PUBLIC include/crimild/math/isNaN.hpp
  PUBLIC include/crimild/math/isZero.hpp
  PUBLIC include/crimild/math/length.hpp
  PUBLIC include/crimild/math/lookAt.hpp
  PUBLIC include/crimild/math/max.hpp
  PUBLIC include/crimild/math/min.hpp
  PUBLIC include/crimild/math/normal.hpp
  PUBLIC include/crimild/math/normalize.hpp
  PUBLIC include/crimild/math/numbers.hpp
  PUBLIC include/crimild/math/origin.hpp
  PUBLIC include/crimild/math/ortho.hpp
  PUBLIC include/crimild/math/orthonormalization.hpp
  PUBLIC include/crimild/math/overlaps.hpp
  PUBLIC include/crimild/math/permutation.hpp
  PUBLIC include/crimild/math/perspective.hpp
  PUBLIC include/crimild/math/pow.hpp
  PUBLIC include/crimild/math/project.hpp
  PUBLIC include/crimild/math/radius.hpp
  PUBLIC include/crimild/math/reflect.hpp
  PUBLIC include/crimild/math/refract.hpp
  PUBLIC include/crimild/math/rotation.hpp
  PUBLIC include/crimild/math/round.hpp
  PUBLIC include/crimild/math/scale.hpp
  PUBLIC include/crimild/math/series.hpp
  PUBLIC include/crimild/math/sign.hpp
  PUBLIC include/crimild/math/size.hpp
  PUBLIC include/crimild/math/sqrt.hpp
  PUBLIC include/crimild/math/surfaceArea.hpp
  PUBLIC include/crimild/math/swizzle.hpp
  PUBLIC include/crimild/math/translation.hpp
  PUBLIC include/crimild/math/transpose.hpp
  PUBLIC include/crimild/math/trigonometry.hpp
  PUBLIC include/crimild/math/volume.hpp
  PUBLIC include/crimild/math/whichSide.hpp

  PRIVATE src/Random.cpp
)

target_include_directories(
  crimild_math
  PUBLIC include
  PRIVATE src
)

#target_link_libraries(
 #   crimild_math
  #  PUBLIC crimild::foundation
#)

target_compile_features( crimild_math PUBLIC cxx_std_20 )

add_library( crimild::math ALIAS crimild_math )

if ( CRIMILD_BUILD_TESTS )
	add_subdirectory( test )
endif ()
