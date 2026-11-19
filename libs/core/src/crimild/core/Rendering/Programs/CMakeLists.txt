target_sources(
  crimild_core

  PRIVATE LitShaderProgram.cpp
  PUBLIC LitShaderProgram.frag
  PUBLIC LitShaderProgram.hpp
  PUBLIC LitShaderProgram.vert
  PRIVATE SkyboxShaderProgram.cpp
  PUBLIC SkyboxShaderProgram.hpp
  PRIVATE UnlitShaderProgram.cpp
  PUBLIC UnlitShaderProgram.hpp
)
