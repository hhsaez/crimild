target_sources(
  crimild_core

  PUBLIC AnimatorComponent.hpp
  PRIVATE AudioListenerComponent.cpp
  PUBLIC AudioListenerComponent.hpp
  PRIVATE AudioSourceComponent.cpp
  PUBLIC AudioSourceComponent.hpp
  PRIVATE BillboardComponent.cpp
  PUBLIC BillboardComponent.hpp
  PRIVATE FreeLookCameraComponent.cpp
  PUBLIC FreeLookCameraComponent.hpp
  PRIVATE LambdaComponent.cpp
  PUBLIC LambdaComponent.hpp
  PRIVATE MaterialComponent.cpp
  PUBLIC MaterialComponent.hpp
  PRIVATE MotionStateComponent.cpp
  PUBLIC MotionStateComponent.hpp
  PRIVATE NodeComponent.cpp
  PUBLIC NodeComponent.hpp
  PUBLIC NodeComponentCatalog.hpp
  PRIVATE OrbitComponent.cpp
  PUBLIC OrbitComponent.hpp
  PRIVATE PathComponent.cpp
  PUBLIC PathComponent.hpp
  PRIVATE RenderStateComponent.cpp
  PUBLIC RenderStateComponent.hpp
  PRIVATE RotationComponent.cpp
  PUBLIC RotationComponent.hpp
  PUBLIC SingleValueComponent.hpp
  PRIVATE SkinnedMeshComponent.cpp
  PUBLIC SkinnedMeshComponent.hpp
  PRIVATE TimerComponent.cpp
  PUBLIC TimerComponent.hpp
  PRIVATE UIResponder.cpp
  PUBLIC UIResponder.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE MaterialComponent.test.cpp
    PRIVATE MotionStateComponent.test.cpp
  )
endif ()
