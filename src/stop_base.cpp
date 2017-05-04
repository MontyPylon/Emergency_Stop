#include "ros/ros.h"
#include "std_msgs/String.h"
#include <tf/tf.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
#include <actionlib/server/simple_action_server.h>
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseArray.h"
#include "geometry_msgs/Twist.h"
#include <std_msgs/Bool.h>
#include <iostream>
#include <stdexcept>
#include <stdio.h>
#include <string>

bool globalPushed = false;

std::string exec(const char* cmd) {
    char buffer[128];
    std::string result = "";
    FILE* pipe = popen(cmd, "r");
    if (!pipe) throw std::runtime_error("popen() failed!");
    try {
        while (!feof(pipe)) {
            if (fgets(buffer, 128, pipe) != NULL)
                result += buffer;
        }
    } catch (...) {
        pclose(pipe);
        throw;
    }
    pclose(pipe);
    return result;
}

void emergency_pushed(const std_msgs::Bool::ConstPtr& pushed)
{
  if(pushed->data) {
    ROS_INFO("EMERGENCY BUTTON PRESSED!");
    globalPushed = true;
  }
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "stop_base");
  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("/emergency_pushed", 1000, emergency_pushed);

  ros::Publisher stop_pub = n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);
  ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);

  actionlib_msgs::GoalID goal;

  actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base",true);
  move_base_msgs::MoveBaseGoal move_goal;

  geometry_msgs::Twist twist;

  while (ros::ok())
  {
    //std::cout << "loopin" << std::endl;
    if(globalPushed) {
      
      std::cout << "Sending CANCEL ALL GOALS" << std::endl;
      goal.stamp = ros::Time::now();
      goal.id = "{}";
      stop_pub.publish(goal);
      globalPushed = false;

      std::cout << "Checking for other nodes to cancel" << std::endl;
      std::string nodes_running = exec("rosnode list");
      //std::cout << nodes_running << std::endl;

      std::size_t found1 = nodes_running.find("/teleop_twist_keyboard");
      if (found1 != std::string::npos) {
        //std::cout << "Found teleop_twist_keyboard" << std::endl;
        //std::cout << "Killing teleop_twist_keyboard node" << std::endl;
        system("rosnode kill /teleop_twist_keyboard");

        twist.linear.x = 0; 
        twist.linear.y = 0; 
        twist.linear.z = 0
        twist.angular.x = 0; 
        twist.angular.y = 0; 
        twist.angular.z = 0;
        vel_pub.publish(twist);

        move_goal.target_pose.header.stamp = ros::Time::now();
        move_goal.target_pose.header.frame_id = "/base_link";
        //set relative x, y, and angle
        move_goal.target_pose.pose.position.x = 0.0;
        move_goal.target_pose.pose.position.y = 0.0;
        move_goal.target_pose.pose.position.z = 0.0;
        move_goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(0);
        //send the move_goal
        ac.sendGoal(move_goal);
      }

      std::size_t found2 = nodes_running.find("/action_executor");
      if (found2 != std::string::npos) {
        //std::cout << "Found /action_executor" << std::endl;
        //std::cout << "Killing /action_executor node" << std::endl;

        system("rosnode kill /action_executor");
        move_goal.target_pose.header.stamp = ros::Time::now();
        move_goal.target_pose.header.frame_id = "/base_link";
        //set relative x, y, and angle
        move_goal.target_pose.pose.position.x = 0.0;
        move_goal.target_pose.pose.position.y = 0.0;
        move_goal.target_pose.pose.position.z = 0.0;
        move_goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(0);
        //send the move_goal
        ac.sendGoal(move_goal);
      }

      std::size_t found3 = nodes_running.find("/bwi_kr");
      if (found3 != std::string::npos) {
        //std::cout << "Found /bwi_kr" << std::endl;
        //std::cout << "Killing /bwi_kr node" << std::endl;

        system("rosnode kill /bwi_kr");
        move_goal.target_pose.header.stamp = ros::Time::now();
        move_goal.target_pose.header.frame_id = "/base_link";
        //set relative x, y, and angle
        move_goal.target_pose.pose.position.x = 0.0;
        move_goal.target_pose.pose.position.y = 0.0;
        move_goal.target_pose.pose.position.z = 0.0;
        move_goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(0);
        //send the move_goal
        ac.sendGoal(move_goal);
      }

      // TODO cancel kr execution goals
    }

    //loop_rate.sleep();
    ros::spinOnce();
  }
  
  return 0;
}