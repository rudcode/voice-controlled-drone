#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
 
int main(int argc , char *argv[])
{
	char buffer[100000];
	Mat img_stream;
	
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
		  namedWindow("Server Streamer", WINDOW_NORMAL); 

		while( (read_size = recv(client_sock , client_message , 100000 , 0)) > 0 ){
			cout << read_size << " Bytes Received" << endl;
			
			Mat img_buff(Size(640, 360), CV_8U, client_message);				// deklarasi tempat untuk frame gambar
			img_stream = imdecode(img_buff, CV_LOAD_IMAGE_COLOR);		// men decode image data yang diterima
			if (!img_stream.empty()){
				imshow("Server Streamer", img_stream);					// menampilkan gambar
			}
			
			if (waitKey(30) == 27){
				cout << "esc key is pressed by user" << endl;
				break; 
			}
				
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
