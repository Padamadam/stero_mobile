#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"
#include <cmath>

void odomCallback(const nav_msgs::Odometry& msg)
 {

    // ROS_INFO("Predkosc liniowa x: [%f]", msg.twist.twist.linear.x);

    // ROS_INFO("Predkosc katowa z: [%f]", msg.twist.twist.angular.z);

    // ROS_INFO("Pose x: [%f]", msg.pose.pose.position.x);
    // ROS_INFO("Pose y: [%f]", msg.pose.pose.position.y);
}

void stateCallback(const gazebo_msgs::ModelStates& msg){

}

int main(int argc, char **argv)
{
 
    ros::init(argc, argv, "recorder");


    ros::NodeHandle n;

    ros::Subscriber real_odom = n.subscribe("mobile_base_controller/odom", 1000, odomCallback);
    ros::Subscriber real_state = n.subscribe("gazebo/model_states", 1000, stateCallback);
    ros::Rate loop_rate(10);

    // 1. odczytac pozycje poczatkowa
    // 2. wyliczyc wspolrzedne pod koniec automatu
    // 3. wyliczyc blad

    while (ros::ok())
    {
    ros::spinOnce();

    loop_rate.sleep();
    }


   return 0;
 }