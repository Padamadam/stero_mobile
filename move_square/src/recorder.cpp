#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"
#include <cmath>

class Recorder {
public:
    Recorder() : linear_error(0.0), angular_error(0.0), vel_x(0.0) {
    }

    ~Recorder() {
    }


    void requestedValsCallback(const geometry_msgs::Twist& msg) {
        requested_linear_x = msg.linear.x;
        requested_angular_z = msg.angular.z;
    }

    void odomCallback(const nav_msgs::Odometry& msg) {
        linear_error += std::abs(msg.twist.twist.linear.x - requested_linear_x);
        angular_error += std::abs(msg.twist.twist.angular.z - requested_angular_z);

        ROS_INFO("linear.x: [%f]", msg.twist.twist.linear.x);
        ROS_INFO("angular.z: [%f]", msg.twist.twist.angular.z);
    }

    void stateCallback(const gazebo_msgs::ModelStates& msg){
    }

private:
    float linear_error;
    float angular_error;
    float vel_x;
    float requested_linear_x;
    float requested_angular_z;
};

Recorder* recorder_ptr = nullptr;

int main(int argc, char **argv) {
    ros::init(argc, argv, "recorder");
    ros::NodeHandle n;

    Recorder recorder;

    ros::Subscriber real_odom = n.subscribe("mobile_base_controller/odom", 1000, &Recorder::odomCallback, &recorder);
    ros::Subscriber requested = n.subscribe("/key_vel", 1000, &Recorder::requestedValsCallback, &recorder);
    ros::Subscriber real_state = n.subscribe("gazebo/model_states", 1000, &Recorder::stateCallback, &recorder);
    ros::Rate loop_rate(10);

    while (ros::ok()) {
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
}
