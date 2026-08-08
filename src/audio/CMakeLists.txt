set(AUDIO_SRC ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Audio
  ${AUDIO_SRC}/decoder.cpp
  ${AUDIO_SRC}/audio_stream.cpp
)

target_include_directories(Audio PUBLIC
  ${AUDIO_SRC}
  ${PROJECT_ROOT}
)

# We fetch content so users don't have to worry about installing vorbis library
include(FetchContent)

FetchContent_Declare(
  vorbis
  GIT_REPOSITORY https://github.com/xiph/vorbis.git
  GIT_TAG v1.3.7
)

FetchContent_MakeAvailable(vorbis)

target_link_libraries(Audio PRIVATE Util vorbis)

