#include "ros/ros.h"
#include "gazebo_msgs/ModelStates.h"
#include "std_msgs/String.h"
#include <geometry_msgs/Twist.h>
#include "nav_msgs/Odometry.h"
#include <cmath>

// TODO zczytac real time z symulacji
// TODO parametry yaml
void odomCallback(const nav_msgs::Odometry& msg)
 {

  // ROS_INFO("Predkosc liniowa x: [%f]", msg.twist.twist.linear.x);
  // ROS_INFO("Predkosc liniowa y: [%f]", msg.twist.twist.linear.y);
  // ROS_INFO("Predkosc liniowa z: [%f]", msg.twist.twist.linear.z);

  // ROS_INFO("Predkosc katowa x: [%f]", msg.twist.twist.angular.x);
  // ROS_INFO("Predkosc katowa y: [%f]", msg.twist.twist.angular.y);
  // ROS_INFO("Predkosc katowa z: [%f]", msg.twist.twist.angular.z);

  // ROS_INFO("Pose x: [%f]", msg.pose.pose.position.x);
  // ROS_INFO("Pose y: [%f]", msg.pose.pose.position.y);
  // ROS_INFO("Pose z: [%f]", msg.pose.pose.position.z);

}


float myround(float const &num){
  return roundf(num*10000)/10000;
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

  float state0_time;
  float state1_time;
  float state2_time;
  float state4_time;

  float duration; 
  int rotation_time = 0;

  float constvel_time = 2.0;
  float final_vel = 1.0;

  // odczytanie czasu symulacji (SimTime) nie uzywany do niczego
  ros::Time time = ros::Time::now();
  double secs = time.toSec();

  while (ros::ok())
  {
    geometry_msgs::Twist goal;

    if(state == 0){
      // reset odmierzonych czasow
      state0_time = 0.0;
      state1_time = 0.0;
      state2_time = 0.0;
      state4_time = 0.0;
      duration = 0.0;

      state1_time = ros::Time::now().toSec();
      ROS_INFO("state0_time [%f]", myround(state0_time));
      state = 1;
    }

    if(state == 1){
      // ros z jakiegos powodu zerowal state_time dlatego nalezy je tu przypisac
      if(state1_time == 0.0) state1_time = myround(ros::Time::now().toSec());

      vel_x += 0.1;
      ROS_INFO("state0_time [%f]", myround(state0_time));

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = vel_x;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      publish_move.publish(goal);

      ROS_INFO("vel_x [%f]", myround(vel_x));
      ROS_INFO("final_vel [%f]", myround(final_vel));
      if(myround(vel_x) == myround(final_vel)) {
        // zmierz czas przyspieszania, chyba nieuzywane
        // acc_time = ros::Time::now().toSec() - state0_time;
        float state2_time = myround(ros::Time::now().toSec());
        ROS_INFO("state2_time [%f]", state2_time);
        state = 2; 
      }

    }else if(state == 2){
      // przypisanie czasu wyzerowanego state2_time do faktycznej wartosci
      if(state2_time == 0.0) state2_time = myround(ros::Time::now().toSec());

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = final_vel;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      publish_move.publish(goal);

      float duration = ros::Time::now().toSec() - state2_time;
      ROS_INFO("duration [%f]", myround(duration));
      ROS_INFO("state2_time [%f]", state2_time);
      if(myround(duration) == constvel_time) state = 3;

    }else if(state == 3){
      vel_x -= 0.1;

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = 0.0;

      goal.linear.x = vel_x;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;

      publish_move.publish(goal);

      float state4_time = myround(ros::Time::now().toSec());

      if(myround(vel_x) == 0.0)  state = 4; 
    }else if(state == 4){
      if(state4_time == 0.0) state4_time = myround(ros::Time::now().toSec());

      //obliczyc ile ma sie obracac

      // na sam koniec sprawdzic czy minal wskazany czas na obrot

      goal.angular.x = 0.0;
      goal.angular.y = 0.0;
      goal.angular.z = M_PI/4; // 45 stopni / sekunde

      goal.linear.x = 0.0;
      goal.linear.y = 0.0;
      goal.linear.z = 0.0;


      publish_move.publish(goal);

      rotation_time = ros::Time::now().toSec() - state4_time;
      // obrot powinien trwac 2 sekundy
      if(round(rotation_time) == 2.0) state = 0;

    }

    ros::spinOnce();

    loop_rate.sleep();
    ++count;
   }
 
 
   return 0;
 }