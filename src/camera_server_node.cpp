#include "camera_server_node.h"

// NOTE : Keep here to prevent namespace pollution
using namespace std::chrono_literals;
namespace stryderx {

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

  cameraSpecs_ = this->GetCameraSpecs();
  if (!cameraSpecs_.has_value()) {
    RCLCPP_FATAL(
        this->get_logger(),
        "%s:: Could not initialize camera hardware. Shutting down server.",
        __func__);
    this->ShutdownServer();
    return;
  }

  cameraName_ = cameraSpecs_->name;
  maxMissedFrameCount_ = timeoutSeconds * cameraSpecs_->fps;

  luminosityPub_ =
      this->create_publisher<std_msgs::msg::Float32>("~/luminosity_value", 10);
  imageCompressedPub_ =
      this->create_publisher<sensor_msgs::msg::CompressedImage>(
          "~/camera/image/compressed", 3);

  startStreamingSrv_ = this->create_service<std_srvs::srv::Trigger>(
      "~/start_streaming",
      [this](const std::shared_ptr<rmw_request_id_t> header,
             const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
             std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
        this->HandleStartRequest(header, request, response);
      });

  pauseStreamingSrv_ = this->create_service<std_srvs::srv::Trigger>(
      "~/pause_streaming",
      [this](const std::shared_ptr<rmw_request_id_t> header,
             const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
             std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
        this->HandlePauseRequest(header, request, response);
      });

  shutdownCameraSrv_ = this->create_service<std_srvs::srv::Trigger>(
      "~/shutdown_server",
      [this](const std::shared_ptr<rmw_request_id_t> header,
             const std::shared_ptr<std_srvs::srv::Trigger::Request> request,
             std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
        this->HandleShutdownRequest(header, request, response);
      });

  RCLCPP_INFO(this->get_logger(), "%s()::Sever started.", __func__);

  timer_ = this->create_wall_timer(33ms, [this]() { this->StartStreaming(); });
}

void CameraServerNode::StartStreaming() {
  if (streamPaused_) {
    return;
  }

  auto frame = CaptureFrame();

  if (!frame.has_value()) {
    missedFrameCount_++;

    if (missedFrameCount_ > maxMissedFrameCount_) {
      RCLCPP_FATAL(this->get_logger(),
                   "%s()::Consecutive frame failures. Shutting down server.",
                   __func__);
      this->ShutdownServer();
    }
    return;
  }

  missedFrameCount_ = 0;

  PublishImage(frame.value());
  PublishLuminosity(frame.value());

  RCLCPP_INFO_ONCE(this->get_logger(), "%s()::Feeds live for [%s].", __func__,
                   cameraName_.c_str());
}

void CameraServerNode::ShutdownServer() {
  if (rclcpp::ok()) {
    RCLCPP_INFO(this->get_logger(), "%s()::Stopped live feeds for [%s].",
                __func__, cameraName_.c_str());
  }
  if (timer_ && !timer_->is_canceled()) {
    timer_->cancel();
  }
  rclcpp::shutdown();
}

void CameraServerNode::PublishLuminosity(const cv::Mat &frame) {
  // NOTE: Create shallow copy of frame to satisfies the non-const 'cv::Mat &'
  // signature of GetLuminosity without duplicating the heavy pixel data.
  cv::Mat non_const_frame = frame;
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

void CameraServerNode::HandleStartRequest(
    const std::shared_ptr<rmw_request_id_t> /*header*/,
    const std::shared_ptr<std_srvs::srv::Trigger::Request> /*request*/,
    std::shared_ptr<std_srvs::srv::Trigger::Response> response) {

  std::string message;

  if (streamPaused_) {
    streamPaused_ = false;
    message = "Resuming live feeds for [" + cameraName_ + "].";
  } else {
    message = "Feeds already live for [" + cameraName_ + "].";
  }

  response->success = true;
  response->message = message;

  RCLCPP_INFO(this->get_logger(), "%s()::%s", __func__,
              response->message.c_str());
}

void CameraServerNode::HandlePauseRequest(
    const std::shared_ptr<rmw_request_id_t> /*header*/,
    const std::shared_ptr<std_srvs::srv::Trigger::Request> /*request*/,
    std::shared_ptr<std_srvs::srv::Trigger::Response> response) {

  std::string message;

  if (!streamPaused_) {
    streamPaused_ = true;
    message = "Pausing live feeds for [" + cameraName_ + "].";
  } else {
    message = "Feeds already paused for [" + cameraName_ + "].";
  }

  response->success = true;
  response->message = message;

  RCLCPP_INFO(this->get_logger(), "%s()::%s", __func__,
              response->message.c_str());
}

void CameraServerNode::HandleShutdownRequest(
    const std::shared_ptr<rmw_request_id_t> /*header*/,
    const std::shared_ptr<std_srvs::srv::Trigger::Request> /*request*/,
    std::shared_ptr<std_srvs::srv::Trigger::Response> response) {
  response->success = true;
  response->message = "Camera server stopped.";

  RCLCPP_INFO(this->get_logger(), "%s()::%s", __func__,
              response->message.c_str());

  this->ShutdownServer();
}
} // namespace stryderx

int main(int argc, char **argv) {
  rclcpp::init(argc, argv);
  auto node =
      std::make_shared<stryderx::CameraServerNode>("RPi4", "USB", 0, 30);
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
