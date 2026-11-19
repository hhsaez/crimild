target_sources(
  crimild_core

  PRIVATE EchoConsoleCommand.cpp
  PUBLIC EchoConsoleCommand.hpp
  PRIVATE SetConsoleCommand.cpp
  PUBLIC SetConsoleCommand.hpp
)
