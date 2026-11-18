target_sources(
  crimild_core

  PRIVATE Operations_blend.cpp
  PRIVATE Operations_brightPassFilter.cpp
  PRIVATE Operations_channel.cpp
  PRIVATE Operations_computeBRDFLUT.cpp
  PRIVATE Operations_computeBuffer.cpp
  PUBLIC Operations_computeBuffer.hpp
  PRIVATE Operations_computeImage.cpp
  PUBLIC Operations_computeImage.hpp
  PRIVATE Operations_computeIrradianceMap.cpp
  PRIVATE Operations_computePrefilterMap.cpp
  PRIVATE Operations_computeReflectionMap.cpp
  PRIVATE Operations_computeRT.cpp
  PUBLIC Operations_computeRT.hpp
  PRIVATE Operations_convolution.cpp
  PRIVATE Operations_debug.cpp
  PRIVATE Operations_debugLightCascades.cpp
  PUBLIC Operations_debugLightCascades.hpp
  PRIVATE Operations_debugShadowAtlas.cpp
  PUBLIC Operations_debugShadowAtlas.hpp
  PRIVATE Operations_fetchRenderables.cpp
  PRIVATE Operations_forwardUnlitPass.cpp
  PRIVATE Operations_gaussianBlur.cpp
  PRIVATE Operations_gBufferPass.cpp
  PRIVATE Operations_grayscale.cpp
  PRIVATE Operations_iblPass.cpp
  PRIVATE Operations_invert.cpp
  PRIVATE Operations_lightingPass.cpp
  PRIVATE Operations_present.cpp
  PRIVATE Operations_renderShadowAtlas.cpp
  PRIVATE Operations_shader.cpp
  PRIVATE Operations_softRT.cpp
  PUBLIC Operations_softRT.hpp
  PRIVATE Operations_ssao.cpp
  PUBLIC Operations_ssao.hpp
  PRIVATE Operations_tonemapping.cpp
  PUBLIC Operations.hpp
  PRIVATE OperationUtils.cpp
  PUBLIC OperationUtils.hpp
)
