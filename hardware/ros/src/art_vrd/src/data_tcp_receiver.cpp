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


int main(int argc, char **argv){
	
	ros::init(argc, argv, "data_tcp_receiver");
	ros::NodeHandle n;
	ros::Publisher pub_string = n.advertise<std_msgs::String>("art_vrd/voice_data", 1024);
	std_msgs::String voice_cmd;
	
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[512];
    char message_ok[] = "OK";
    int port_number = 8889;
     
    socket_desc = socket(AF_INET , SOCK_STREAM , 0);
    if (socket_desc == -1){
        printf("Could not create socket");
    }
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port_number);
    if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0){
		perror("bind failed. Error");
        return 1;
    }   
    listen(socket_desc , 3);
    c = sizeof(struct sockaddr_in);
		 
	while(ros::ok()){
		puts("Waiting for incoming connections...");
		
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0){
			
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");
		 

		while(ros::ok()){
			read_size = recv(client_sock , client_message , 512 , 0);
			
			if(read_size == 0){
				puts("Client disconnected");
				break;
			}
			else if(read_size == -1){
				perror("recv failed");
				break;
			}
			
			// Always Send Reply
			// send(client_sock , message_ok , strlen(message_ok) , 0);
			// Always Send Reply
			
			client_message[read_size] = '\0';
			voice_cmd.data = client_message;
			pub_string.publish(voice_cmd);
			ros::spinOnce();
		}
		close(client_sock);
        sleep(1);
	}
	close(socket_desc);
     
    return 0;
}

