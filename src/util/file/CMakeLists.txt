set(FILE_UTIL_SRC ${UTIL_SRC}/file)

add_library(FileUtil
  ${FILE_UTIL_SRC}/mapped_file.cpp
)

target_include_directories(FileUtil PUBLIC
  ${FILE_UTIL_SRC}
  ${PROJECT_ROOT}
)
