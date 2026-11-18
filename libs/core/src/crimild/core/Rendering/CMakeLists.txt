target_sources(
  crimild_core

  PRIVATE Buffer.cpp
  PUBLIC Buffer.hpp
  PRIVATE BufferAccessor.cpp
  PUBLIC BufferAccessor.hpp
  PRIVATE BufferView.cpp
  PUBLIC BufferView.hpp
  PUBLIC Catalog.hpp
  PUBLIC ColorBlendState.hpp
  PRIVATE ColorMaskState.cpp
  PUBLIC ColorMaskState.hpp
  PRIVATE CommandBuffer.cpp
  PUBLIC CommandBuffer.hpp
  PUBLIC CompareOp.hpp
  PUBLIC ComputePass.hpp
  PUBLIC DepthStencilState.hpp
  PUBLIC DescriptorSet.hpp
  PUBLIC Extent.hpp
  PRIVATE Font.cpp
  PUBLIC Font.hpp
  PUBLIC Format.hpp
  PUBLIC Framebuffer.hpp
  PRIVATE FrameGraphOperation.cpp
  PUBLIC FrameGraphOperation.hpp
  PRIVATE FrameGraphResource.cpp
  PUBLIC FrameGraphResource.hpp
  PRIVATE Image.cpp
  PUBLIC Image.hpp
  PRIVATE ImageManager.cpp
  PUBLIC ImageManager.hpp
  PRIVATE ImageTGA.cpp
  PUBLIC ImageTGA.hpp
  PRIVATE ImageView.cpp
  PUBLIC ImageView.hpp
  PRIVATE IndexBuffer.cpp
  PUBLIC IndexBuffer.hpp
  PRIVATE Material.cpp
  PUBLIC Material.hpp
  PUBLIC Pipeline.hpp
  PUBLIC RasterizationState.hpp
  PUBLIC RenderableSet.hpp
  PRIVATE Renderer.cpp
  PUBLIC Renderer.hpp
  PRIVATE RenderPass.cpp
  PUBLIC RenderPass.hpp
  PRIVATE RenderQueue.cpp
  PUBLIC RenderQueue.hpp
  PUBLIC RenderResource.hpp
  PRIVATE RenderState.cpp
  PUBLIC RenderState.hpp
  PRIVATE Sampler.cpp
  PUBLIC Sampler.hpp
  PUBLIC ScalingMode.hpp
  PUBLIC ScenePass.hpp
  PRIVATE Shader.cpp
  PUBLIC Shader.hpp
  PUBLIC ShaderLibrary.hpp
  PRIVATE ShaderLocation.cpp
  PUBLIC ShaderLocation.hpp
  PRIVATE ShaderProgram.cpp
  PUBLIC ShaderProgram.hpp
  PRIVATE ShaderProgramLibrary.cpp
  PUBLIC ShaderProgramLibrary.hpp
  PRIVATE ShaderUniform.cpp
  PUBLIC ShaderUniform.hpp
  PRIVATE ShaderUniformImpl.cpp
  PUBLIC ShaderUniformImpl.hpp
  PRIVATE ShadowMap.cpp
  PUBLIC ShadowMap.hpp
  PRIVATE SkinnedMesh.cpp
  PUBLIC SkinnedMesh.hpp
  PRIVATE StorageBuffer.cpp
  PUBLIC StorageBuffer.hpp
  PUBLIC Swapchain.hpp
  PRIVATE SystemFont.cpp
  PUBLIC SystemFont.hpp
  PUBLIC SystemFontData.hpp
  PRIVATE Texture.cpp
  PUBLIC Texture.hpp
  PUBLIC UniformBuffer.hpp
  PUBLIC Vertex.hpp
  PRIVATE VertexAttribute.cpp
  PUBLIC VertexAttribute.hpp
  PRIVATE VertexBuffer.cpp
  PUBLIC VertexBuffer.hpp
  PRIVATE VertexLayout.cpp
  PUBLIC VertexLayout.hpp
  PUBLIC Viewport.hpp
  PUBLIC ViewportDimensions.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE Attachment.test.cpp
    PRIVATE Buffer.test.cpp
    PRIVATE BufferAccessor.test.cpp
    PRIVATE BufferView.test.cpp
    PRIVATE CommandBuffer.test.cpp
    PRIVATE DescriptorSet.test.cpp
    PRIVATE Image.test.cpp
    PRIVATE ImageView.test.cpp
    PRIVATE IndexBuffer.test.cpp
    PRIVATE Material.test.cpp
    PRIVATE Pipeline.test.cpp
    PRIVATE RenderPass.test.cpp
    PRIVATE Sampler.test.cpp
    PRIVATE Shader.test.cpp
    PRIVATE ShaderProgram.test.cpp
    PRIVATE SkinnedMesh.test.cpp
    PRIVATE Texture.test.cpp
    PRIVATE UniformBuffer.test.cpp
    PRIVATE Vertex.test.cpp
    PRIVATE VertexAttribute.test.cpp
    PRIVATE VertexBuffer.test.cpp
    PRIVATE VertexLayout.test.cpp
  )
endif ()

add_subdirectory( Materials )
add_subdirectory( Operations )
add_subdirectory( Programs )
add_subdirectory( Uniforms )
