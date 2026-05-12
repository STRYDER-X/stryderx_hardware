/**
 * @file joystick_teleop.h
 * @author Julian A. Rendon (jarendon10@gmail.com)
 * @brief ROS 2 node for managing joystick teleoperation inputs and converting
 * them into velocity values.
 * @version 0.1
 * @date 2026-05-11
 *
 * @copyright Copyright (c) 2026
 *
 */

#ifndef STRYDERX_JOYSTICK_TELEOP_H
#define STRYDERX_JOYSTICK_TELEOP_H

#include <memory>

#include "geometry_msgs/msg/twist.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/joy.hpp"

/**
 * @namespace stryderx
 * @brief Namespace for stryderx related code.
 */
namespace stryderx
{

/**
 * @brief Defines the name of the joystick teleop node.
 */
  constexpr char JOYSTICK_TELEOP_NODE_NAME[] = "joystick_teleop_node";

/**
 * @class JoystickTeleop
 * @brief A ROS 2 Node that manages joystick teleoperation inputs and converts
 * them into velocity values.
 * This class inherits from rclcpp::Node and subscribes to joystick messages
 * to publish twist messages for velocity control.
 */
  class JoystickTeleop: public rclcpp::Node
  {
public:
    /**
     * @brief Construct a new JoystickTeleop object from ROS parameters.
     */
    JoystickTeleop();

private:
    /**
     * @brief Get the joystick axis value.
     *
     * @param msg Shared pointer to the joystick message.
     * @param axis Index of the axis to retrieve.
     * @return double Value of joystick axis, 0.0 as safe guard.
     */
    double GetAxisValue(const sensor_msgs::msg::Joy::SharedPtr msg, const int axis);

    /**
     * @brief Callback function for joystick messages.
     * @param msg Shared pointer to the received joystick message.
     */
    void JoyCB(const sensor_msgs::msg::Joy::SharedPtr msg);

    /** @brief Subscriber for joystick input messages. */
    rclcpp::Subscription < sensor_msgs::msg::Joy > ::SharedPtr joySub_;

    /** @brief Publisher for velocity commands. */
    rclcpp::Publisher < geometry_msgs::msg::Twist > ::SharedPtr cmdVelPub_;

    /** @brief Axis index for steering input. */
    int leftJoystickAxis_;

    /** @brief Axis index for reverse throttle input. */
    int leftTriggerAxis_;

    /** @brief Axis index for forward throttle input. */
    int rightTriggerAxis_;
  };

} // namespace stryderx

#endif // STRYDERX_JOYSTICK_TELEOP_H
