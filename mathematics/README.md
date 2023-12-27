# Mathematics

## real_t and high-precision computations.
real_t is used for float values

crimild::real_t is used for float values

[real_t](@ref crimild::real_t) is used for float values

[real_t](@ref real_t) is used for float values

## Constants

## Coordinate System
Crimild uses a right handed coordinate system.

### The `FORWARD` vector
The [FORWARD](@ref crimild::Vector3Impl<T>::Constants::FORWARD) vector, declared as a crimild::Vector3 constant, points towards the negative z-coordinate.

The [FORWARD](@ref crimild::Vector3Impl<T>::Constants) vector, declared as a crimild::Vector3 constant, points towards the negative z-coordinate.

The [FORWARD](@ref crimild::Vector3Impl::Constants::FORWARD) vector, declared as a crimild::Vector3 constant, points towards the negative z-coordinate.

## Low-level entities

### Tuples

Tuple template classes, [Tuple2](@ref crimild::Tuple2), [Tuple3](@ref crimild::Tuple3) and [Tuple4](@ref crimild::Tuple4)
provide common utilities for two-, three- and four-dimension entities like points, vectors and normals.

They make use of the "curiously recurring template pattern" (`CRTP`) to know the type of a `Derived` class
(i.e. [Vector2](@ref crimild::Vector2)) and, therefore, the exact type that should be returned from some of
its generic functions (see their [implementations for various operators overloads](@ref crimild::Tuple2::operator+), for
an example). This helps reducing the amount of repeated code in derived classes, while also ensuring those operations
cannot be accidentally mixed. On the contast, if we use Tuple2 (or any other tuple base type) instead of the type of
a derived class, we could end up mixing operations between Vector2 and Point2, which are not supposed to be allowed. For
example, you're not supposed to add a Normal3 to a Point3 or two Point2 togheter.

Also, the use of `CRTP` makes Tuple templates a kind of an abstract classes (since you cannot instantiate them without
specifying a derived type), without having to rely on pure virutal functions (the mathematics library avoids the use
of virtual functions for performance reasons).

#### Constructors
Constructors are made protected to enphasize that tuples are not classes that can be instantiated directly, but
they are intended to be used by derived classes only.

#### About NaN values
None of the constructors check if the values are floating-point "not a number"
values (NaNs), both for performance and logic reasons. This choice allows tuples (and derived classes) to
potentially represent NaN values, which is left to the user to decide if that situation
is valid or not. For example, while in most cases a NaN value could be considered a bug in the system,
there is a valid case where they are used to split line segments in shaders.

#### Public members
Tuple2 stores its coordinates as public variables. One could argue that public
variables are not very "object-oriented", but the truth is that encapsulation
is not really appropriated here and, in fact, ends up adding more code with no
real benefit.

As a side note, the use of curly braces ensures they are default initialized
(numeric types will be initialized to zero by default).

Please not that derived classes might require to bring these variable into scope by doing:
```cpp
template< typename T >
class SomeDerivecClass : public Tuple2< SomeDerivedClass, T > {
public:
    using Tuple2< SomeDerivedClass, T >::x;
    using Tuple2< SomeDerivedClass, T >::y;
};
```

#### Index-based accessors for tuple components
Some rutines do find it more useful to access tuple components inside a loop. The non-const
overload returns a reference, allowing to set the values of each component while indexing.

A check is done to ensure that the provided `index` value is
within the expected range. Otherwise, it will fail. For performance reasons, this check is done
using an `assert` call, which is only evaluated in debug builds.

#### Comparison operators
Tuple class overload the comparison operator `==` in order to check if two tuples are the same.

In order to prevent ambiguities, these overloads use TupleX base class as param. It is still
not possible to compare objects of different Derived types, though, so comparing
Vector2 and Point2 instances is not allowed.

These operator overloads perform strict equality for tuple components. In some scenarios,
you might want to use [isEqual()](@ref crimild::isEqual) instead for comparing floating-point values
with an error margin.

####  Arithmetic operations
Tuple classes implement arithmetic operations that operate on the values of a tuple. While the operations themselves
are trivial and well known, there are several things that we need to consider in regards with their implementation:

For startes, the code for each operation might seem a bit complex, since most of them work with the `Derived`
class (instead of base Tuple classes, which might seem more intuitive).

