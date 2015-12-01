/*
 * Find Command such as
 * 
 * Climb 5 meter		=>	naik 5 Meter
 * Drop 5 meter			=>	turun 5 Meter
 * go forward 10 meter	=>	maju 10 meter
 * go backward 10 meter	=>	mundur 10 meter
 * go left 10 meter		=>	kiri 10 meter
 * go right 10 meter	=>	kanan 10 meter
 * return to home		=>	pulang ke pangkalan
 * land here			=>	mendarat disini
 * fly now				=>	terbang sekarang
 * 
 * */

#include <iostream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/PoseStamped.h"
// ################ new mavros msg lib is in mavros_msgs ################ 
//#include "mavros/SetMode.h"
//#include "mavros/State.h"
//#include "mavros/OverrideRCIn.h"
//#include "mavros/RCIn.h"
#include "mavros_msgs/SetMode.h"
#include "mavros_msgs/State.h"
#include "mavros_msgs/OverrideRCIn.h"
#include "mavros_msgs/RCIn.h"
// ################ new mavros msg lib is in mavros_msgs ################ 
#include "std_msgs/Int16.h"
#include "std_msgs/Float64.h"
#include "std_msgs/String.h"
#include <unistd.h>
#define COMMAND_LIST 9

void mainMovementController(const std_msgs::String& main_fmc);
void altReceiver(const std_msgs::Float64& alt_msg);
// ################ new mavros msg lib is in mavros_msgs ################ 
//void rcinReceiver(const mavros::RCIn& rc_in_data);
void rcinReceiver(const mavros_msgs::RCIn& rc_in_data);
// ################ new mavros msg lib is in mavros_msgs ################ 

int rc_in_channel_7 = 0;
float rel_alt = 0;
const char* known_command[COMMAND_LIST];
int command_list = COMMAND_LIST;
bool success;
int rc_taken = 1;
int rc_transit_delay = 2000000;
int channel_7_mid = 1500;
// channel_7_off = 987 | channel_7_on = 2010
	
ros::Publisher pub_incoming_reply;
ros::Publisher pub_quad_pos;	
ros::ServiceClient client;
mavros_msgs::SetMode flight;
geometry_msgs::PoseStamped quad_pos;


int main(int argc, char **argv)
{	
	ros::init(argc, argv, "movement_controller");
	ros::NodeHandle n;
	ros::Subscriber vd_sub		= n.subscribe("art_vrd/voice_data", 10, mainMovementController);
	ros::Subscriber rc_in_sub 	= n.subscribe("/mavros/rc/in", 100, rcinReceiver);
	ros::Subscriber sub_rel_alt = n.subscribe("/mavros/global_position/rel_alt", 1, altReceiver );
	client					 	= n.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");
	pub_quad_pos 				= n.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 1000);
	pub_incoming_reply 			= n.advertise<std_msgs::String>("art_vrd/incoming_reply", 100);
    ROS_INFO("Starting Movement Controller.");
	printf("%d\n",command_list);
    
    bool success;
	int last_flightmode_data;
	int rc_taken = 1;
	int rc_transit_delay = 2000000;
	int channel_7_mid = 1500;
	// channel_7_off = 987 | channel_7_on = 2010
	known_command[0] = "naik";
	known_command[1] = "turun";
	known_command[2] = "maju";
	known_command[3] = "mundur";
	known_command[4] = "kiri";
	known_command[5] = "kanan";
	known_command[6] = "pulang ke pangkalan";
	known_command[7] = "mendarat disini";
	known_command[8] = "lepas landas";
	
	ros::spin();
	return 0;
}

// ################ new mavros msg lib is in mavros_msgs ################ 
//void rcinReceiver(const mavros::RCIn& rc_in_data);
void rcinReceiver(const mavros_msgs::RCIn& rc_in_data){
	
	rc_in_channel_7 = rc_in_data.channels[6];
}

void altReceiver(const std_msgs::Float64& alt_msg){

	rel_alt = alt_msg.data;
}

void mainMovementController(const std_msgs::String& vData)
{
	std_msgs::String incoming_reply;
	int x;
	const char* command_search;
	
	if ( vData.data[0] == 's' && vData.data[1] == 'c' ){
		
		for(x=0; x < command_list;x++){
			command_search = strstr(vData.data.c_str(), known_command[x]);			
		}
		
		/*
		quad_pos.header.stamp = ros::Time::now();
		quad_pos.header.frame_id = "1";
		quad_pos.pose.position.x = x_halogen_home;	
		quad_pos.pose.position.y = y_halogen_home; 	
		quad_pos.pose.position.z = 6; 	
		pub_quad_pos.publish(quad_pos);
		*/
		
		if(rc_in_channel_7 > channel_7_mid){

			flight.request.base_mode = 0;				//Set to 0 to use custom_mode
			flight.request.custom_mode = "LOITER";		//Set to '' to use base_mode
			success = client.call(flight);
			// Check for success and print out info.

			if(success){
				ROS_INFO_STREAM( "Flight Mode changed to "<< flight.request.custom_mode ) ;
			} 
			else {
				ROS_ERROR_STREAM( "Failed to changed." ) ;
			}
		}
		
		else if ( rc_in_channel_7 < channel_7_mid) {
			
			ROS_INFO("RC Take Over!");
		}
		
		incoming_reply.data = "OK:FMC";
		pub_incoming_reply.publish(incoming_reply);
		ROS_INFO_STREAM( "It's a sc command") ;
				
	}
	
	else{
		ROS_INFO_STREAM( "It's not a sc command") ;
	}	
	ROS_INFO_STREAM( "Voice Data : " << vData.data) ;
}

