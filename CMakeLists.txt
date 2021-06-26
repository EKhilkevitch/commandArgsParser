
cmake_minimum_required( VERSION 2.6 )

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif()

set( EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/" )
set( LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib/" )

if ( NOT LIBRARY_TYPE )
  set( LIBRARY_TYPE STATIC )
endif()

message( "LIBRARY_TYPE = ${LIBRARY_TYPE}" )
message( "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )

if ( UNIX )
  set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS} -Wall -Wextra -std=c89 -pedantic") 
endif()

add_subdirectory( ./source/commandArgsParser/ )
add_subdirectory( ./source/tests/ )

