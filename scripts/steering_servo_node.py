#!/usr/bin/env python3
import rclpy
from rclpy.node import Node

# from vendor.rosmaster_lib_v3.Rosmaster_Lib.Rosmaster_Lib import Rosmaster
from Rosmaster_Lib.Rosmaster_Lib import Rosmaster
from std_msgs.msg import Int32


class SteeringServoNode(Node):
    def __init__(self) -> None:
        super().__init__("servo_node")

        self.robot = Rosmaster(car_type=4)

        self.steering_sub_ = self.create_subscription(
            Int32, "~/steering_joint/angle", self.steering_cb, 5
        )

    def steering_cb(self, msg: Int32) -> None:
        self.get_logger().info(f"Setting steering servo angle to {msg.data}")
        self.robot.set_pwm_servo(1, msg.data)


def main(args=None) -> None:
    rclpy.init(args=args)
    node = SteeringServoNode()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == "__main__":
    main()
