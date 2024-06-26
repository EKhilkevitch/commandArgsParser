
cmake_minimum_required( VERSION 2.6 )
project( commandArgsParser ) 

if( NOT CMAKE_BUILD_TYPE )
  set( CMAKE_BUILD_TYPE Release )
endif()

set( EXECUTABLE_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/bin/" )
set( LIBRARY_OUTPUT_PATH "${PROJECT_SOURCE_DIR}/lib/" )

set( CMAKE_POSITION_INDEPENDENT_CODE ON )

if ( NOT LIBRARY_TYPE )
  set( LIBRARY_TYPE STATIC )
endif()

message( "LIBRARY_TYPE = ${LIBRARY_TYPE}" )
message( "CMAKE_BUILD_TYPE = ${CMAKE_BUILD_TYPE}" )

if ( UNIX )
  set( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   -Wall -Wextra -pedantic -ansi -std=c89") 
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -std=c++98" )
endif()

add_subdirectory( ./source/commandArgsParser/ )
add_subdirectory( ./source/tests/ )
add_subdirectory( ./source/examples/ )

