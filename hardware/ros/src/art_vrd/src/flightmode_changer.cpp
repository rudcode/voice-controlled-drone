#include <iostream>
#include <string>
#include "ros/ros.h"
#include "mavros/SetMode.h"
#include "mavros/State.h"
#include "mavros/OverrideRCIn.h"
#include "mavros/RCIn.h"
#include "std_msgs/Int16.h"
#include "std_msgs/Float64.h"
#include <unistd.h>

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
 
void mainFMChanger(const std_msgs::String& main_fmc);
void rcinReceiver(const mavros::RCIn& rc_in_data);
void altReceiver(const std_msgs::Float64& alt_msg);

int rc_in_channel_7 = 0;
float rel_alt = 0;
ros::Publisher pub_incoming_reply;

int main(int argc, char **argv)
{	
	ros::init(argc, argv, "flightmode_changer");
	ros::NodeHandle n;
	ros::Subscriber vd_sub = n.subscribe("art_vrd/voice_data", 10, mainFMChanger);
	ros::Subscriber rc_in_sub = n.subscribe("/mavros/rc/in", 100, rcinReceiver);
	ros::Subscriber sub_rel_alt = n.subscribe("/mavros/global_position/rel_alt", 1, altReceiver );
	ros::ServiceClient client = n.serviceClient<mavros::SetMode>("/mavros/set_mode");
	pub_incoming_reply = n.advertise<std_msgs::String>("art_vrd/incoming_reply", 100)
	mavros::SetMode flight;
    ROS_INFO("Starting Flight Mode Changer.");
	
    bool success;
	int last_flightmode_data;
	int rc_taken = 1;
	int rc_transit_delay = 2000000;
	int channel_7_mid = 1500;
	// channel_7_off = 987 | channel_7_on = 2010
	
	ros::spin();
	
				

  return 0;
}


void rcinReceiver(const mavros::RCIn& rc_in_data){
	
	rc_in_channel_7 = rc_in_data.channels[6];
}

void altReceiver(const std_msgs::Float64& alt_msg){

  rel_alt = alt_msg.data;
}

void mainFMChanger(const std_msgs::String& main_fmc)
{
	std_msgs::String incoming_reply;
	
	if ( vData.data[0] == 's' && vData.data[1] == 'c' ){
	
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
		incoming_reply.data = "Reply Data Here";
		pub_incoming_reply.publish(incoming_reply);
		ROS_INFO_STREAM( "It's a vf command") ;
				
	}
	
	else{
		ROS_INFO_STREAM( "It's not a vf command") ;
	}	
	ROS_INFO_STREAM( "Voice Data : " << vData.data) ;
}

