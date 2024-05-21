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
    Concepts.hpp
    Cylinder.hpp
    Frustum.hpp
    Interpolation.hpp
    Intersection.hpp
    LineSegment3.hpp
    Matrix3.hpp
    Matrix4.hpp
    Normal3.hpp
    Numbers.hpp
    Numeric.hpp
    Plane3.hpp
    Point2.hpp
    Point3.hpp
    Quaternion.hpp
    Random.hpp
    Ray3.hpp
    Rect.hpp
    Root.hpp
    Series.hpp
    Size2.hpp
    Size3.hpp
    Sphere.hpp
    Transformation.hpp
    Triangle.hpp
    Tuple2.hpp
    Tuple3.hpp
    Tuple4.hpp
    Vector2.hpp
    Vector3.hpp
    Vector4.hpp

    abs.hpp
    bisect.hpp
    ceil.hpp
    centroid.hpp
    clamp.hpp
    combine.hpp
    conjugate.hpp
    corner.hpp
    cross.hpp
    determinant.hpp
    diagonal.hpp
    distance.hpp
    dot.hpp
    easing.hpp
    edges.hpp
    euler.hpp
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
    lookAt.hpp
    max.hpp
    min.hpp
    normal.hpp
    normalize.hpp
    origin.hpp
    ortho.hpp
    orthonormalization.hpp
    overlaps.hpp
    permutation.hpp
    perspective.hpp
    pow.hpp
    project.hpp
    radius.hpp
    reflect.hpp
    refract.hpp
    rotation.hpp
    round.hpp
    scale.hpp
    sign.hpp
    size.hpp
    sqrt.hpp
    surfaceArea.hpp
    swizzle.hpp
    translation.hpp
    transpose.hpp
    trigonometry.hpp
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
