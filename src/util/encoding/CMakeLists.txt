set(ENCODING_UTIL_SRC ${UTIL_SRC}/encoding)

add_library(EncodingUtil
  ${ENCODING_UTIL_SRC}/encoding.cpp
)

target_include_directories(EncodingUtil PUBLIC
  ${ENCODING_UTIL_SRC}
  ${PROJECT_ROOT}
)
