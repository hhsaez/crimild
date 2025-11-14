---
date: Crimild's Development Log
author: H. Hernan Saez
---

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

