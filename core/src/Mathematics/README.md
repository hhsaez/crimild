# Notes about Math library in Crimild

## Everything is a POCO

## A more functional approach

## Column order for matrices and vectors
First of all, members of both Vector* and Matrix* entities are stored linearly in memory. This helps copying them around, encoding, and sending them to shaders.

A Vector* instance represents a column vector. So, when writing `v = { 1, 2, 3, 4 }`, we need to think about it as:

```
  |1|
  |2|
v=|3|
  |4|

```

## The Foward Vector
Defined for Vector3, the Forward vector points to -Z
