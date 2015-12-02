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
#define DISTANCELIMIT 10

using namespace std;

void positionReceiver(const geometry_msgs::PoseStamped& local_recv);
void mainMovementController(const std_msgs::String& main_fmc);
void altReceiver(const std_msgs::Float64& alt_msg);
// ################ new mavros msg lib is in mavros_msgs ################ 
//void rcinReceiver(const mavros::RCIn& rc_in_data);
//void stateReceiver(const mavros::State& state_recv);
void stateReceiver(const mavros_msgs::State& state_recv);
void rcinReceiver(const mavros_msgs::RCIn& rc_in_data);
// ################ new mavros msg lib is in mavros_msgs ################ 

int rc_in_channel_7 = 0;
float rel_alt = 0;
const char* known_command[COMMAND_LIST];
int command_list = COMMAND_LIST;
int pos_x = 0;
int pos_y = 0;
int pos_z = 0;
string flight_mode = "AUTO";
bool success;
int rc_taken = 1;
int rc_transit_delay = 2000000;
int channel_7_mid = 1500;
// channel_7_off = 987 | channel_7_on = 2010
	
ros::Publisher pub_incoming_reply;
ros::Publisher pub_quad_pos;	
ros::ServiceClient client;
// ################ new mavros msg lib is in mavros_msgs ################ 
//mavros::SetMode flight;
mavros_msgs::SetMode flight;
// ################ new mavros msg lib is in mavros_msgs ################ 
geometry_msgs::PoseStamped quad_pos;


int main(int argc, char **argv)
{	
	ros::init(argc, argv, "movement_controller");
	ros::NodeHandle n;
	ros::Subscriber vd_sub		= n.subscribe("art_vrd/voice_data", 10, mainMovementController);
	ros::Subscriber rc_in_sub 	= n.subscribe("/mavros/rc/in", 100, rcinReceiver);
	ros::Subscriber sub_rel_alt = n.subscribe("/mavros/global_position/rel_alt", 1, altReceiver );
	ros::Subscriber sub_pos 	= n.subscribe("/mavros/local_position/local", 100, positionReceiver );
	ros::Subscriber sub_state 	= n.subscribe("/mavros/state", 100, stateReceiver);
	// ################ new mavros msg lib is in mavros_msgs ################ 
	//client					= n.serviceClient<mavros::SetMode>("/mavros/set_mode");
	client					 	= n.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");
	// ################ new mavros msg lib is in mavros_msgs ################ 
	pub_quad_pos 				= n.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 1000);
	pub_incoming_reply 			= n.advertise<std_msgs::String>("art_vrd/incoming_reply", 100);
    ROS_INFO("Starting Movement Controller.");
    
    known_command[0] = "naik";
	known_command[1] = "turun";
	known_command[2] = "maju";
	known_command[3] = "mundur";
	known_command[4] = "kiri";
	known_command[5] = "kanan";
	known_command[6] = "pulang ke pangkalan";
	known_command[7] = "mendarat di sini";
	known_command[8] = "lepas landas";
    
    bool success;
	int last_flightmode_data;
	int rc_taken = 1;
	int rc_transit_delay = 2000000;
	int channel_7_mid = 1500;
	// channel_7_off = 987 | channel_7_on = 2010
	
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

void positionReceiver(const geometry_msgs::PoseStamped& local_recv){
	
	pos_x = local_recv.pose.position.x;
	pos_y = local_recv.pose.position.y;
	pos_z = local_recv.pose.position.z;
}

// ################ new mavros msg lib is in mavros_msgs ################ 
// stateReceiver(const mavros::State& state_recv)
void stateReceiver(const mavros_msgs::State& state_recv){
	
	flight_mode = state_recv.mode;
}

