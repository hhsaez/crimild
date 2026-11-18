target_sources(
  crimild_core

  PRIVATE DistanceToTarget.cpp
  PUBLIC DistanceToTarget.hpp
  PRIVATE HasTargets.cpp
  PUBLIC HasTargets.hpp
  PRIVATE IsAtTarget.cpp
  PUBLIC IsAtTarget.hpp
  PRIVATE TestContextValue.cpp
  PUBLIC TestContextValue.hpp
  PRIVATE TestInputAxis.cpp
  PUBLIC TestInputAxis.hpp
  PRIVATE TestSettingValue.cpp
  PUBLIC TestSettingValue.hpp
)
