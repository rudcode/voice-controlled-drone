#include "../include/art_vrd/movement_controller.h"

#define YES 1
#define NO 0
#define RC NO

int main(int argc, char **argv)
{	
	ros::init(argc, argv, "movement_controller");
	ros::NodeHandle n;
	ros::Subscriber vd_sub		= n.subscribe("art_vrd/voice_data", 10, mainMovementController);
	ros::Subscriber rc_in_sub 	= n.subscribe("/mavros/rc/in", 100, rcinReceiver);
	ros::Subscriber sub_rel_alt = n.subscribe("/mavros/global_position/rel_alt", 1, altReceiver );
	ros::Subscriber sub_pos 	= n.subscribe("/mavros/local_position/local", 100, positionReceiver );
	ros::Subscriber sub_state 	= n.subscribe("/mavros/state", 100, stateReceiver);
	client					 	= n.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");
	pub_rc_override				= n.advertise<mavros_msgs::OverrideRCIn>("/mavros/rc/override", 1, true);
	pub_quad_pos 				= n.advertise<geometry_msgs::PoseStamped>("mavros/setpoint_position/local", 1000);
	pub_incoming_reply 			= n.advertise<std_msgs::String>("art_vrd/incoming_reply", 100);
    ROS_INFO_STREAM("Starting Movement Controller.");
    
    known_command[0] = "naik";
	known_command[1] = "turun";
	known_command[2] = "maju";
	known_command[3] = "mundur";
	known_command[4] = "kiri";
	known_command[5] = "kanan";
	known_command[6] = "pulang ke pangkalan";
	known_command[7] = "mendarat di sini";
	known_command[8] = "lepas landas";
	
	#if RC == NO

	// ############# comment this if you have a rc #############  
	usleep(2000000);
	
	rc_override_data.channels[6] = 1000;
	pub_rc_override.publish(rc_override_data);
	
	usleep(2000000);
	
	rc_override_data.channels[6] = 2000;
	pub_rc_override.publish(rc_override_data);
	
	// ############# comment this if you have a rc #############  

	#endif
	
	
	ros::spin();
	return 0;
}


void mainMovementController(const std_msgs::String& vData)
{
	int x;
	int distance_vdata = 0;
	
	if ( vData.data[0] == 's' && vData.data[1] == 'c' ){
		
		for(x=0; x < command_list;x++){
			command_search = strstr(vData.data.c_str(), known_command[x]);		
			if (command_search != NULL){
				command_detected = x;
				break;						// exit loop if found a known_command
			}
			
			else{
				command_detected = -1;		// set to invalid if known_command not found
			}
		}
		
		if(rc_in_channel_7 > channel_7_mid){
			if(command_detected == 0 && flight_mode == "GUIDED" && arm_state){
				
				// naik command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('z',pos_z+distance_vdata);
			}
			
			else if(command_detected == 1 && flight_mode == "GUIDED" && arm_state){
				
				// turun command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('z',pos_z-distance_vdata);
		
			}
			
			else if(command_detected == 2 && flight_mode == "GUIDED" && arm_state){
				
				// maju command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('y',pos_y+distance_vdata);
			}
			
			else if(command_detected == 3 && flight_mode == "GUIDED" && arm_state){
				
				// mundur command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('y',pos_y-distance_vdata);

			}
			
			else if(command_detected == 4 && flight_mode == "GUIDED" && arm_state){
				
				// kiri command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('x',pos_x-distance_vdata);

			}
			
			else if(command_detected == 5 && flight_mode == "GUIDED" && arm_state){
				
				// kanan command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('x',pos_x+distance_vdata);
				
			}
			
			else if(command_detected == 6 ){
				
				if (flight_mode == "RTL"){
					command_detected = -2;		// send error reply to android
				}
				else {
					if(!changeFlightMode("RTL")){
						command_detected = -2;		// send error reply to android
					}
				}
			}
			
			else if(command_detected == 7 ){
				
				if (flight_mode == "LAND"){
					command_detected = -2;		// send error reply to android
				}
				else {
					if(!changeFlightMode("LAND")){
						command_detected = -2;		// send error reply to android
					}
				}
			}
			
			else if(command_detected == 8 && !arm_state){				
				
				changeFlightMode("STABILIZE");
				usleep(3000000);
				
				system("rosrun mavros mavsafety arm");
				ROS_INFO_STREAM( "Wait 5 seconds" ) ;
				usleep(5000000);
				
				for(int i=0; i < 8; i++) rc_override_data.channels[i] = 65535;
				rc_override_data.channels[2] = 1185; 
				pub_rc_override.publish(rc_override_data);
				usleep(5000000);
				changeFlightMode("AUTO");
				usleep(13000000);
				changeFlightMode("GUIDED");
				
				for(int i=0; i < 8; i++) rc_override_data.channels[i] = 65535;
				rc_override_data.channels[2] = 0; 
				pub_rc_override.publish(rc_override_data);

							
			}
			
			else {
				command_detected = -1;
			}
			
			if(command_detected == -3 ){
				sendReply("sc:drone_movement_is_zero\n");
			}
			
			else if(command_detected == -2 ){
				sendReply("sc:can't_change_fm\n");
			}
			
			else if(command_detected != -1 ){
				sendReply("sc:ok\n");
			}
						
			else{
				sendReply("sc:invalid_or_error\n");
			}
		}
		
		else{
			sendReply("sc:rc_takeover\n");
			ROS_ERROR_STREAM( "[MC] RC Take Over!" ) ;
		}
	}
}

