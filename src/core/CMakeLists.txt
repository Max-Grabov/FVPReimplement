set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/)

add_library(Core
  ${CORE_SRC}/fvp.cpp
)

target_include_directories(Core PUBLIC
  ${CORE_SRC}
)

target_link_libraries(Core PUBLIC Formats Image Audio)

