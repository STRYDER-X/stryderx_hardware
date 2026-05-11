/**
 * @file camera_server_node.h
 * @author Julian A. Rendon (jarendon10@gmail.com)
 * @brief ROS 2 node for managing camera hardware.
 * @version 0.1
 * @date 2026-03-01
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef STRYDERX__CAMERA_SERVER_NODE_H_
#define STRYDERX__CAMERA_SERVER_NODE_H_

#include <cv_bridge/cv_bridge.h>
#include <memory>
#include <optional>
#include <string>

#include "camera.h"
#include "camera_utils.h"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/compressed_image.hpp"
#include "std_msgs/msg/float32.hpp"
#include "std_srvs/srv/trigger.hpp"

/**
 * @namespace stryderx
 * @brief Namespace for stryderx related code.
 */
namespace stryderx
{

/**
 * @brief Defines the name of the camera server node.
 */
  constexpr char CAMERA_SERVER_NODE_NAME[] = "camera_server_node";

/**
 * @class CameraServerNode
 * @brief A ROS 2 Node that manages camera hardware and publishes
 * image/luminosity data.
 * This class inherits from rclcpp::Node for ROS communication and owns a
 * camera_driver::Camera instance for low-level hardware access.
 */
  class CameraServerNode: public rclcpp::Node
  {
public:
    /**
     * @brief Construct a new CameraServerNode object from ROS parameters.
     */
    CameraServerNode();

    /**
     * @brief Destroy the CameraServerNode object and ensures hardware is
     * released.
     */
    virtual ~CameraServerNode();

private:
    /** @brief Publisher for luminosity data extracted from camera frames. */
    rclcpp::Publisher < std_msgs::msg::Float32 > ::SharedPtr luminosityPub_;

    /** @brief Publisher for JPEG compressed images from the camera feed. */
    rclcpp::Publisher < sensor_msgs::msg::CompressedImage > ::SharedPtr
    imageCompressedPub_;

    /** @brief Service to resume/start the camera stream. */
    rclcpp::Service < std_srvs::srv::Trigger > ::SharedPtr startStreamingSrv_;

    /** @brief Service to pause the camera stream. */
    rclcpp::Service < std_srvs::srv::Trigger > ::SharedPtr pauseStreamingSrv_;

    /** @brief Service to shut down the node and camera hardware. */
    rclcpp::Service < std_srvs::srv::Trigger > ::SharedPtr shutdownCameraSrv_;

    /** @brief Main processing timer to poll the camera and publish data. */
    rclcpp::TimerBase::SharedPtr timer_;

    /** @brief Camera driver configured from ROS parameters. */
    std::optional < camera_driver::Camera > camera_;

    /** @brief Cached hardware specifications from the configured camera. */
    std::optional < camera_utils::CameraInfo > cameraSpecs_;

    /** @brief Limit for consecutive dropped frames before a fatal error is
     * triggered. */
    int maxMissedFrameCount_;

    /** @brief Counter tracking current consecutive missed frames. */
    int missedFrameCount_ = 0;

    /** @brief Internal state indicating if the stream is currently active. */
    bool streamPaused_ = false;

    /** @brief Hardware operation timeout duration in seconds. */
    int timeoutSeconds_;

    /**
     * @brief Sets up publishers, services, and hardware initialization logic.
     */
    void InitializeServer();

    /**
     * @brief Callback function triggered by the wall timer to capture and process
     * frames.
     */
    void StartStreaming();

    /**
     * @brief Stops timers and initiates a clean ROS 2 shutdown sequence.
     */
    void ShutdownServer();

    /**
     * @brief Extracts luminosity and publishes to the topic.
     * @param frame Reference to the current OpenCV matrix frame.
     */
    void PublishLuminosity(const cv::Mat & frame);

    /**
     * @brief Compresses the frame to JPEG and publishes the message.
     * @param frame Reference to the current OpenCV matrix frame.
     */
    void PublishImage(const cv::Mat & frame);

    /**
     * @brief Callback for the start/resume service.
     * @param[in] header RMW request metadata.
     * @param[in] request Empty trigger request.
     * @param[out] response Result status and descriptive message.
     */
    void HandleStartRequest(
      const std::shared_ptr < rmw_request_id_t > header,
      const std::shared_ptr < std_srvs::srv::Trigger::Request > request,
      std::shared_ptr < std_srvs::srv::Trigger::Response > response);

    /**
     * @brief Callback for the pause service.
     * @param[in] header RMW request metadata.
     * @param[in] request Empty trigger request.
     * @param[out] response Result status and descriptive message.
     */
    void HandlePauseRequest(
      const std::shared_ptr < rmw_request_id_t > header,
      const std::shared_ptr < std_srvs::srv::Trigger::Request > request,
      std::shared_ptr < std_srvs::srv::Trigger::Response > response);

    /**
     * @brief Callback for the shutdown service.
     * @param[in] header RMW request metadata.
     * @param[in] request Empty trigger request.
     * @param[out] response Result status and descriptive message.
     */
    void HandleShutdownRequest(
      const std::shared_ptr < rmw_request_id_t > header,
      const std::shared_ptr < std_srvs::srv::Trigger::Request > request,
      std::shared_ptr < std_srvs::srv::Trigger::Response > response);
  };
} // namespace stryderx

#endif // STRYDERX__CAMERA_SERVER_NODE_H_
