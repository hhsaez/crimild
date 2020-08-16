Crimild
=======
A scene graph based framework for interactive 3D applications

Development Diaries
===================
Check out Crimild's blog at http://crimild.hhsaez.com

Building
========

**Note:** In order for the Vulkan shader compiler to work correctly, make sure there is no `glslang` directory in VULKAN_SDK/macOS/include
That way, the compiler uses the header files that are in `third-party` instead.

MSYS
----
```
cmake . -G "MSYS Makefiles" -i
```

XCode
-----
```
cmake . -G XCode
```

Licensing
=========

Crimild is distributed under the BSD license. See the LICENSE file for details

Contributing
============

The easiest way to contribute is to submit pull requests with your changes against the master branch in GitHub.
