target_sources(
  crimild_core

  PRIVATE Decorator.cpp
  PUBLIC Decorator.hpp
  PRIVATE Inverter.cpp
  PUBLIC Inverter.hpp
  PRIVATE Repeat.cpp
  PUBLIC Repeat.hpp
  PRIVATE RepeatUntilFail.cpp
  PUBLIC RepeatUntilFail.hpp
  PRIVATE RepeatUntilSuccess.cpp
  PUBLIC RepeatUntilSuccess.hpp
  PRIVATE Succeeder.cpp
  PUBLIC Succeeder.hpp
  PUBLIC Throttle.hpp
)
