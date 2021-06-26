
project( commandArgsParser ) 

set( source_c_files 
       internal/strcopy.c
       internal/optiontype.c
       internal/option.c
       internal/fileitem.c
       internal/map.c
       internal/parser.c
       parser.c
   )

 #set( source_cpp_files
 #    parser_cpp.cpp
 # )

add_library( commandArgsParser ${LIBRARY_TYPE} ${source_c_files} )
target_include_directories( commandArgsParser PRIVATE "${PROJECT_SOURCE_DIR}/.." )

if ( WIN32 )
  target_compile_definitions( commandArgsParser PRIVATE "-D_CRT_SECURE_NO_DEPRECATE=1" )
endif()

#add_library( commandArgsParserCpp ${LIBRARY_TYPE} ${source_cpp_files} )
#target_include_directories( commandArgsParserCpp PRIVATE "${PROJECT_SOURCE_DIR}/.." )
#target_link_libraries( commandArgsParserCpp commandArgsParser )

