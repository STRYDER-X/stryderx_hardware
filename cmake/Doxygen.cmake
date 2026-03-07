find_package(Doxygen)

if(DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs")
    set(DOXYFILE_IN "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Doxyfile.in")
    doxygen_add_docs(docs
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src"
    CONFIG_FILE "${DOXYFILE_IN}"
    COMMENT "[${PROJECT_NAME}]Generating API documentation"
    )

    message(STATUS "Documentation target created.")

elseif(BUILD_DOC AND NOT DOXYGEN_FOUND)
    message(WARNING "[${PROJECT_NAME}] Doxygen not found.")
endif()