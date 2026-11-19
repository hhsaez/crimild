target_sources(
  crimild_core

  PRIVATE Console.cpp
  PUBLIC Console.hpp
  PUBLIC ConsoleCommand.hpp
)

add_subdirectory( Commands )
