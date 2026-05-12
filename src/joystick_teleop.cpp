/**
 * @file joystick_teleop.cpp
 * @author Julian A. Rendon (jarendon10@gmail.com)
 * @brief ROS 2 node for managing joystick teleoperation inputs and converting them into velocity values.
 * @version 0.1
 * @date 2026-05-11
 *
 * @copyright Copyright (c) 2026
 *
 */
#include "joystick_teleop.h"

namespace stryderx
{
JoystickTeleop::JoystickTeleop() : Node(JOYSTICK_TELEOP_NODE_NAME)
{
  leftJoystickAxis_ = this->declare_parameter<int>("left_joystick", 0);
  leftTriggerAxis_ = this->declare_parameter<int>("left_trigger", 2);
  rightTriggerAxis_ = this->declare_parameter<int>("right_trigger", 5);

  cmdVelPub_ = this->create_publisher<geometry_msgs::msg::Twist>("/cmd_vel", 5);

  joySub_ =
    this->create_subscription<sensor_msgs::msg::Joy>("/joy", 5,
                                                     [this](const sensor_msgs::msg::Joy::SharedPtr
                                                            msg){
      this->JoyCB(msg);
    });

}

double JoystickTeleop::GetAxisValue(const sensor_msgs::msg::Joy::SharedPtr msg, const int axis)
{
  if (axis < 0 || static_cast<std::size_t>(axis) >= msg->axes.size())
  {
    return 0.0;
  }
  return msg->axes[axis];

}

void JoystickTeleop::JoyCB(const sensor_msgs::msg::Joy::SharedPtr msg)
{
  geometry_msgs::msg::Twist cmdVel;

  const double forward = (1.0 - GetAxisValue(msg, rightTriggerAxis_)) / 2.0;
  const double reverse = (1.0 - GetAxisValue(msg, leftTriggerAxis_)) / 2.0;

  cmdVel.linear.x = forward - reverse;
  cmdVel.angular.z = GetAxisValue(msg, leftJoystickAxis_);
  cmdVelPub_->publish(cmdVel);
}
} // namespace stryderx

/**
 * @brief Entry point for the Joystick Teleop Node.
 * Initializes the ROS 2 communications, instantiates the JoystickTeleop
 * and begins the execution loop.
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line arguments.
 * @return int Execution status (0 for success).
 */
int main(int argc, char** argv)
{
  rclcpp::init(argc, argv);
  auto node = std::make_shared<stryderx::JoystickTeleop>();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
