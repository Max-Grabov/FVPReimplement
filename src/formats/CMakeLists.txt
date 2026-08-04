set(FORMATS_SRC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Formats
  ${FORMATS_SRC}/bin.cpp
  ${FORMATS_SRC}/persistent_file.cpp
)

target_include_directories(Formats PUBLIC  
  ${FORMATS_SRC} 
  ${UTIL_SRC}
)
