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
  ros::init(argc, argv, "kill_vel");
  ros::NodeHandle n;

  ros::Subscriber sub = n.subscribe("/emergency_pushed", 1000, emergency_pushed);

  actionlib_msgs::GoalID goal;
  ros::Publisher stop_pub = n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);
  
  geometry_msgs::Twist twist;
  ros::Publisher vel_pub = n.advertise<geometry_msgs::Twist>("cmd_vel", 1000);

  actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base",true);
  move_base_msgs::MoveBaseGoal move_goal;

  //ros::Rate r(0.5);

  while (ros::ok())
  {
    //std::cout << "loopin" << std::endl;
    if(globalPushed) {
      globalPushed = false;
      std::cout << "Checking which nodes publish to /cmd_vel:" << std::endl;
      std::string topic_list = exec("rostopic info /cmd_vel");
      std::string delimiter = "\n";
      size_t pos = 0;
      std::string token;

      std::vector<std::string> nodes;
      bool grab = false;
      bool end = false;

      while ((pos = topic_list.find(delimiter)) != std::string::npos && !end) {
          token = topic_list.substr(0, pos);
          if(grab) {
            std::size_t pot_node = token.find("*");
            if(pot_node != std::string::npos) {
              std::string buf; // Have a buffer string
              std::stringstream ss(token); // Insert the string into a stream
              int count = 0;
              while (ss >> buf) {
                if(count == 1 && buf != "/kill_vel") {
                  nodes.push_back(buf);
                  std::cout << buf << std::endl;
                }
                count ++;
              }
            }
          }
          std::size_t found1 = token.find("Publishers:");
          std::size_t found2 = token.find("Subscribers:");
          if (found1 != std::string::npos) {
            //std::cout << "found publishers" << std::endl;
            grab = true;
          } else if(found2 != std::string::npos) {
            //std::cout << "found subscribers" << std::endl;
            grab = false;
            end = true;
          }
          topic_list.erase(0, pos + delimiter.length());
      }

      for(int a = 0; a < nodes.size(); a++) {
        system(("rosnode kill " + nodes[a]).c_str());
        twist.linear.x = 0; 
        twist.linear.y = 0; 
        twist.linear.z = 0;
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
    }

    //r.sleep();
    ros::spinOnce();
  }
  
  return 0;
}