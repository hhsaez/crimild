target_sources(
  crimild_foundation
  PUBLIC Enableable.hpp
  PUBLIC Key.hpp
  PUBLIC KeyValuePair.hpp
  PUBLIC Macros.hpp
  PUBLIC Named.hpp
  PUBLIC NamedObject.hpp
  PUBLIC ObjectFactory.hpp
  PUBLIC Observable.hpp
  PUBLIC RTTI.hpp
  PUBLIC SharedObject.hpp
  PUBLIC Singleton.hpp
  PUBLIC StringUtils.hpp
  PUBLIC Types.hpp
  PUBLIC Version.hpp
  PUBLIC Versionable.hpp
  PRIVATE NamedObject.cpp
  PRIVATE ObjectFactory.cpp
  PRIVATE Version.cpp
)

if ( CRIMILD_BUILD_TESTS )
  target_sources(
    crimild_foundation_test
    PRIVATE Invariant.test.cpp
    PRIVATE NamedObject.test.cpp
    PRIVATE Named.test.cpp
    PRIVATE RTTI.test.cpp
    PRIVATE Version.test.cpp
  )
endif ()

