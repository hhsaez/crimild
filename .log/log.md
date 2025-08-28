---
date: Crimild's Development Log
author: H. Hernan Saez
---

# TODO
- [ ] Fix deinit errors when closing `GLFWOpenWindow`s
- [ ] Text encoding
- [ ] Unit tests for memory decoder
- [ ] Text decoding
- [ ] Fix assembly file size increasing bug.
- [ ] Use unique IDs for nodes, links and pins. See comments [here](./20250701_hhsaez.md)
- [ ] Show pin's context menu (do I need this?)
- [ ] Move `Assembly` to `Crimild::Core`
- [ ] Add a transformation node to the Assembly graph
- [ ] Simplify `View` hierarchy. Just have a `draw()` virtual functional. Windows, Modals and other subclasses may include a `drawContent` as needed.
- [ ] Render File System in a splitter panel
- [ ] Handle object selection in Scene3D (maybe storing in the Assembly's Editable extension?)

# MAYBE
- [ ] Create `Identifiable` class. See comments [here](./20250717_hhsaez.md)
- [ ] Use string-based UIDs

# 2025-08-28
- [x] Add keyboard signals to `GLFWOpenGLWindow`
- I decided to have all log notes on a single file. It's a lot easier that way and avoid duplicating a lot of text.

# 2025-08-16
- [x] Universal OpenGL Player: Textures
- Continue working the `UniversalPlayer` library. It's based on OpenGL so it can be used in as many platforms as possible.
- I decided to make as simple/complex as needed. No extra features will be added unless they are required by clients.
- Continue working on the text encoder.
- I'm dealing with a frustrating exception (memory access violation) when trying to run one of the tests that uses `setName()`. Something is wrong with `crimild::alloc` but I can't tell what it is, and neither does Copilot (which is more frustrating). Everything works with `std::make_shared` though.

