include_directories (${KPMG_SOURCE_DIR}/include)

set(CXX_FLAGS
 -ggdb
 #-march=native
 -O0
 # -DVALGRIND
 # -DMUDUO_STD_STRING
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 #-Wconversion
 -Wno-unused-parameter
 #-Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 #-Wshadow
 -Wwrite-strings
 # -MMD
 #-std=c++0x
 -rdynamic
 )
 
string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}")
set(KPMG_BUZZER_SRCS buzzer.c) 

 
SET_SOURCE_FILES_PROPERTIES( ${KPMG_BUZZER_SRCS} PROPERTIES COMPILE_FLAGS " ${TRUNK} ")
INCLUDE_DIRECTORIES( /usr/local/include/ )
LINK_DIRECTORIES( /usr/local/lib/ )

add_executable (buzzer_cld ${KPMG_BUZZER_SRCS} )
target_link_libraries (buzzer_cld event2 )

INSTALL(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/buzzer_cld DESTINATION ${KPMG_SOURCE_DIR}/bin)
