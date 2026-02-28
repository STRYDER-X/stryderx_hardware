#pragma once
#include "camera.h"
#include "camera_utils.h"
#include <cv_bridge/cv_bridge.h>
#include <memory>
#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/compressed_image.hpp>
#include <std_msgs/msg/float32.hpp>
#include <std_srvs/srv/trigger.hpp>

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
  rclcpp::Service<std_srvs::srv::Trigger>::SharedPtr stopCameraService_;
  rclcpp::TimerBase::SharedPtr timer_;

  void InitializeServer();
  void Start();
  void Stop();
  void PublishLuminosity(const cv::Mat &frame);
  void PublishImage(const cv::Mat &frame);
};