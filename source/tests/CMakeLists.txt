
cmake_minimum_required( VERSION 2.6 )

if ( ENABLE_TESTS )

  project( selfTest-commandArgsParser ) 

  set ( tests_source_files
          gtest_parser.cpp
      )

  if ( UNIX )
    set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++0x") 
  endif()  

  find_library( gtest NAMES gtest )
  find_library( gtest_main  NAMES gtest_main )

  if ( NOT ${gtest} MATCHES gtest-NOTFOUND )
    message( "Tests enabled" )
    add_executable( selfTest-commandArgsParser ${tests_source_files} )
    target_link_libraries( selfTest-commandArgsParser ${gtest} )
    target_link_libraries( selfTest-commandArgsParser ${gtest_main} )
    target_link_libraries( selfTest-commandArgsParser commandArgsParser )
    target_link_libraries( selfTest-commandArgsParser pthread )
    target_include_directories( selfTest-commandArgsParser PRIVATE "${PROJECT_SOURCE_DIR}/../" )
  else()
    message( "gtest not found, test not build" )
  endif()  
else()
    message( "Tests disabed (use -DENABLE_TESTS=1)" )
endif()

