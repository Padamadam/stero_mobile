#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"
#include <cmath>
#include <yaml-cpp/yaml.h>


// float myround(float const &num, const int precision){
//   return roundf(num*pow(10, precision))/pow(10, precision);
// }

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

  
  float vel_x;

  float state0_time;
  float state1_time;
  float state2_time;
  float state3_time;
  float state4_time;
  float acc_duration;
  float constvel_duration;
  float iteration_time;
  float rate;

  float duration; 
  float rotation_time;

  /* Read yaml parameters */
  int is_right;
  n.getParam("/is_right", is_right);

  float side_lenght;
  n.getParam("/side_length", side_lenght);

  float constvel;
  n.getParam("/constvel", constvel);

  float acceleration;
  n.getParam("/acceleration", acceleration);


  /* Setting beginning of state-machine */
  int state = 0;

  while (ros::ok())
  {
    /* Initializing goal message to publisher */
    geometry_msgs::Twist goal;

    /* State 0 - resets measurements from previous iteration*/
    if(state == 0){

      state1_time = 0.0;
      state2_time = 0.0;
      state3_time = 0.0;
      state4_time = 0.0;
      iteration_time = 0.0;
      duration = 0.0;
      acc_duration = 0.0;
      constvel_duration = 0.0;
      rotation_time = 0.0;
      rate = 0.0;

      vel_x = 0.0;


      state = 1;
    }

    /* State 1 - accelerate to yaml param velocity */
    if(state == 1){
      /* Read state1_time that was reset to 0 by ROS */
      if(state1_time == 0.0) 
            state1_time = ros::Time::now().toSec();
          
      float iteration_time;

      /* Hardcoded acceleration rate */
      rate = acceleration / loop_rate.expectedCycleTime().toSec();
      vel_x += rate;
      ROS_INFO("Loop rate: [%f]", loop_rate.expectedCycleTime().toSec());
      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = vel_x;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      /* Publish set velocity */
      publish_move.publish(goal);

      /* Measure lenght of one iteration */
      if(iteration_time == 0.0) iteration_time = ros::Time::now().toSec() - state1_time;

      /* Check whether final_vel was reached */
      if(is_close(vel_x, constvel, rate)) {
        /* Check how long acceleration took*/
        acc_duration = ros::Time::now().toSec() - state1_time;
        state = 2; 
      }

    /* State 2 - move at constant speed */
    }else if(state == 2){
      /* Read reset state2_time */
      if(state2_time == 0.0) state2_time = ros::Time::now().toSec();

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = constvel;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      /* Calculate how long should constant velocity take place
                     based on acceleration time*/
      constvel_duration = (side_lenght/constvel) - acc_duration;

      /* Check if yaml parameters make sense */
      if(constvel_duration < 0.001){
        ROS_INFO("Incorrect yaml parameters");
        break;
      }

      /* Publish constant velocity every iteration*/
      publish_move.publish(goal);

      /* Check how long does state2 last*/
      duration = ros::Time::now().toSec() - state2_time;
      // ROS_INFO("constvelduration [%f], duration [%f]", constvel_duration, duration);

      /* Check whether movement with constant velocity lasted enough*/
      if(is_close(duration, constvel_duration, 0.1)) state = 3;

    /* State 3 - deccelerating*/
    }else if(state == 3){
      /* Read state3_time that was reset to 0 by ROS only to make iteration lenght
          same as for acceleration (state1) */
      if(state3_time == 0.0) state3_time = ros::Time::now().toSec();

      /* Decceleration rate*/
      vel_x -= rate*1.04; /* higher decceleration rate to ensure quicker stop due to inertia*/

      /* Set decreasing speed */
      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = vel_x;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      publish_move.publish(goal);

      /* Check whether robot stopped */
      // modle sie do Boga aby on zwalnial tyle samo co przyspieszal xdd
      if(is_close(vel_x, 0.0, rate))  state = 4; 

      /* State 4 - rotating*/
    }else if(state == 4){

      /* Read state4 begin time*/
      if(state4_time == 0.0) state4_time = ros::Time::now().toSec();

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
      if(is_close(rotation_time, 4.0, 0.001)) state = 0;
    }

    ros::spinOnce();

    loop_rate.sleep();
    ROS_INFO("state [%d]", state);
   } 
   return 0;
 }