---
date: Crimild's Development Log
author: H. Hernan Saez
---

# TODO
- [ ] Handle user interaction (clicks)
- [ ] Cache parent 3D
- [ ] Bind signals to other signals of the same type, allowing a "hierarhcy" of signals
- [ ] RTTI as template<"crimild::Foo">
- [ ] Resources with prototype strategy for clonning (deep, shallow)
- [ ] Move test setup to same CMakeFiles as library
- [ ] Shaders should be reused between all instances of the same material. They are stateless, except for the OpenGL ID.
- [ ] Fix window dimensions for different DPI.

# 2026-04-02
- Working on unit tests for `crimild::experimental::Spatial3D`
- Working on a common interface for `ParentNode` and fixing mixins.

# 2026-04-02
- Working on unit tests for `crimild::experimental::Spatial3D`

# 2026-04-01
- Working on unit tests for `crimild::experimental::Spatial3D`

# 2026-03-28
- [x] CMake presets
- Learning about cmake presets and how to use them here.
- I created a prefix for Linux. We'll do the same for windows and mac later
- Still having isssues with LSP and Emacs on Linux. 
- Working on Nodes and Visitors

# 2026-03-24
- Continue working on a formal definition for assemblies

# 2026-03-23
- Considering representing assemblies as ASTs (abstract syntax trees)

# 2026-03-12
- Still analyzing the benefits/cons of having a base class Node with children
  1. Uniform tree API everywhere: You can attach/traverse/reparent any gameplay object the same way.
  2. Simpler engine plumbing: Serialization, editor tooling, scene loading, signals, lifecycle callbacks, and traversal systems target one abstraction.
  3. Composability: Mixing concerns (render, audio, gameplay, UI helpers) in one tree is easy, including intermediary utility nodes.
  4. Lower cognitive overhead: Users learn one mental model: "everything is a node in a scene tree"
  5. Faster prototyping: You can build features quickly without creating a new hierarchy type each time.
- Switch nodes can be implemented by just enabling/disabling specific children
- Same for MultiSwitch/LODs/etc
- `getChildren` vs `getActiveChildren`

# 2026-02-19
- Trying and fixing selection issues due to ray/bounding intersection not working correctly.
- Found the problem: window dimensions (getWidth(), getHeight()) are returning sizes multiplied by DPI. A 1024x768 window is being returned as 2048x1536. Then, the ray calculation incorrect produces the wrong value due to invalid NDC coordinates.
- How should I fix this?

# 2026-02-18
- Working on boundings and selection.
- New boundings example.
- Selectable nodes as decorators.
- I don't mind overusing dynamic cast. It keeps things simpler and I'm keepining contained to a single algorithm. It should be fine (famous last words). 
- I implemented clicking by using rays. Simple. 
- I like that I'm able to reuse all the math tricks I made for ray tracing rendering, specially the inverse ray trick to test intersection with shapes. 
- Clicking still don't work as expected. I guess there's something wrong in transforming coordinates to NDC. 

# 2026-02-15
- Improving material workflow and binding
- Allow unlit materials to bind different colors
- Allow extensions to be attached with name overrides

# 2026-02-14
- Working on a new example for Bounding3D classes

# 2026-02-12
- ImageTGA is quite old and it was not using the new way of declaring images with buffer views
- I feel like I need to deprecate the entire engine and start over :P

# 2026-02-11
- Fixed triangle example. Now it uses a texture for vertex coloring

# 2026-02-07
- Continue improving the basic OpenGL-based render pass 
- Crimild works on Linux too

# 2026-02-06
- I finally render a triangle using the new crimild::nodes hierarchy

# 2026-02-05
- Moving forward with implementing Triangle3D as an example using the new assembly classes

# 2026-01-30
- I finally feel like I'm moving forward 
- Creating new classes for nodes (Node, Spatial3D, Geometry3D)
- Creating new tests

# 2026-01-29
- I finally decided to go with classes for the new scene hierarchy instead of implementing a graph with views. 
- Moving forward, I now need to work on how the Spatial3D class works and propagates invalidation across the hierarchy

# 2026-01-22
- Continue working on Assembly representation
- I feel like I'm going deeper into a rabit hole without end.
- Now I'm trying with a more simpler approach: Node as an entity with parent/children. And then forget about groups

# 2026-01-21
- Continue working on Assembly representation
- I'm not convinced about the who "ownership" concept for entities. How owns a Resource?
- I think I need a better separation of concerns. A Node has a parent. A resource has consumers.

# 2026-01-19
- Working on assemblies as classes with entities having an owner, which may be different from the scene traversal parent.
- Using Signal<> as a way to invalidate transformations for children
- Got a first implementation working for Spatial3D and children updates for world state. 

# 2026-01-15
- Working on understanding how to represent Assemblies, entities and their relationships
- I'm no longer conviced that an actual graph is the best approach. It makes traversing the scene a very complex process. 
- Alternatively, implementing a DAG with just classes might be the best approach. The graph is left for editing only (which is done by the ImGui library)
- Keep it simple. 
- A DAG is friendlier to execution too. 
- The question is, how to represent parent/child relationships? How's the owner of an entity?
- I found a bug in my custom allocator regarding default alignment set to 4 bytes instead of 8 (or more). This lead to an investigation that resulting in a future deprecation of that allocator, in favor of standard allocation using std::make_shared and std:make_unique, which are more cache friendly. 
- https://github.com/hhsaez/crimild/issues/93 

# 2026-01-14
- Continue working on Assemblies, graphs and entities
- I always considered the graph editor and the scene view as different panels in the editor, but connected. But what if they are the same panel and suddenly all nodes in the graph editor have a physical representation in the 3D world? That's easy to visualize for a 3D model, but what if a behavior has a position and visual geometry that can be manipulated, translated and assigned, all in the 3D space? It would really be like playing with Legos, right?

# 2026-01-12
- Continue working on Assemblies, graphs and entities
- The current pain point is how to support a group of entities and also hierarchical 3D transformations. 

# 2025-11-19
- Moving Assembly to Crimild::Core library

# 2025-11-14
- Finished adding tests for `Signal` class.

# 2025-11-13
- Continue working on improving `Signal` class by adding more unit tests and documentation.

# 2025-11-12
- Finally started working on unit tests for `Signal` class

# 2025-10-24
- Today I merged the `assemblies_experimental` branch to `master`. It was long overdue
- From now on, I would like to focus on smaller PRs, if possible.

# 2025-10-13
- Getting back into dev mode after a while. Feels like I need to learn everything again.
- I decided to keep everything as simple as possible and I'm going to continue using VS Code as my main editor (instead of Emacs). 
- I removed the TODO list from this file, in favor of using a dedicated tool (like a canvas in Obsidian). 

# 2025-08-28
- [x] Add keyboard signals to `GLFWOpenGLWindow`
- I decided to have all log notes on a single file. It's a lot easier that way and avoid duplicating a lot of text.

# 2025-08-16
- [x] Universal OpenGL Player: Textures
- Continue working the `UniversalPlayer` library. It's based on OpenGL so it can be used in as many platforms as possible.
- I decided to make as simple/complex as needed. No extra features will be added unless they are required by clients.
- Continue working on the text encoder.
- I'm dealing with a frustrating exception (memory access violation) when trying to run one of the tests that uses `setName()`. Something is wrong with `crimild::alloc` but I can't tell what it is, and neither does Copilot (which is more frustrating). Everything works with `std::make_shared` though.

