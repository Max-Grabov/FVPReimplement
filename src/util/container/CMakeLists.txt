set(CONTAINER_UTIL_SRC ${UTIL_SRC}/container)

add_library(ContainerUtil INTERFACE
)

target_include_directories(ContainerUtil INTERFACE
  ${CONTAINER_UTIL_SRC}
  ${PROJECT_ROOT}
)
