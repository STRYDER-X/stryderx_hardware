if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)

  set(AMENT_LINT_AUTO_EXCLUDE
    third_party
  )

  # TODO: Remove once Copyrights are established.
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()