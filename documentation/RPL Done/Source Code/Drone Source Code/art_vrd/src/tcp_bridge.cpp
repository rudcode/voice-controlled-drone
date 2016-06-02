#include "ros/ros.h"
#include "std_msgs/String.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>


std::string incoming_reply;
int incoming_reply_size;
void incomingReply(const std_msgs::String& data_reply);

int main(int argc, char **argv){

	
	std_msgs::String voice_cmd;
	const char * message_reply;
	
	ros::init(argc, argv, "tcp_bridge");
	ros::NodeHandle n;
	ros::Publisher pub_voice = n.advertise<std_msgs::String>("art_vrd/voice_data", 1024);
	ros::Subscriber sub_incoming_reply = n.subscribe("art_vrd/incoming_reply", 1024, incomingReply);
	ROS_INFO("Starting TCP Bridge.");
	// ################# Intializing Socket	#################  
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[512];
    int port_number = 50001;
    // add argument to tcp port so that it can be changed on the fly via .launch file
     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
		ROS_ERROR_STREAM("[TB] Could not create socket");
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
		ROS_ERROR_STREAM("[TB] bind failed. Error: " << strerror(errno));
        return 1;
    }   
    listen(socket_desc , 1);
    c = sizeof(struct sockaddr_in);

	// recv timeout configuration
    struct timeval tv;
	tv.tv_sec = 30;  /* 30 Secs Timeout */
	tv.tv_usec = 0;  // Not init'ing this can cause strange errors
	// ################# Intializing Socket	#################  
		 
	while(ros::ok()){
		ROS_INFO_STREAM("[TB] Waiting for incoming connections...");
		
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0){
			ROS_ERROR_STREAM("[TB] accept failed. Error: " << strerror(errno));
			return 1;
		}
		ROS_INFO_STREAM("[TB] Connection accepted");
		setsockopt(client_sock, SOL_SOCKET, SO_RCVTIMEO, &tv,sizeof(tv));
    	while(ros::ok()){
			errno = 0;	// reset errno
			read_size = recv(client_sock , client_message , 512 , 0);
			if (errno != EAGAIN){
				client_message[read_size] = '\0';
				ROS_INFO_STREAM( "[TB] Voice Data : " << client_message);
				voice_cmd.data = client_message;
				pub_voice.publish(voice_cmd);			
				incoming_reply = "NULL";
				
				ROS_INFO_STREAM("[TB] Waiting reply from art_vrd nodes");
				while(read_size != -1 && read_size != 0 && incoming_reply == "NULL" && ros::ok()){
					ros::spinOnce();			
				}
				ROS_INFO_STREAM("[TB] Reply accepted from art_vrd nodes");
				
				message_reply = incoming_reply.c_str();
				incoming_reply_size = incoming_reply.size();
				
				// ################## Command Reply Header Data ########################
				int32_t byteBuffer = htonl(incoming_reply_size);
				if(send(client_sock, &byteBuffer, sizeof(byteBuffer), 0) < 0) {
					ROS_ERROR_STREAM("[TB] send failed. Error: " << strerror(errno));
					break;
				}	
				// ################## Command Reply Header Data ########################
				
				if( send(client_sock , message_reply , incoming_reply_size , 0) < 0){
					ROS_ERROR_STREAM("[TB] send failed. Error: " << strerror(errno));
					break;
				}
				
				if(read_size == 0){
					ROS_WARN_STREAM("[TB] Client disconnected");
					break;
				}
				else if(read_size == -1){
					ROS_ERROR_STREAM("[TB] recv failed. Error: " << strerror(errno));
					break;
				}
				else if(incoming_reply == "NULL"){
					ROS_WARN_STREAM("[TB] Client disconnected with NULL message");;
					break;
				}
			}
			
			else if (errno == EAGAIN){
				ROS_ERROR_STREAM("[TB] recv timed out. Error: " << strerror(errno));
				break;
			}
			
			ros::spinOnce();
		}
		close(client_sock);
        sleep(1);
	}	
	close(socket_desc);
     
    return 0;
}

void incomingReply(const std_msgs::String& data_reply)
{
	incoming_reply = data_reply.data;
}
