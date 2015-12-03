#include "../include/art_vrd/movement_controller.h"

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
	//pub_rc_override			= n.advertise<mavros::OverrideRCIn>("/mavros/rc/override", 1, true);
	client					 	= n.serviceClient<mavros_msgs::SetMode>("/mavros/set_mode");
	pub_rc_override				= n.advertise<mavros_msgs::OverrideRCIn>("/mavros/rc/override", 1, true);
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
		
		if(rc_in_channel_7 > channel_7_mid | 1){
			if(command_detected == 0 ){
				
				// naik command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('z',pos_z+distance_vdata);
			}
			
			else if(command_detected == 1 ){
				
				// turun command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('z',pos_z-distance_vdata);
		
			}
			
			else if(command_detected == 2 ){
				
				// maju command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('y',pos_y-distance_vdata);
			}
			
			else if(command_detected == 3 ){
				
				// mundur command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('y',pos_y+distance_vdata);

			}
			
			else if(command_detected == 4 ){
				
				// kiri command
				distance_vdata = strtol (command_search+strlen(known_command[command_detected]),NULL,10);
				moveDrone('x',pos_x-distance_vdata);

			}
			
			else if(command_detected == 5 ){
				
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
			
			else if(command_detected == 8 ){				
				
				changeFlightMode("LOITER");
				
			}
			
			if(command_detected == -2 ){
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
			ROS_ERROR_STREAM( "RC Take Over!" ) ;
		}
		
		ROS_INFO_STREAM( "It's a sc command") ;
		
	}
	
	else{
		ROS_INFO_STREAM( "It's not a sc command") ;
	}	
	
	ROS_INFO_STREAM( "Voice Data : " << vData.data) ;
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
			movement_distance = DISTANCELIMIT+1;			// always never sent to topic if axis input invaalid
		}
	
	if (movement_distance < DISTANCELIMIT){
		quad_pos.header.stamp = ros::Time::now();
		quad_pos.header.frame_id = "1";
		
		if (axis == 'x'){
			quad_pos.pose.position.x = location;
		}
		
		else if (axis == 'y'){
			quad_pos.pose.position.y = location;
		}
		
		else if (axis == 'z'){
			quad_pos.pose.position.z = location;
		}
		
		else {
			command_detected = -1;
		}
			 	
		pub_quad_pos.publish(quad_pos);
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
	flight.request.custom_mode = mode;		//Set to '' to use base_mode
	success = client.call(flight);
	
	if(success){
		ROS_INFO_STREAM( "Flight Mode changed to "<< flight.request.custom_mode ) ;
	} 
	else {
		ROS_ERROR_STREAM( "Failed to change." ) ;
	}
	
	return success;
}
