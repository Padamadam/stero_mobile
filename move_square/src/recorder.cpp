#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"
#include <cmath>
#include <fstream>

float full_error; /* Error after full square */
float linear_error; /* Error of velocity */
float angular_error; /* Error of angular speed */
float vel_x; /* Current velocity */

float requested_linear_x; /* Value of requested velocity published on key_vel */
float requested_angular_z; /* Value of requested angular speed published on key_vel */

int state = 0;
std::ofstream outfile("output.txt"); /* Open a text file for writing */

bool is_close(float a, float b, float epsilon) {
    return std::abs(a - b) < epsilon;
}

void requestedValsCallback(const geometry_msgs::Twist& msg){
    requested_linear_x = msg.linear.x;
    requested_angular_z = msg.angular.z;
}

void odomCallback(const nav_msgs::Odometry& msg)
 {

    linear_error += std::abs(msg.twist.twist.linear.x - requested_linear_x);
    linear_error += std::abs(msg.twist.twist.linear.x - requested_linear_x);


    outfile << ros::Time::now().toSec() << "\t" << vel_x << std::endl;

    // if(state == 0){
    //   if (is_close(msg.twist.twist.linear.x, 0.0, 0.001)){
    //     linear_error += std::abs(msg.twist.twist.linear.x - requested_linear_x);
    //   }
    // }

    // /* Acceleration */
    // if(state == 1)
    // {
    //   // zliczac blad dla przyspieszania
    // }
    // /* Constant velocity */
    // if(state == 2)
    // {
    //   // zliczac error dla stalej predkosci
    // }
    // /* Decceleration */
    // if(state == 3)
    // {
    //   // zliczac blad przy hamowaniu
    // }
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

    /* Close the file before exiting the program */
    outfile.close();


   return 0;
 }
 