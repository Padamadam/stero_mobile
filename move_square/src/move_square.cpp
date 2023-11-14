#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"
#include <cmath>
#include <yaml-cpp/yaml.h>


float myround(float const &num, const int precision){
  return roundf(num*pow(10, precision))/pow(10, precision);
}

bool is_close(float a, float b, float epsilon) {
    return std::abs(a - b) < epsilon;
}


geometry_msgs::Twist& setForwardSpeed(geometry_msgs::Twist& goal, float vel_x){
      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = vel_x;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      return goal;
}

int main(int argc, char **argv){


/* Init move_square node*/
  ros::init(argc, argv, "move_square_node");

  /* Init node handle*/
  ros::NodeHandle n;

  /* Create publisher to publish robot odometry*/
  ros::Publisher publish_move = n.advertise<geometry_msgs::Twist>("/key_vel", 1000);
  ros::Rate loop_rate(10);

  
  float vel_x = 0.0;

  float state0_time;
  float state1_time;
  float state2_time;
  float state4_time;

  float duration; 
  int rotation_time = 0;

  float constvel_time = 2.0;
  float final_vel = 1.0;

  /* Read yaml parameters */
  int is_right;
  n.getParam("/is_right", is_right);

  float side_lenght;
  n.getParam("/side_length", side_lenght);

  float constvel;
  n.getParam("/constvel", constvel);


  /* Setting beginning of state-machine */
  int state = 0;

  while (ros::ok())
  {
    /* Initializing goal message to publisher */
    geometry_msgs::Twist goal;

    /* State 0 - resets measurements from previous iteration*/
    if(state == 0){
      state0_time = 0.0;  /* Unused */
      state1_time = 0.0;
      state2_time = 0.0;
      state4_time = 0.0;
      duration = 0.0;


      state = 1;
    }

    /* State 1 - accelerate to yaml param velocity */
    if(state == 1){
      /* Read state1_time that was reset to 0 by ROS - NOT USED */
      if(state1_time == 0.0) state1_time = ros::Time::now().toSec();

      /* Hardcoded acceleration rate */
      vel_x += 0.05;

      goal = setForwardSpeed(goal, vel_x);

      /* Publish set velocity */
      publish_move.publish(goal);

      ROS_INFO("vel_x [%f]", vel_x);
      ROS_INFO("final_vel [%f]", final_vel);

      /* Check whether final_vel was reached */
      if(is_close(vel_x, final_vel, 0.06)) state = 2; 

    /* State 2 - move at constant speed */
    }else if(state == 2){
      /* Read reset state2_time */
      if(state2_time == 0.0) state2_time = ros::Time::now().toSec();

      goal = setForwardSpeed(goal, final_vel);

      /* Publish constant velocity every iteration*/
      publish_move.publish(goal);

      /* Check how long does state2 last*/
      float duration = ros::Time::now().toSec() - state2_time;
      ROS_INFO("duration [%f]", duration, 2);
      ROS_INFO("state2_time [%f]", state2_time);

      /* Check whether movement with constant velocity lasted enough*/
      if(is_close(duration, constvel_time, 0.001)) state = 3;

    /* State 3 - deaccelerating*/
    }else if(state == 3){

      /* Deacceleration rate*/
      vel_x -= 0.05;

      /* Set decreasing speed */
      goal = setForwardSpeed(goal, vel_x);

      publish_move.publish(goal);

      /* Check whether robot stopped */
      if(is_close(vel_x, 0.0, 0.01))  state = 4; 

      /* State 4 - rotating*/
    }else if(state == 4){

      /* Read state4 begin time*/
      if(state4_time == 0.0) state4_time = ros::Time::now().toSec();

      //obliczyc ile ma sie obracac

      // na sam koniec sprawdzic czy minal wskazany czas na obrot

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      if(is_right == 0){
        /* pi/8 radians/sec = 22.5 degrees/sec */
        goal.angular.z = M_PI/8;
      }else if (is_right == 1){
        goal.angular.z = -M_PI/8;
      }
      

      goal.linear.x = 0.0;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;


      publish_move.publish(goal);

      rotation_time = ros::Time::now().toSec() - state4_time;

      /* Check whether robot finished rotating*/
      if(is_close(rotation_time, 4.0, 0.0001)) state = 0;

    }

    ros::spinOnce();

    loop_rate.sleep();
   }
 
 
   return 0;
 }