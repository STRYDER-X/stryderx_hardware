#pragma once
#include "camera.h"
#include "camera_utils.h"
#include <cv_bridge/cv_bridge.h>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_srvs/srv/trigger.hpp>

// namespace stryderx {

constexpr char CAMERA_SERVER_NODE_NAME[] = "camera_server_node";
class CameraServerNode : public rclcpp::Node, protected camera_driver::Camera {
public:
  CameraServerNode();
  CameraServerNode(std::string name, std::string type, int index, int fps);
  ~CameraServerNode();

private:
  rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr luminosityPub_;
  rclcpp::Publisher<sensor_msgs::msg::CompressedImage>::SharedPtr
      imageCompressedPub_;

  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr startStreamingSrv_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr pauseStreamingSrv_;
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr shutdownCameraSrv_;

  rclcpp::TimerBase::SharedPtr timer_;

  std::optional<camera_utils::CameraInfo> cameraSpecs_;
  std::string cameraName_;
  int missedFrameCount_ = 0;

  bool streamPaused_ = false;

  void InitializeServer();
  void StartStreaming();
  void ShutdownServer();

  void PublishLuminosity(const cv::Mat &frame);
  void PublishImage(const cv::Mat &frame);

  void HandleStartRequest(
      const std::shared_ptr<rmw_request_id_t> header,
      const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
      std::shared_ptr<std_srvs::srv::Trigger::Response> response);
  void HandleShutdownRequest(
      const std::shared_ptr<rmw_request_id_t> header,
      const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
      std::shared_ptr<std_srvs::srv::Trigger::Response> response);
  void HandlePauseRequest(
      const std::shared_ptr<rmw_request_id_t> header,
      const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
      std::shared_ptr<std_srvs::srv::Trigger::Response> response);
};

// } // namespace stryderx