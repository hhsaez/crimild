---
date: Crimild's Development Log
author: H. Hernan Saez
---

# TODO
- [ ] Bind signals to other signals of the same type, allowing a "hierarhcy" of signals
- [ ] Deprecate crimild::alloc/alloc_unique in favor of std::make_shared and std::make_unique. 

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

