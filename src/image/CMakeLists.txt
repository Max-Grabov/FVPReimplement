set(IMAGE_SRC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Image
  ${IMAGE_SRC}/image_decoder.cpp
  ${IMAGE_SRC}/image.cpp
)

target_include_directories(Image PUBLIC
  ${IMAGE_SRC}
  ${PROJECT_ROOT}
)

FetchContent_Declare(
  ZStr
  GIT_REPOSITORY    https://github.com/mateidavid/zstr
  GIT_TAG           master
)

FetchContent_MakeAvailable(ZStr)

target_link_libraries(Image PUBLIC Util zstr::zstr)