void mainMovementController(const std_msgs::String& vData)
{
	std_msgs::String incoming_reply;
	int x;
	const char* command_search;
	int command_detected;
	int distance;
	
	if ( vData.data[0] == 's' && vData.data[1] == 'c' ){
		
		for(x=0; x < command_list;x++){
			command_search = strstr(vData.data.c_str(), known_command[x]);		
			if (command_search != NULL){
				command_detected = x;
				break;
			}
			
			else{
				command_detected = -1;
			}
		}
		
		if(rc_in_channel_7 > channel_7_mid){
			if(command_detected == 0 ){
				
				// naik command
				distance = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				if (distance < DISTANCELIMIT){
					quad_pos.header.stamp = ros::Time::now();
					quad_pos.header.frame_id = "1";
					quad_pos.pose.position.z = pos_z+distance; 	
					pub_quad_pos.publish(quad_pos);
				}
				
				else {
					command_detected = -1;
				}
				
			}
			
			else if(command_detected == 1 ){
				
				// turun command
				distance = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				if (distance < DISTANCELIMIT){
					quad_pos.header.stamp = ros::Time::now();
					quad_pos.header.frame_id = "1";
					quad_pos.pose.position.z = pos_z-distance; 	
					pub_quad_pos.publish(quad_pos);
				}
				else {
					command_detected = -1;
				}
				
			}
			
			else if(command_detected == 2 ){
				
				// maju command
				distance = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				if (distance < DISTANCELIMIT){
					quad_pos.header.stamp = ros::Time::now();
					quad_pos.header.frame_id = "1";
					quad_pos.pose.position.y = pos_y-distance; 	
					pub_quad_pos.publish(quad_pos);
				}
				else {
					command_detected = -1;
				}
				
			}
			
			else if(command_detected == 3 ){
				
				// mundur command
				distance = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				if (distance < DISTANCELIMIT){
					quad_pos.header.stamp = ros::Time::now();
					quad_pos.header.frame_id = "1";
					quad_pos.pose.position.y = pos_y+distance; 	
					pub_quad_pos.publish(quad_pos);
				}
				else {
					command_detected = -1;
				}
			}
			
			else if(command_detected == 4 ){
				
				// kiri command
				distance = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				if (distance < DISTANCELIMIT){
					quad_pos.header.stamp = ros::Time::now();
					quad_pos.header.frame_id = "1";
					quad_pos.pose.position.x = pos_x-distance; 	
					pub_quad_pos.publish(quad_pos);
				}
				else {
					command_detected = -1;
				}
			}
			
			else if(command_detected == 5 ){
				
				// kanan command
				distance = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				if (distance < DISTANCELIMIT){
					quad_pos.header.stamp = ros::Time::now();
					quad_pos.header.frame_id = "1";
					quad_pos.pose.position.x = pos_x+distance; 	
					pub_quad_pos.publish(quad_pos);
				}
				else {
					command_detected = -1;
				}
				
			}
			
			else if(command_detected == 6 ){
				
				flight.request.base_mode = 0;				//Set to 0 to use custom_mode
				flight.request.custom_mode = "RTL";		//Set to '' to use base_mode
				success = client.call(flight);

				// Check for success and print out info.
				if(success){
					ROS_INFO_STREAM( "Flight Mode changed to "<< flight.request.custom_mode ) ;
				} 
				else {
					ROS_ERROR_STREAM( "Failed to changed." ) ;
				}
			}
			
			else if(command_detected == 7 ){
				
				flight.request.base_mode = 0;				//Set to 0 to use custom_mode
				flight.request.custom_mode = "LAND";		//Set to '' to use base_mode
				success = client.call(flight);

				// Check for success and print out info.
				if(success){
					ROS_INFO_STREAM( "Flight Mode changed to "<< flight.request.custom_mode ) ;
				} 
				else {
					ROS_ERROR_STREAM( "Failed to changed." ) ;
				}
			}
			
			else if(command_detected == 8 ){				
				
				flight.request.base_mode = 0;				//Set to 0 to use custom_mode
				flight.request.custom_mode = "AUTO";		//Set to '' to use base_mode
				success = client.call(flight);

				// Check for success and print out info.
				if(success){
					ROS_INFO_STREAM( "Flight Mode changed to "<< flight.request.custom_mode ) ;
				} 
				else {
					ROS_ERROR_STREAM( "Failed to changed." ) ;
				}
			}
			
			if(command_detected != -1 ){
				incoming_reply.data = "sc:ok\n";
				pub_incoming_reply.publish(incoming_reply);}
			
			else{
				incoming_reply.data = "sc:invalid\n";
				pub_incoming_reply.publish(incoming_reply);
			}
		}
		
		else{
			incoming_reply.data = "sc:rc_takeover\n";
			pub_incoming_reply.publish(incoming_reply);
			ROS_ERROR_STREAM( "RC Take Over!" ) ;
		}
		ROS_INFO_STREAM( "It's a sc command") ;
		
	}
	
	else{
		ROS_INFO_STREAM( "It's not a sc command") ;
	}	
	ROS_INFO_STREAM( "Voice Data : " << vData.data) ;
}
