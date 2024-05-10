crimild_trace()

add_library( crimild_mathematics OBJECT )

target_sources(
    crimild_mathematics

    PUBLIC
    Crimild_Mathematics.hpp

    PRIVATE
    Bounds3.hpp
    Box.hpp
    ColorRGB.hpp
    ColorRGBA.hpp
    Cylinder.hpp
    Cylinder_normal.hpp
    Frustum.hpp
    Interpolation.hpp
    Intersection.hpp
    LineSegment3.hpp
    Matrix3.hpp
    Matrix3_constants.hpp
    Matrix3_determinant.hpp
    Matrix3_equality.hpp
    Matrix3_inverse.hpp
    Matrix3_operators.hpp
    Matrix3_transpose.hpp
    Matrix4.hpp
    Matrix4_determinant.hpp
    Matrix4_operators.hpp
    Matrix4_transpose.hpp
    Quaternion.hpp
    Random.hpp
    Ray3.hpp
    Rect.hpp
    Series.hpp
    Size2.hpp
    Size3.hpp
    Sphere.hpp
    abs.hpp
    bisect.hpp
    ceil.hpp
    centroid.hpp
    clamp.hpp
    combine.hpp
    corner.hpp
    cross.hpp
    diagonal.hpp
    distance.hpp
    dot.hpp
    easing.hpp
    expand.hpp
    factorial.hpp
    floor.hpp
    get_ptr.hpp
    inside.hpp
    intersect.hpp
    io.hpp
    isEqual.hpp
    isInfinity.hpp
    isNaN.hpp
    isZero.hpp
    length.hpp
    max.hpp
    min.hpp
    Normal3.hpp
    normal.hpp
    normalize.hpp
    Numbers.hpp
    Numeric.hpp
    origin.hpp
    ortho.hpp
    orthonormalization.hpp
    overlaps.hpp
    permutation.hpp
    perspective.hpp
    Plane3.hpp
    Point2.hpp
    Point3.hpp
    pow.hpp
    project.hpp
    radius.hpp
    reflect.hpp
    refract.hpp
    Root.hpp
    round.hpp
    sign.hpp
    sqrt.hpp
    surfaceArea.hpp
    swizzle.hpp
    Transformation.hpp
    Transformation_apply.hpp
    Transformation_constants.hpp
    Transformation_easing.hpp
    Transformation_euler.hpp
    Transformation_inverse.hpp
    Transformation_isIdentity.hpp
    Transformation_lookAt.hpp
    Transformation_operators.hpp
    Transformation_rotation.hpp
    Transformation_scale.hpp
    Transformation_translation.hpp
    Triangle.hpp
    Triangle_edges.hpp
    Triangle_normal.hpp
    trigonometry.hpp
    Tuple2.hpp
    Tuple3.hpp
    Tuple4.hpp
    Vector2.hpp
    Vector3.hpp
    Vector4.hpp
    volume.hpp
    whichSide.hpp

    PRIVATE
    Crimild_Mathematics.cpp

    PRIVATE
    Random.cpp
)

target_include_directories( crimild_mathematics PUBLIC . )

target_compile_features( crimild_mathematics PUBLIC cxx_std_20 )
add_library( Crimild::Mathematics ALIAS crimild_mathematics )
