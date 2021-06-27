
cmake_minimum_required( VERSION 2.6 )

if ( ENABLE_EXAMPLES )
  
  message( "Examples enabled" )

  if( POLICY CMP0012 )
    cmake_policy(SET CMP0012 NEW)
  endif()

  project( examaple_c )
  add_executable( examaple_c example_c.c )
  target_link_libraries( examaple_c commandArgsParser )
  target_include_directories( examaple_c PRIVATE "${PROJECT_SOURCE_DIR}/../" )
  
  project( examaple_cpp )
  add_executable( examaple_cpp example_cpp.cpp )
  target_link_libraries( examaple_cpp commandArgsParserCpp )
  target_include_directories( examaple_cpp PRIVATE "${PROJECT_SOURCE_DIR}/../" )

else()
  message( "Examples disabed (use -DENABLE_EXAMPLES=1)" )
endif()  



