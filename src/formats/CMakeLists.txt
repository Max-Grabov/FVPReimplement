set(FORMATS_SRC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Formats
  ${FORMATS_SRC}/bin.cpp
)

target_include_directories(Formats PUBLIC  
  ${FORMATS_SRC} 
  ${PROJECT_ROOT}
)

target_link_libraries(Formats PRIVATE FileUtil)
