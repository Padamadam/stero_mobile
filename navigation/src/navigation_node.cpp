#include "ros/ros.h"
#include <geometry_msgs/PoseStamped.h>
#include <tf/transform_listener.h>
#include <costmap_2d/costmap_2d_ros.h>
#include <dwa_local_planner/dwa_planner_ros.h>
#include <nav_core/base_global_planner.h>
#include <global_planner/planner_core.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>

std::vector<geometry_msgs::PoseStamped> plan;
geometry_msgs::PoseStamped current_robot_pose;
geometry_msgs::PoseStamped start_pose, goal_pose;
bool goal_set = false;

void odomCallback(nav_msgs::Odometry odom_msg) {
    odom_msg.header.frame_id = "map";
    current_robot_pose.header = odom_msg.header;
    current_robot_pose.pose.position = odom_msg.pose.pose.position;
    current_robot_pose.pose.orientation = odom_msg.pose.pose.orientation;
}

void navGoalCallback(geometry_msgs::PoseStamped msg){
    msg.header.frame_id = "map";
    current_robot_pose.header.frame_id = "map";
    start_pose = current_robot_pose;
    start_pose.header.frame_id = "map";
    goal_pose = msg;
    goal_set = true;
}

int main(int argc, char **argv){

    ros::init(argc, argv, "Navigation");

    tf2_ros::Buffer buffer(ros::Duration(10));
    ros::NodeHandle n;
    tf2_ros::TransformListener tf(buffer);

    costmap_2d::Costmap2DROS globalCostmap("global_costmap", buffer);
    costmap_2d::Costmap2DROS localCostmap("local_costmap", buffer);

    global_planner::GlobalPlanner globalPlanner("global_planner", globalCostmap.getCostmap(), "map");

    dwa_local_planner::DWAPlannerROS dp;
    dp.initialize("my_dwa_planner", &buffer, &localCostmap);
  
    ros::Subscriber set_goal = n.subscribe<geometry_msgs::PoseStamped>("/move_base_simple/goal", 1000, navGoalCallback);

    ros::Subscriber odom_sub = n.subscribe<nav_msgs::Odometry>("/mobile_base_controller/odom", 1000, odomCallback);

    /* Tiago nasluchuje na navel*/
    ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("/nav_vel", 1000);

    ros::Rate loop_rate(10);

    start_pose.header.frame_id = "map";

    geometry_msgs::Twist cmd_vel;

    while (ros::ok())
    {
    // sprawdzac ze goal set byl ustawiony
        if(goal_set){
            globalPlanner.makePlan(current_robot_pose, goal_pose, plan);
            dp.setPlan(plan);
            if(dp.computeVelocityCommands(cmd_vel)){
                // Add this after dp.computeVelocityCommands(cmd_vel);
                ROS_INFO("Computed velocity commands: linear=(%f, %f), angular=%f", cmd_vel.linear.x, cmd_vel.linear.y, cmd_vel.angular.z);

                vel_pub.publish(cmd_vel);
            }
        }
        ros::spinOnce();
        loop_rate.sleep();
    }

    return 0;
 }