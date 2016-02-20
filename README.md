Crimild
=======
A scene graph based framework for interactive 3D applications

Development Diaries
===================
Check out Crimild's blog at http://crimild.wordpress.com

Building
========

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

Release Notes
====================

v4.1.0
------
* Core
 * Fixed aspect calculation for main camera
 * Added dump debug tool
 * Added cloning mechanism for bounding volumes

* Rendering
 * Added flag to textures in order to support different wrap modes
 * Avoid switching rendering states when the queues are empty
 * Wait for message to present next frame
 * Fixed bug in render queue reset
 * [iOS] Create frame buffer objects in with correct resolution
 * [iOS] trigger frame presentation after binding default FBO
 * [OpenGL] Improved frame buffer object catalog to support iOS
 * [OpenGL] Improved error logging
 * [OpenGL] Fix screen shaders to work on iOS
 * [OpenGL] Improved Image Effects to support iOS

* Simulation
 * Compute aspect ratio automatically for Main Camera
 * Allow custom configurations for TaskManager
 * Fixed integration error in Update system
 * Decreased accumulation threshold to at most 4 frames in update system
 * Clear AssetManager cache before loading a scene
 * Add tools for getting the documents directory
 * Make sure there is a main camera before starting components  

* Scripting
 * Use RTTI for builders in Lua Scene Builder
 * Enable on-demand logging
 * Clone nodes in lua builder

* Audio
 * Enable OpenAL in iOS

* Infrastructure
 * Added CMake tool for easier app configuration
 * Added CMake tool for easier app configuration


v4.0.1
------
* Build System
 * Fixed Windows (MSYS) build
 * Fixed Android build
* Misc
 * Removed old COLLADA files


v4.0.0
------
* Foundation
 * Implemented small object allocator
 * Implement explicit ownership for objects
* Math
 * Improved random number generation
 * Renamed TransformationImpl to Transformation (and viceversa)
* Rendering
 * Move shader code to separated files (Ongoing)
 * Move Renderer object cache to AssetManager
 * Improve image effect processing while rendering frames
 * Handle S and D buffers in base RenderPass class
 * [OpenGL] Unify OpenGL renderers (Desktop and Mobile)
 * [OpenGL] Increase GL version to 4 for all desktop shaders
 * [OpenGL] Deprecate GLES code in favor of new renderer
 * [OpenGL] Implemented custom GLSL prefixes with macros for each platforms
 * [OpenGL] Fixed normals mapping artifacts in forward shaders
* Scene Graph
 * Improve text support and tools
 * Improved Text and Font classes to automatically use SDF fonts if avaiable
 * Add flag to Camera class to enable/disable culling (default: enabled)
 * Allow node retrieval from parents using names
 * Implemented shortcut for component retrieval from other components
* Simulation
 * Parallel architecture for simulation
 * Implemented simulation systems
 * Enforce frame synchronization with messages
 * Improved scene loading with streaming system
 * Removed simulation tasks in favor of systems
* Loaders
 * Improve OBJ loader
 * Fixed vertex tanget computation in OBJ loader
* Scripting
 * Lua scene builder
* Misc & Tools
 * Support MinGW in Windows
 * Improved platform detection and macros


