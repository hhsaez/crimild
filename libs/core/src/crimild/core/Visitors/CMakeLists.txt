target_sources(
  crimild_core

  PRIVATE Apply.cpp
  PUBLIC Apply.hpp
  PRIVATE ApplyToGeometries.cpp
  PUBLIC ApplyToGeometries.hpp
  PRIVATE BinTreeScene.cpp
  PUBLIC BinTreeScene.hpp
  PRIVATE ComputeRenderQueue.cpp
  PUBLIC ComputeRenderQueue.hpp
  PRIVATE FetchCameras.cpp
  PUBLIC FetchCameras.hpp
  PRIVATE FetchGeometries.cpp
  PUBLIC FetchGeometries.hpp
  PRIVATE FetchLights.cpp
  PUBLIC FetchLights.hpp
  PRIVATE IntersectWorld.cpp
  PUBLIC IntersectWorld.hpp
  PRIVATE NodeVisitor.cpp
  PUBLIC NodeVisitor.hpp
  PRIVATE ParallelApply.cpp
  PUBLIC ParallelApply.hpp
  PRIVATE Picking.cpp
  PUBLIC Picking.hpp
  PRIVATE RTAcceleration.cpp
  PUBLIC RTAcceleration.hpp
  PRIVATE SelectNodes.cpp
  PUBLIC SelectNodes.hpp
  PRIVATE ShallowCopy.cpp
  PUBLIC ShallowCopy.hpp
  PRIVATE StartComponents.cpp
  PUBLIC StartComponents.hpp
  PRIVATE UpdateComponents.cpp
  PUBLIC UpdateComponents.hpp
  PRIVATE UpdateRenderState.cpp
  PUBLIC UpdateRenderState.hpp
  PRIVATE UpdateWorldState.cpp
  PUBLIC UpdateWorldState.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE BinTreeScene.test.cpp
    PRIVATE IntersectWorld.test.cpp
    PRIVATE NodeVisitor.test.cpp
    PRIVATE RTAcceleration.test.cpp
    PRIVATE UpdateRenderState.test.cpp
    PRIVATE UpdateWorldState.test.cpp
  )
endif ()
