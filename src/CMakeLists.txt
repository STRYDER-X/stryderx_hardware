set(CAMERA_SERVER_EXE_SOURCES ${CAMERA_SERVER_EXE_NAME}.cpp)

add_executable(${CAMERA_SERVER_EXE_NAME} ${CAMERA_SERVER_EXE_SOURCES})

ament_target_dependencies(${CAMERA_SERVER_EXE_NAME}
    cv_bridge
    rclcpp
    sensor_msgs
    std_msgs
    std_srvs)

target_link_libraries(${CAMERA_SERVER_EXE_NAME}
    camera_driver
    ${OpenCV_LIBS})

install(TARGETS
    ${CAMERA_SERVER_EXE_NAME}
    DESTINATION lib/${PROJECT_NAME})