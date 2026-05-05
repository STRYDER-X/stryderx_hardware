#!/usr/bin/env python3

import rclpy
from geometry_msgs.msg import Twist
from rclpy.node import Node
from Rosmaster_Lib import Rosmaster
from sensor_msgs.msg import Joy

# X-Box 360
LT = 2  # Forward
RT = 5  # Reverse

ESC_MIN_ANGLE = 80
ESC_NEUTRAL_ANGLE = 90
ESC_MAX_ANGLE = 100


class MotorControlNode(Node):
    def __init__(self):
        super().__init__("motor_control_node")
        self.robot_ = Rosmaster(car_type=4)
        self.clamp_esc_angle_ = self.declare_parameter("clamp_esc_angle", True).value
        self.vel_sub_ = self.create_subscription(Twist, "~/cmd_vel", self.motor_control_cb, 5)
        self.joystick_sub_ = self.create_subscription(Joy, "/joy", self.joystick_cb, 5)
        self.get_logger().info("Motor Control Node Intialized.")

    def joystick_cb(self, msg: Joy) -> None:
        """
        Callback for joystick input to control the ESC throttle.
        Maps dual-trigger input to a combined PWM angle.

        Args:
            msg (Joy): Joystick message.
        """
        # Forward Logic (Right Trigger)
        # Normalize RT from [-1,1] to target angle [80, 90]
        # Calculate deviation from neutral (90) to determine forward intensity
        rt_input = max(-1, min(1, msg.axes[RT]))
        rt_angle = (5 * rt_input) + 85
        rt_offset = rt_angle - ESC_NEUTRAL_ANGLE

        # Reverse Logic (Left Trigger)
        # Normalize RT input from [-1, 1] to target angle [100, 90]
        # Calculate deviation from neutral (90) to determine reverse intensity
        lt_input = max(-1, min(1, msg.axes[LT]))
        lt_angle = 95 - (5 * lt_input)
        lt_offset = lt_angle - ESC_NEUTRAL_ANGLE

        # Create Net Sum logic to determine esc_angle and clamp
        esc_angle = ESC_NEUTRAL_ANGLE + rt_offset + lt_offset
        if self.clamp_esc_angle_:
            esc_angle = max(ESC_MIN_ANGLE, min(ESC_MAX_ANGLE, esc_angle))

        self.get_logger().info(f"Throttle speed to angle:{esc_angle}")
        self.robot_.set_pwm_servo(servo_id=2, angle=esc_angle)

    def cleanup(self):
        self.robot_.set_pwm_servo(servo_id=2, angle=ESC_NEUTRAL_ANGLE)


def main(args=None) -> None:
    rclpy.init(args=args)
    node = MotorControlNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.cleanup()
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
