target_sources(
  crimild_core

  PRIVATE Async.cpp
  PUBLIC Async.hpp
  PUBLIC debounce.hpp
  PRIVATE Job.cpp
  PUBLIC Job.hpp
  PRIVATE JobScheduler.cpp
  PUBLIC JobScheduler.hpp
  PUBLIC WorkStealingDeque.hpp
)
