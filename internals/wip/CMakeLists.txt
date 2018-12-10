if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU" AND
    "${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O2")
endif()

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS} /O2")
endif()

add_conventional_executable(wip)
target_link_libraries(wip PUBLIC polyfill_v1)
