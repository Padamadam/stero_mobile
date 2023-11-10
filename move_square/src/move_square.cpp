#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"

// TODO zczytac real time z symulacji
// TODO parametry yaml
void odomCallback(const nav_msgs::Odometry& msg)
 {

  ROS_INFO("Predkosc liniowa x: [%f]", msg.twist.twist.linear.x);
  ROS_INFO("Predkosc liniowa y: [%f]", msg.twist.twist.linear.y);
  ROS_INFO("Predkosc liniowa z: [%f]", msg.twist.twist.linear.z);

  ROS_INFO("Predkosc katowa x: [%f]", msg.twist.twist.angular.x);
  ROS_INFO("Predkosc katowa y: [%f]", msg.twist.twist.angular.y);
  ROS_INFO("Predkosc katowa z: [%f]", msg.twist.twist.angular.z);

  ROS_INFO("Pose x: [%f]", msg.pose.pose.position.x);
  ROS_INFO("Pose y: [%f]", msg.pose.pose.position.y);
  ROS_INFO("Pose z: [%f]", msg.pose.pose.position.z);



}

int main(int argc, char **argv){
 
  ros::init(argc, argv, "move_square");

  ros::NodeHandle n;
 
  ros::Publisher publish_move = n.advertise<geometry_msgs::Twist>("/key_vel", 1000);
  ros::Subscriber real_odom = n.subscribe("mobile_base_controller/odom", 1000, odomCallback);
  // ros::Subscriber real_position = n.subscribe("gazebo/model_states", 1000, positionCallback);

  ros::Rate loop_rate(10);

  int count = 0;
  int state = 0;
  float vel_x = 0.0;
  float vel_y = 0.0; 

  int acc_time = 0;
  int deacc_time = 0;
  int obrot_time = 0;

  int constvel_time = 0;
  float final_vel = 1.0;

  while (ros::ok())
  {
    geometry_msgs::Twist goal;

    if(state == 0){

      vel_x += 0.1;

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = vel_x;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      acc_time += 1;

      publish_move.publish(goal);

      ROS_INFO("acc tiume: [%d]", acc_time);

      if(roundf(vel_x * 1000)/1000 == roundf(final_vel * 1000)/1000)  state = 1; 

    }else if(state == 1){

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = final_vel;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      publish_move.publish(goal);

      constvel_time += 1;

      if(constvel_time == acc_time) state = 2;
    }else if(state == 2){
      vel_x -= 0.1;

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = vel_x;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      deacc_time += 1;

      publish_move.publish(goal);

      ROS_INFO("acc tiume: [%d]", acc_time);

      if(roundf(vel_x * 100)/100 == 0.0)  state = 3; 
    }else if(state == 3){
      // sprawdzic czas symulacji

      //obliczyc ile ma sie obracac

      // na sam koniec sprawdzic czy minal wskazany czas na obrot

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 2.0;

      goal.linear.x = 0.0;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      obrot_time += 1;

      publish_move.publish(goal);

      if(obrot_time == acc_time) state = 0;

    }


    ROS_INFO("State: [%d]", state);

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
   }
 
 
   return 0;
 }