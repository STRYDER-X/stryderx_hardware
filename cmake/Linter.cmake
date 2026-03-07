if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  set(ament_cmake_uncrustify_CONFIG_FILE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/.uncrustify.cfg")
  set(AMENT_LINT_AUTO_EXCLUDE
    third_party
  )

  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()
