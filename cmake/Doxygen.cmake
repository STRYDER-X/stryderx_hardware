find_package(Doxygen)

if(BUILD_DOC AND DOXYGEN_FOUND)
    set(DOXYGEN_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/docs")
    set(DOXYGEN_USE_MDFILE_AS_MAINPAGE "${CMAKE_CURRENT_SOURCE_DIR}/README.md")
    set(DOXYGEN_WARN_AS_ERROR YES)
    set(DOXYGEN_WARN_IF_UNDOCUMENTED YES)
    set(DOXYGEN_WARN_IF_DOC_ERROR YES)
    set(DOXYGEN_WARN_NO_PARAM_DOC YES)
    set(DOXYGEN_RECURSIVE YES)
    set(DOXYGEN_FILE_PATTERNS
        "*.hpp"
        "*.cpp"
        "*.h"
        "*.c"
        "*.py"
        "drive_controller"
        "steering_controller"
    )
    set(DOXYGEN_EXTENSION_MAPPING
        "py=Python"
        "no_extension=Python"
    )
    set(DOXYGEN_EXCLUDE_PATTERNS
        "*/third_party/*"
        "*/__pycache__/*"
    )
    set(DOXYGEN_GENERATE_HTML YES)
    set(DOXYGEN_GENERATE_LATEX NO)
    set(DOXYGEN_HAVE_DOT YES)
    set(DOXYGEN_EXTRACT_ALL NO)
    set(DOXYGEN_CLASS_DIAGRAMS YES)
    doxygen_add_docs(docs
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/scripts"
        "${CMAKE_CURRENT_SOURCE_DIR}/README.md"
        COMMENT "[${PROJECT_NAME}] Generating API documentation with validation."
    )
    message(STATUS "[${PROJECT_NAME}] Documentation target 'docs' created.")

elseif(BUILD_DOC AND NOT DOXYGEN_FOUND)
    message(WARNING "[${PROJECT_NAME}] Doxygen not found.")
endif()
