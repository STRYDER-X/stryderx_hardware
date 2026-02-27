#include "camera_server_node.h"

CameraServerNode::CameraServerNode() : Node(CAMERA_SERVER_NODE_NAME), Camera() {
  InitializeServer();
}

CameraServerNode::CameraServerNode(std::string name, std::string type,
                                   int index, int fps)
    : Node(CAMERA_SERVER_NODE_NAME), Camera(name, type, index, fps) {
  InitializeServer();
}

CameraServerNode::~CameraServerNode() {
  RCLCPP_INFO(this->get_logger(), "%s()::Server stopped.", __func__);
}

void CameraServerNode::InitializeServer() {
  RCLCPP_INFO(this->get_logger(), "%s()::Setting up camera server.", __func__);
  luminosityPub_ =
      this->create_publisher<std_msgs::msg::Float32>("/luminosity_value", 10);
  imageCompressedPub_ =
      this->create_publisher<sensor_msgs::msg::CompressedImage>(
          "/camera/image/compressed", 3);

  timer_ = this->create_wall_timer(std::chrono::milliseconds(CAMERA_FPS_MS),
                                   std::bind(&CameraServerNode::Start, this));
}

void CameraServerNode::Start() {
  RCLCPP_INFO_ONCE(this->get_logger(), "%s()::Sever started.", __func__);

  auto frame = CaptureFrame();
  if (frame.has_value()) {
    PublishImage(frame.value());
    PublishLuminosity(frame.value());
  } else {
    RCLCPP_ERROR(this->get_logger(), "%s()::Failed to capture frame.",
                 __func__);
    Stop();
  }
}

void CameraServerNode::Stop() {
  RCLCPP_INFO(this->get_logger(), "%s()::Stopping server.", __func__);
  rclcpp::shutdown();
}

void CameraServerNode::PublishImage(const cv::Mat &frame) {
  cv_bridge::CvImage cv_image;
  cv_image.header.stamp = this->now();
  cv_image.header.frame_id = "camera_frame";
  cv_image.encoding = sensor_msgs::image_encodings::BGR8;
  cv_image.image = frame;

  auto compressed_image = std::make_shared<sensor_msgs::msg::CompressedImage>();
  compressed_image->header = cv_image.header;
  compressed_image->format = "jpeg";
  cv::imencode(".jpg", cv_image.image, compressed_image->data);

  imageCompressedPub_->publish(*compressed_image);
}

void CameraServerNode::PublishLuminosity(const cv::Mat &frame) {
  // TODO: Remove once newer version of camera driver pkg is implemented.
  // NOTE: GetLuminosity only accepts non-const variables
  cv::Mat non_const_frame = frame.clone();

  auto luminosity = camera_utils::GetLuminosity(non_const_frame);
  if (luminosity) {
    auto luminosityMsg = std_msgs::msg::Float32();
    luminosityMsg.data = luminosity.value();
    luminosityPub_->publish(luminosityMsg);
    RCLCPP_DEBUG(this->get_logger(), "%s()::Luminosity value - [%.2f]",
                 __func__, luminosityMsg.data);
  } else {
    RCLCPP_WARN(this->get_logger(),
                "%s()::Failed to get luminosity from the frame.", __func__);
  }
}

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<CameraServerNode>("RPi4", "USB", 0, 30);
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}