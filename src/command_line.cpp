#include "ros/ros.h"

int main(int argc, char **argv)
{
  ros::init(argc, argv, "command_line");
  ros::NodeHandle n;

  //ros::Rate loop_rate(60);

  while (ros::ok())
  {
    std::cout << "test" << std::endl;
    //loop_rate.sleep();
    ros::spinOnce();
  }
  
  return 0;
}