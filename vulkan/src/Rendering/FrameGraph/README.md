# Crimild Frame Graph

## What is it
It's a structure that defines how a single frame is rendered

## Frame Asset Manager
A container for all assets used by a frame graph. 

### Dynamic Storage
For objects that change in between frames and need to be updated during command recording

### Static Storage
For objects that are uploaded once and never change. 
This storage must be shared among all asset managers in order to avoid duplicating data in GPU.

## Multiple in-flight frame graphs
Since a Frame Graph deals with rendering a single frame, we need multiple frame graphs, each with their own asset manager

# TODO
- [x] Rename RenderGraph to FrameGraph
- [ ] Use WithHandle everywhere (search for m_handle or getHandle)
- [ ] Keep track of objects that have been deelted and need to be removed from the caches (but be carefull with objects that are no longer visible)
- [ ] Draw Primitive
- [ ] Update descriptor sets (only dynamic ones if possible)
- [ ] Render target transitions
- [ ] Render simulation panel
- [ ] Render scene panel
- [ ] Do no wait for command submit
- [ ] Add fence for each primary command buffer (?)





# To think about...
Objects crated by current render operation and not accessed anywhere else:
- Descriptors (and pools/layouts)
- Pipelines
- RenderPass/Framebuffer
- Buffers, images, views, etc...

Objects created outside of the current operation but only needed here:
- Material (assuming a deferred architecture)

Objects created outside of current operation and readed in multiple ones:
- Shadow maps
- Buffers (vertex, index and uniform buffers)
- Render targets



Make submit() method to force wait on command buffer execution. That way I won't have to worry about synchronization for now.

