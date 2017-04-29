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

bool globalPushed = false;

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
  
  //ros::Rate loop_rate(60);

  actionlib_msgs::GoalID goal;

  while (ros::ok())
  {
    //std::cout << "loopin" << std::endl;
    if(globalPushed) {
      std::cout << "Sending CANCEL ALL GOALS" << std::endl;
      goal.stamp = ros::Time::now();
      goal.id = "{}";
      stop_pub.publish(goal);
      globalPushed = false;
    }

    //loop_rate.sleep();
    ros::spinOnce();
  }
  
  return 0;
}