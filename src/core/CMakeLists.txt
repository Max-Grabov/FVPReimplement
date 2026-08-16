set(CORE_SRC ${CMAKE_CURRENT_SOURCE_DIR})

option(USE_SDL3 "Use SDL3 Library" ON)

if(${USE_SDL3})
  set(SDL_X11_XTEST OFF)
  FetchContent_Declare(
    sdl3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG release-3.4.8
  )
  FetchContent_MakeAvailable(sdl3)
endif()

add_library(Core
  ${CORE_SRC}/fvp.cpp
  ${CORE_SRC}/audio_playback.cpp
)

target_include_directories(Core PUBLIC
  ${CORE_SRC}
  ${PROJECT_ROOT}
)

target_link_libraries(Core PUBLIC FileUtil Formats Image Audio SDL3::SDL3)

