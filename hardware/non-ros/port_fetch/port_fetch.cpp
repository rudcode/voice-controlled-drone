#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>    
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#define SOURCE_START_IDENTIFIER "0.tcp.ngrok.io"
#define SOURCE_START_IDENTIFIER2 "<html>" 		//this is the name of the identifier that the 
#define	MAX_BUFFER 3000
int getForwardingPort(char *datain);
 
int main(int argc , char *argv[])
{
	if(argc != 4){
		printf("Usage %s : hostname file_location port\n",argv[0]);
		return(0);
	}
	
    int socket_desc;
    int connect_socket;
    struct sockaddr_in server;
    char server_reply[MAX_BUFFER];
    char message[MAX_BUFFER];
    int port_number;    
    int ngrok_port_number = 0;    
    int last_ngrok_port_number = 1;    
    int connect_error_flag = 1;     
    int ngrok_disconnect_flag = 1;     
    int create_socket_flag = 1;     
         
    port_number = atoi(argv[3]);
 
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons( port_number );
 
	while(1){
		socket_desc = socket(AF_INET , SOCK_STREAM , 0);
		if (socket_desc == -1){

			if (create_socket_flag == 1	){
			
				puts("could not create socket");
				create_socket_flag = 0;
			}
			
			close(socket_desc);
			usleep(1000000);
			continue;
		}
		
		create_socket_flag = 1;
		//Connect to remote server
		if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
		{
			if (connect_error_flag == 1){
				
				puts("connect error");
				connect_error_flag = 0;
			}
		    close(socket_desc);
			usleep(1000000);
			continue;
		}
		connect_error_flag = 1;
		
		sprintf(message,"GET %s HTTP/1.1\r\nHost: %s\r\n\r\n",argv[2],argv[1]);
		
		//Send some data
		if( send(socket_desc , message , strlen(message) , 0) < 0)
		{
			puts("Send failed");
			return 1;
		}
		//Receive a reply from the server
		if( recv(socket_desc, server_reply , MAX_BUFFER , 0) < 0)
		{
			puts("recv failed");
		}
		
		if(getForwardingPort(server_reply) > 9999){
			ngrok_port_number = getForwardingPort(server_reply);
			if (ngrok_port_number != last_ngrok_port_number){
				printf("new ngrok port is %d\n", ngrok_port_number);
				last_ngrok_port_number = ngrok_port_number;
			}
		}
		
		else{
		
			printf("ngrok not connected\n");
		}
		
		usleep(1000000);
		close(socket_desc);
	}
     
    return 0;
}

//sprintf(message,"POST %s HTTP/1.1\nHost: %s\nContent-length: 10\nContent-Type: application/x-www-form-urlencoded\nConnection: close\n\nport=%s\n",argv[2],argv[1],port_data_http);

int getForwardingPort(char *datain){

	char *source;
	char forward_port[8];
	int i;
    int ngrok_port;
    source = strstr(datain,SOURCE_START_IDENTIFIER);

	if(source == NULL)
	{
		source = strstr(datain,SOURCE_START_IDENTIFIER2);		
	}	

	//printf("%s\n",source);
	i = 0;
	while (i < 5 ){
		
		forward_port[i] = source[i+15];
		i++;
	}
	ngrok_port = atoi(forward_port);
	
	return ngrok_port;
	
	
}
