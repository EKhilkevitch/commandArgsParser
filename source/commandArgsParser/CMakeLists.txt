
project( commandArgsParser ) 

set( source_files 
       internal/strcopy.c
       internal/optiontype.c
       internal/option.c
       internal/fileitem.c
       internal/map.c
       internal/parser.c
       parser.c
   )


add_library( commandArgsParser ${LIBRARY_TYPE} ${source_files} )
target_include_directories( commandArgsParser PRIVATE "${PROJECT_SOURCE_DIR}/.." )

if ( WIN32 )
  target_compile_definitions( commandArgsParser PRIVATE "-D_CRT_SECURE_NO_DEPRECATE=1" )
endif()


