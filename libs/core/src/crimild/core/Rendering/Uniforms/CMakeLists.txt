target_sources(
  crimild_core

  PUBLIC CallbackUniformBuffer.hpp
  PRIVATE CameraViewProjectionUniformBuffer.cpp
  PUBLIC CameraViewProjectionUniformBuffer.hpp
  PRIVATE LightingUniform.cpp
  PUBLIC LightingUniform.hpp
  PRIVATE LightUniform.cpp
  PUBLIC LightUniform.hpp
  PRIVATE ModelUniformBuffer.cpp
  PUBLIC ModelUniformBuffer.hpp
  PRIVATE ModelViewProjectionUniformBuffer.cpp
  PUBLIC ModelViewProjectionUniformBuffer.hpp
)
