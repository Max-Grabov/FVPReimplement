add_executable(test_suite 
  audio_tests.cpp
  file_view_tests.cpp
  utility_tests.cpp
  image_tests.cpp
)

target_link_libraries(test_suite PUBLIC gtest gtest_main Formats Audio Image)

set_target_properties(test_suite 
  PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY
      "${CMAKE_BINARY_DIR}/tests"
)
