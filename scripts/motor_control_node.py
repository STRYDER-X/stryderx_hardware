#!/usr/bin/env python3

import time

import rclpy
from geometry_msgs.msg import Twist
from rclpy.node import Node
from Rosmaster_Lib import Rosmaster


class MotorControlNode(Node):
    def __init__(self):
        super().__init__("motor_control_node")
        self.robot_ = Rosmaster(car_type=4)

        self.vel_sub_ = self.create_subscription(Twist, "~/cmd_vel", self.motor_control_cb, 5)
        self.get_logger().info("Motor Control Node Intialized.")

    def motor_control_cb(self, msg):
        self.robot_.set_pwm_servo(servo_id=2, angle=msg.linear.x)
        self.get_logger().info(f"Publishing: Linear.x =[{msg.linear.x}]")

        time.sleep(2)
        # Reset to prevent continous output
        self.robot_.set_pwm_servo(servo_id=2, angle=0)

    def cleanup(self):
        self.robot_.set_pwm_servo(servo_id=2, angle=0)


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
