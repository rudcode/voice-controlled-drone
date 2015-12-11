#include "ros/ros.h"
#include "std_msgs/String.h"
#include "std_msgs/Int16.h"
#include <geometry_msgs/Point.h>
#include <stdlib.h>
#include <iostream>
#include <string>
/**
 * This tutorial demonstrates simple receipt of messages over the ROS system.
 */
char stateData;
std::string spin_recv = "aldwin";

void chatterCallback(const std_msgs::String& str)
{
	ROS_INFO_STREAM( "string 1 received") ;
	spin_recv = "akbar";
}

void chatterCallback2(const std_msgs::Int16& integer)
{
  ROS_INFO_STREAM( "\nInteger Value = " << integer.data) ;
}

void chatterCallback3(const std_msgs::String& str3)
{
	
	ROS_INFO_STREAM( "aldwin") ;
	while(spin_recv != "akbar" && ros::ok()){
		ros::spinOnce();
	}
	ROS_INFO_STREAM( "akbar Receive") ;
}


int main(int argc, char **argv)
{

  ros::init(argc, argv, "subscriber");
  ros::NodeHandle n;
  ros::Subscriber sub = n.subscribe("krti15/string", 1000, chatterCallback);
  ros::Subscriber sub2 = n.subscribe("krti15/flight_mode", 1000, chatterCallback2);
  ros::Subscriber sub3 = n.subscribe("krti15/string3", 1000, chatterCallback3);
  ros::spin();
  return 0;
}
