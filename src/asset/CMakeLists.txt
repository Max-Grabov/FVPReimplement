set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FORMATS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/formats)
set(AUDIO_SRC ${CMAKE_CURRENT_SOURCE_DIR}/audio)
set(IMAGE_SRC ${CMAKE_CURRENT_SOURCE_DIR}/image)

add_library(Formats
  ${FORMATS_SRC}/bin.cpp
  ${FORMATS_SRC}/hcb.cpp
  ${FORMATS_SRC}/file_view.cpp
)

add_library(Audio
  ${AUDIO_SRC}/decoder.cpp
  ${AUDIO_SRC}/audio_stream.cpp
  ${AUDIO_SRC}/audio_playback.cpp
)

add_library(Image
  ${IMAGE_SRC}/image_decoder.cpp
  ${IMAGE_SRC}/image.cpp
)

target_include_directories(Formats PUBLIC  
  ${FORMATS_SRC} 
  ${UTIL_SRC}
)

target_include_directories(Audio PUBLIC
  ${AUDIO_SRC}
  ${UTIL_SRC}
)

target_include_directories(Image PUBLIC
  ${IMAGE_SRC}
  ${UTIL_SRC}
)

option(USE_SDL3 "Use SDL3 Library" ON)

# We fetch content so users don't have to worry about installing vorbis library
include(FetchContent)

FetchContent_Declare(
  vorbis
  GIT_REPOSITORY https://github.com/xiph/vorbis.git
  GIT_TAG v1.3.7
)

if(${USE_SDL3})
  set(SDL_X11_XTEST OFF)
  FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.8
  )
  FetchContent_MakeAvailable(sdl3)
endif()

FetchContent_Declare(
  ZStr
  GIT_REPOSITORY    https://github.com/mateidavid/zstr
  GIT_TAG           master
)

FetchContent_MakeAvailable(ZStr)

FetchContent_MakeAvailable(vorbis)

target_link_libraries(Audio PRIVATE vorbis)

target_link_libraries(Image PUBLIC zstr::zstr)

if(${USE_SDL3})
  # Maybe find a way to link to only audio? TODO
  target_link_libraries(Audio PUBLIC SDL3::SDL3)
endif()

