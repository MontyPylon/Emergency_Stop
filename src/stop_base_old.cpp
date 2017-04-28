#include <ros/ros.h>
#include <tf/tf.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
#include <actionlib/server/simple_action_server.h>
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseArray.h"

#include <vector>
#include <iostream>

bool stopBase = true;

int main(int argc, char **argv)
{
  ros::init(argc, argv, "stop_base");
  ros::NodeHandle n;

  actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> ac("move_base",true);
  move_base_msgs::MoveBaseGoal goal;
  
  while (ros::ok())
  {

    std::cout << "STOPPING ROBOT" << std::endl;
    //set the header
    goal.target_pose.header.stamp = ros::Time::now();
    goal.target_pose.header.frame_id = "/base_link";
    
    //set relative x, y, and angle
    goal.target_pose.pose.position.x = 0.0;
    goal.target_pose.pose.position.y = 0.0;
    goal.target_pose.pose.position.z = 0.0;
    goal.target_pose.pose.orientation = tf::createQuaternionMsgFromYaw(0);

	//send the goal
    ac.sendGoal(goal);
    
    //block until the action is completed
    //ac.waitForResult();
  }
  return 0;
}
