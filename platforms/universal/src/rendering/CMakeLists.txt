crimild_trace()

target_sources(
  crimild_universal

  PRIVATE OpenGLBindable.hpp
  PRIVATE OpenGLIndexBufferBindable.cpp
  PRIVATE OpenGLIndexBufferBindable.hpp
  PRIVATE OpenGLMaterialBindable.cpp
  PRIVATE OpenGLMaterialBindable.hpp
  PRIVATE OpenGLUnlitMaterialBindable.cpp
  PRIVATE OpenGLUnlitMaterialBindable.hpp
  PRIVATE OpenGLPrimitiveBindable.cpp
  PRIVATE OpenGLPrimitiveBindable.hpp
  PRIVATE OpenGLShader.cpp
  PRIVATE OpenGLShader.hpp
  PRIVATE OpenGLShaderProgram.cpp
  PRIVATE OpenGLShaderProgram.hpp
  PRIVATE OpenGLTextureBindable.cpp
  PRIVATE OpenGLTextureBindable.hpp
  PRIVATE OpenGLVertexBufferBindable.cpp
  PRIVATE OpenGLVertexBufferBindable.hpp
)

