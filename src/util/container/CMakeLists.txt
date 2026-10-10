set(CONTAINER_UTIL_SRC ${UTIL_SRC}/container)

add_library(ContainerUtil
)

target_include_directories(ContainerUtil PUBLIC
  ${CONTAINER_UTIL_SRC}
  ${PROJECT_ROOT}
)
