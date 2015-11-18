#include <iostream>
#include <string>
#include "ros/ros.h"
#include "std_msgs/String.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*
 * Find Command such as
 * 
 * Climb 5 meter
 * Drop 5 meter
 * go forward 10 meter
 * go backward 10 meter
 * go left 10 meter
 * go right 10 meter
 * circle 10 meter
 * return to home
 * land here
 * fly now
 * 
 * */
using namespace std;

void mainVDInterpreter(const std_msgs::String& str);
string voiceData;

int main(int argc, char **argv){
		
	ros::init(argc, argv, "message_interpreter");
	ros::NodeHandle n;
	ros::Subscriber vd_sub = n.subscribe("art_vrd/voice_data", 1, mainVDInterpreter);
	ROS_INFO("Starting Message Interpreter.");
	ros::spin();
	return 0;
}

void mainVDInterpreter(const std_msgs::String& vData){
	
	//const char *filteredVD;
	
	voiceData = vData.data;
	//const char *castedVD = voiceData.c_str();
	//filteredVD = strstr(castedVD,"climb");
	
	ROS_INFO_STREAM( "Voice Data : " << voiceData) ;
}

