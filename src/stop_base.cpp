#include "ros/ros.h"
#include "std_msgs/String.h"
#include <tf/tf.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
#include <actionlib/server/simple_action_server.h>
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseArray.h"
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
  actionlib_msgs::GoalID goal;

  actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base",true);
  move_base_msgs::MoveBaseGoal moveGoal;
  

  while (ros::ok())
  {
    //std::cout << "loopin" << std::endl;
    if(globalPushed) {
      std::cout << "Sending CANCEL ALL GOALS" << std::endl;
      goal.stamp = ros::Time::now();
      goal.id = "{}";
      stop_pub.publish(goal);
      globalPushed = false;


      
      std::cout << "--------------------------" << std::endl;
      std::cout << "Killing teleop_twist_keyboard node" << std::endl;
      system("rosnode kill teleop_twist_keyboard");

      moveGoal.target_pose.header.stamp = ros::Time::now();
      moveGoal.target_pose.header.frame_id = "/base_link";
      //set relative x, y, and angle
      moveGoal.target_pose.pose.position.x = 0.0;
      moveGoal.target_pose.pose.position.y = 0.0;
      moveGoal.target_pose.pose.position.z = 0.0;
      moveGoal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(0);
      //send the moveGoal
      ac.sendGoal(moveGoal);
    }

    //loop_rate.sleep();
    ros::spinOnce();
  }
  
  return 0;
}