Additionally, most operations are templated based on another type `U`, supporting operating over
two elements of the same `Derived` template class, but with potentially different storage types for
their components (i.e. an addition between a Vector2<float> and a Vector2<int>).

The return type of each operation is also worth noting. First, they must return `Derived` in most cases, so any
operation involving, for example, `Vector2`, will also return a `Vector2`. This prevents the case of
operations returning the wrong implementation (if we use `Tuple2` instead, it might be _legal_ to
return a different type, like `Point2`, instead which won't make sense). Also, the storage type
of the returned type is determined based on both input types (`T` and `U`). Then, we use
standard C++ type promotion rules: adding a floating-point value to an integer one
will result in a floating-point value as expected.

Please note that it is still possible for derived classes to implement additional arithmetic operations for special
cases. For example, while `Tuple2` does not supports it, it is possible to add a `Vector2` to a `Point2`
to obtain a new `Point2` (the inverse is not allowed, though). This operation is defined in the `Point2` class.

There are many arithmetic operations, such as:

- Basic oeprator for per-component addition (`+`), substraction (`-`) and negation (`-`), including the _in place_
(i.e `operator+=`) froms of them.
- Component-wise multiplication and division by scalar (including _in place_ variants)
- Component-wise multiplication and division by another tuple of the same type (but potentially using a different
storage type)
- `abs()`, `ceil()`, `floor()` utilities for return the absolute value and rounding for each component of the input tuple
- `min()` and `max()` for returning the minimum/maximum value of a single tuple or the minimum/maximum per component of two tuples
- `minDimension()` and `maxDimension()` for returning the index of the minimum/maximum value in a tuple
- `permute()` for switching values of a tuple
- and many more...

### Vectors
Crimild provides types for 2D, 3D and 4D vectors, based on the corresponding two-, three- and four-dimensional
tuple class. All vector types are parametrized by their underlying storage type, like integer or floating-point types.

Aliases are defined for widely-used vector types, such as `Vector3f` for a three-dimensional vector type of floating point values
or `Vector4i` for a four-dimensional vector type of signed integeres.

Default implementation for floating-point vectors, like `Vector3`, use the `real_t` type alias, meaning we can switching them
between 32- and 64-bits representations if needed depending on how the engine is compiled.

Special constructors for creating vectors from other tuple types are flagged as `explicit` so it is not unintentionally used
by authomatic type conversions.

#### Length
Computing the lenght of a vector might seem trivial, but there is a subtletly that needs to be considered: what should be the
_type_ for the length of a vector? For floating-point vectors, the answer is simple: use the same type as the vector itself.
But what if the vector has integer values? Then, the result of the `length()` function should be a floating-point value instead,
right? In order to solve this situation, the [Length](@ref crimild::Length) type trait is introduced. See the
implementation of [length()](@ref crimild::length) for reference about how this trait is used to return the correct value type.

### Points
A point is a zero-dimensional location in 2D, 3D or 4D space, represented by the [Point2](@ref crimild::Point2Impl),
[Point3](@ref crimild::Point3Impl) and [Point4](@ref crimild::Point4Impl), respectively. Although it has a similar
implementation as vectors, the fact that points represents a position whereas a vector represents a direction leads
to a number of important differences in how they should be treated in different parts of the engine.

Please not that some methods of point classes return or take vectors. This requires some operators to be overloaded
in the respective point classes instead of using the ones provided by their base tuple class.

See [Point2](@ref crimild::Point2), [Point3](@ref crimild::Point3)

### Normals
Normals are vectors that are penpendicular to a surface at a given position. What makes them special is that they are
defined in terms of a specific surface, and they behave differently from vectors in some situations, like when applying
transformations.

See [Normal3](@ref crimild::Normal3)

## High-level entities

### Rays

Rays are semi-infinite lines represented by their origin point and direction vector.

The _parametric form_ of a ray expresses it as a function of a scalar value _t_, such us:

```
r(t) = o + t * d    (0 <= t < Inf)
```

See [Ray3](@ref crimild::Ray3)

### Transformation

## Testing
Most of the tests make use of the default `real_t` precision, so they will still be valid if we start using a different precision in the future.

## Column order for matrices and vectors
First of all, members of both Vector* and Matrix* entities are stored linearly in memory. This helps copying them around, encoding, and sending them to shaders.

A Vector* instance represents a column vector. So, when writing `v = { 1, 2, 3, 4 }`, we need to think about it as:

```
  |1|
  |2|
v=|3|
  |4|

```
