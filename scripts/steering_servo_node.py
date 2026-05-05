#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from Rosmaster_Lib.Rosmaster_Lib import Rosmaster
from sensor_msgs.msg import Joy
from std_msgs.msg import Int32

# X-Box 360
LEFT_JOYSTICK = 0

MIN_ANGLE = 40
MAX_ANGLE = 100


class SteeringServoNode(Node):
    def __init__(self) -> None:
        super().__init__("servo_node")
        self.robot = Rosmaster(car_type=4)
        self.steering_sub_ = self.create_subscription(
            Int32, "~/steering_joint/angle", self.steering_cb, 5
        )
        self.joystick_sub_ = self.create_subscription(Joy, "/joy", self.joystick_cb, 5)

    def joystick_cb(self, msg: Joy) -> None:
        """
        Callback for joystick input to control the steering.
        Maps dual-trigger input to a combined PWM angle.

        Args:
            msg (Joy): Joystick message.
        """
        # Normilize left joystick input from [-1, 1] to target angle [40, 100]
        joystick_input = max(-1, min(1, msg.axes[LEFT_JOYSTICK]))
        joystick_angle = 70 - (30 * joystick_input)

        self.get_logger().info(f"Setting steering servo angle to {joystick_angle}")
        self.robot.set_pwm_servo(1, joystick_angle)


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
