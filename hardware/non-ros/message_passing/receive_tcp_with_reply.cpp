#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
 
int main(int argc , char *argv[])
{
	
    int socket_desc , client_sock , c , read_size;
    struct sockaddr_in server , client;
    char client_message[2000];
    int port_number = 50005;
     
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
    
	while(1){
		puts("Waiting for incoming connections...");
		c = sizeof(struct sockaddr_in);
		 
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0){
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");
		 

		while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ){
			client_message[read_size] = '\0';
			printf("Character Received : %d\n",read_size);
			printf("%s\n",client_message);
					
			//Send the message back to client
			if(client_message[0] == 's')
			write(client_sock , "OK\0" , 4);
			
			else if (client_message[0] == 'd')
			write(client_sock , "Status Sent\0" , 13);
			
			else
			write(client_sock , client_message , strlen(client_message));
			
		}
		 
		if(read_size == 0){
			puts("Client disconnected");
			fflush(stdout);
		}
		else if(read_size == -1){
			perror("recv failed");
		}
	}
	close(socket_desc);
     
    return 0;
}
