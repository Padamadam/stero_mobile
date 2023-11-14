#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"
#include <cmath>

float error_const; // blad dla calego kwadratu
float constvel;
float linear_error;

float requested_linear_x;
float requested_angular_z;

int state = 0;

bool is_close(float a, float b, float epsilon) {
    return std::abs(a - b) < epsilon;
}

void requestedValsCallback(const geometry_msgs::Twist& msg){
    requested_linear_x = msg.linear.x;
    requested_angular_z = msg.angular.z;
}

void odomCallback(const nav_msgs::Odometry& msg)
 {

    if(state == 0){
      if (is_close(msg.twist.twist.linear.x, 0.0, 0.001)){
        linear_error += std::abs(msg.twist.twist.linear.x - requested_linear_x);
      }
    }

    /* Acceleration */
    if(state == 1)
    {
      // zliczac blad dla przyspieszania
    }
    /* Constant velocity */
    if(state == 2)
    {
      // zliczac error dla stalej predkosci
    }
    /* Decceleration */
    if(state == 3)
    {
      // zliczac blad przy hamowaniu
    }
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

      /* Read yaml parameters */
      // TODO KWADRAT
      // float side_lenght;
      // n.getParam("/side_length", side_lenght);

      
    n.getParam("/constvel", constvel);

    ros::Subscriber real_odom = n.subscribe("mobile_base_controller/odom", 1000, odomCallback);
    ros::Subscriber requested = n.subscribe("/key_vel", 1000, requestedValsCallback);
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