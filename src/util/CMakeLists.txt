set(UTIL_SRC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Util
  ${UTIL_SRC}/persistent_file.cpp
)

target_include_directories(Util PUBLIC
  ${UTIL_SRC}
  ${PROJECT_ROOT}
)

