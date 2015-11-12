Crimild
=======
A scene graph based framework for interactive 3D applications

Development Diaries
===================
Check out Crimild's blog at http://crimild.wordpress.com

Licensing
=========
Crimild is distributed under the BSD license. See the LICENSE file for details

Release Notes
====================

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


