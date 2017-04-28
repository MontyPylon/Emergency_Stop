#include "ros/ros.h"
#include "std_msgs/String.h"
#include <ros/ros.h>
#include <tf/tf.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
#include <actionlib/server/simple_action_server.h>
#include "geometry_msgs/Pose.h"
#include "geometry_msgs/PoseArray.h"

/**
 * This tutorial demonstrates simple sending of messages over the ROS system.
 */
int main(int argc, char **argv)
{
  ros::init(argc, argv, "stop_base");
  ros::NodeHandle n;

  ros::Publisher stop_pub = n.advertise<actionlib_msgs::GoalID>("/move_base/cancel", 1000);


  ros::Rate loop_rate(10);

  actionlib_msgs::GoalID goal;

  while (ros::ok())
  {
    goal.stamp = ros::Time::now();
    goal.id = "{}";

    stop_pub.publish(goal);
    ros::spinOnce();
    loop_rate.sleep();
  }


  return 0;
}