set(ENGINE_SRC ${CORE_SRC}/engine)

add_library(Engine
  ${ENGINE_SRC}/save_utilities.cpp
)

target_include_directories(Engine PUBLIC
  ${ENGINE_SRC}
  ${PROJECT_ROOT}
)