void rcinReceiver(const mavros_msgs::RCIn& rc_in_data){
	
	rc_in_channel_7 = rc_in_data.channels[6];
}

void altReceiver(const std_msgs::Float64& alt_msg){

	rel_alt = alt_msg.data;
	pos_z	= rel_alt;
}

void positionReceiver(const geometry_msgs::PoseStamped& local_recv){
	
	pos_x = local_recv.pose.position.x;
	pos_y = local_recv.pose.position.y;
}

void stateReceiver(const mavros_msgs::State& state_recv){
	arm_state = state_recv.armed;
	flight_mode = state_recv.mode;
}

void moveDrone(char axis, int location){
	int movement_distance;
	if (axis == 'x'){
			movement_distance = abs(pos_x-location);
		}
		
	else if (axis == 'y'){
			movement_distance = abs(pos_y-location);
		}
		
	else if (axis == 'z'){
			movement_distance = abs(pos_z-location);
		}
		
	else {
			movement_distance = DISTANCELIMIT+1;			// always never sent to topic if axis input invalid
		}
	
	if (movement_distance < DISTANCELIMIT && movement_distance != 0){
		quad_pos.header.stamp = ros::Time::now();
		quad_pos.header.frame_id = "1";
		
		if (axis == 'x'){
			quad_pos.pose.position.x = location;
			quad_pos.pose.position.y = pos_y;
			quad_pos.pose.position.z = pos_z;
		}
		
		else if (axis == 'y'){
			quad_pos.pose.position.x = pos_x;
			quad_pos.pose.position.y = location;
			quad_pos.pose.position.z = pos_z;
		}
		
		else if (axis == 'z'){
			quad_pos.pose.position.x = pos_x;
			quad_pos.pose.position.y = pos_y;
			quad_pos.pose.position.z = location;
		}
		
		else {
			command_detected = -1;
		}
			 	
		pub_quad_pos.publish(quad_pos);
	}
	
	else if(movement_distance == 0){
		command_detected = -3;
	}
	
	else {
		command_detected = -1;
	}
	
}

void sendReply(const char* reply_message){
	
	incoming_reply.data = reply_message;
	pub_incoming_reply.publish(incoming_reply);
}

int changeFlightMode(const char* mode){
	bool success;
	flight.request.base_mode = 0;				//Set to 0 to use custom_mode
	flight.request.custom_mode = mode;			//Set to '' to use base_mode
	success = client.call(flight);
	
	if(success){
		ROS_INFO_STREAM( "[MC] Flight Mode changed to "<< flight.request.custom_mode ) ;
	} 
	else {
		ROS_ERROR_STREAM( "[MC] Failed to change." ) ;
	}
	
	return success;
}
