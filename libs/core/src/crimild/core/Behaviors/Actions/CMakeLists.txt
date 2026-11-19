target_sources(
  crimild_core

  PRIVATE AnimateContextValue.cpp
  PUBLIC AnimateContextValue.hpp
  PRIVATE AnimateParticleSystem.cpp
  PUBLIC AnimateParticleSystem.hpp
  PRIVATE AnimateSettingValue.cpp
  PUBLIC AnimateSettingValue.hpp
  PRIVATE ClampPosition.cpp
  PUBLIC ClampPosition.hpp
  PRIVATE ClampPositionToView.cpp
  PUBLIC ClampPositionToView.hpp
  PRIVATE ClearTargets.cpp
  PUBLIC ClearTargets.hpp
  PRIVATE CopyTransformFromTarget.cpp
  PUBLIC CopyTransformFromTarget.hpp
  PRIVATE EnableNode.cpp
  PUBLIC EnableNode.hpp
  PRIVATE ExecuteBehavior.cpp
  PUBLIC ExecuteBehavior.hpp
  PRIVATE ExecuteBehaviorOnTarget.cpp
  PUBLIC ExecuteBehaviorOnTarget.hpp
  PRIVATE FindTarget.cpp
  PUBLIC FindTarget.hpp
  PRIVATE FollowPath.cpp
  PUBLIC FollowPath.hpp
  PRIVATE LoadScene.cpp
  PUBLIC LoadScene.hpp
  PRIVATE LookAt.cpp
  PUBLIC LookAt.hpp
  PRIVATE MotionApply.cpp
  PUBLIC MotionApply.hpp
  PRIVATE MotionAvoidOthers.cpp
  PUBLIC MotionAvoidOthers.hpp
  PRIVATE MotionAvoidWalls.cpp
  PUBLIC MotionAvoidWalls.hpp
  PRIVATE MotionComputePathToTarget.cpp
  PUBLIC MotionComputePathToTarget.hpp
  PRIVATE MotionFromInput.cpp
  PUBLIC MotionFromInput.hpp
  PRIVATE MotionReset.cpp
  PUBLIC MotionReset.hpp
  PRIVATE MotionSeek.cpp
  PUBLIC MotionSeek.hpp
  PRIVATE PrintMessage.cpp
  PUBLIC PrintMessage.hpp
  PRIVATE ResetNavigation.cpp
  PUBLIC ResetNavigation.hpp
  PRIVATE Rotate.cpp
  PUBLIC Rotate.hpp
  PRIVATE SetContextValue.cpp
  PUBLIC SetContextValue.hpp
  PRIVATE SetSettingValue.cpp
  PUBLIC SetSettingValue.hpp
  PRIVATE Success.cpp
  PUBLIC Success.hpp
  PRIVATE Transform.cpp
  PUBLIC Transform.hpp
  PRIVATE Trigger.cpp
  PUBLIC Trigger.hpp
  PRIVATE Wait.cpp
  PUBLIC Wait.hpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_core_test

    PRIVATE MotionApply.test.cpp
    PRIVATE MotionFromInput.test.cpp
    PRIVATE MotionReset.test.cpp
    PRIVATE Rotate.test.cpp
  )
endif ()
