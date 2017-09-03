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

v4.6.0
------

* Math
    * Compute inverse transformation

* Core
    * Implemented version control for streams
    * Allow for time offsets when specifying skin animations
    * Allow particle system to be paused and resumed
    * Fixed UISystem
    * Fixed invalid camera problem when picking
    * Allow the console to use the command history
    * Fixed particle system emition rate

* Rendering
    * Composite Render Passes
    * Fixed ambient light calculation in standard shader
    * Shadows improvements
    * Use OpenGL Core Profile 3.3

* Navigation
    * Implemented Navigation Meshes
    * Load navmeshes from OBJ files

* Scripting
    * Allow nodes to define a radius for the bounding volume in lua scripts

* Debug
    * Improved debug facilities


v4.5.0
------

* Foundation
    * Define platform-independent types to be used throughout the engine

* Math
    * Implemented time scales for all clocks
    * Improved random number generator

* Core
    * Implemented modular particle system
    * Implemented simple interactive console for simulations
    * Improved streaming support on most classes
    * Improved text support

* Debug
    * Improved Profiler
    * Improved render tools
    * Implemented debug text rendering

* Rendering
    * Implemented filters for textures
    * Improvements on shadow mapping

* Scripting
    * Implemented generic object builder for Lua parser

v4.4.1
------

* Scripting
    * Fixed initial state for cameras in script builder

* Rendering
    * Fallback to OpenGL ES 2 on Android
    * Fixed occluder check when computing render queue

v4.4.0
------

* Math
    * Implemented euler angle conversion for quaternions.
    * Implemented helper methods to get world up vector in local space

* SceneGraph
    * Improved Switch
    * Bind lights only if enabled

* Simulation
    * Update scene in main thread, nodes in parallel
    * Dispatch mouse motion event during input's update phase
    * Retain scene when doing update
    * Fixed concurrency bug in AssetManager
    * Added basic joystick support

* Scripting
    * Added helper method to get a child evaluator

* Rendering
    * Added support for render to texture using an offscreen camera
    * Fixed light type bug in OpenGL
    * Improved Depth of Field image effect
    * Improved Vignette image effect

* Build System
    * Fixed CMake script for file extraction
    * Fixed build system for Visual Studio

v4.3.0
------

* Math
    * Improved Clock

* Core
    * New Job System for concurrency
    * Removed old task manager
    * New logging facilitites
    * Implemented BillboardComponent class

* Rendering
    * Improve render queues to sort objects based on type
    * Add compare functions to DepthState
    * Removed deprecated render passes
    * Promote raytracing-related code to a core module
    * Fixed viewport-related bug when beginning a frame

* iOS
    * Updated iOS project

v4.2.0
------

* Math
	* Return best match in intersection test
	* Implemented pow function in numeric
	* Implemented times function in Matrix and Vector to perform element-wise multipliciation
	* Added method in Ray class to compute interpolated point in ray
	* Fixed polynomial root solver bug
	* Added helper constants in Vector class for known vectors
    * Improved numerical methods
    * Fixed Quaternion SLERP interpolation
    * Allow transformations to be created form a 4x4 matrix

* Core
    * Implemented NonCopyable classes (replacing CRIMILD_DISALLOW_COPY_AND_ASSIGN macro)
    * Implemented simple low-overhead RTTI facilities
    * Improved SkinnedMesh animation
	* Implemented streaming for objects in a scene
	* Fixed concurrency problems with render queues.
	* Fixed cloning for skinned mesh
    * Fixed FileSystem base directory bug
	* Check if message dispatcher instances are valid before invoking them
	* Implemented ArcPrimitive
    * Save skinned mesh (if any) when updating render state for a scene
	* Avoid discarding groups on culling pass.
	* New particle system
	* Implemented progress callback for animations. Fixed time for animation reset

* Rendering
	* Implemented StandardRenderPass
    * Deprecated ForwardRenderPass
    * Deprecated BasicRenderPass
    * Implemented PostRenderPass for post-processing
    * Moved render passes to their own folder
	* Implemented ColorMaskState for occluders
    * Create and configure the screen buffer as early as possible
    * ForwardRenderPass will compute shadow maps in all platforms
    * Fixed shadow map shaders
    * Generate auxiliary FBOs on Renderer setup and store them in asset cache
	* Use GLEW from sources instead of as a submodule
	* Fixed directional lighting rendering issue
    * Improved debug render tools
    * (Experimental) Metal support on iOS/tvOS

* Simulation
    * Clear AssetManager when stopping a simulation
	* Improved deinitialization for simulation and subsystems
    * Implemented axis state in input facilities

* Scripting
    * Enable/Disable verbose logging for scripting from CMake
 	* Improved light support on lua scene builder
 	* Build new particle system from Lua script
    * Enhanced Lua builder to support physical entities

* Audio
	* No changes

* Physics
	* Fixed ground check for rigid bodies
	* Implemented function to detect ground collisions in rigid bodies
    * Implemented standard colliders for physical objects
    * Improved rigid body configuration for physical objects
    * Allowed to apply linear velocity to bodies during simulation

Testing:
    * Fixed existing tests
    * Implement new tests for streaming
    * Modified CMake scripts to include GTest cases
    
    
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


