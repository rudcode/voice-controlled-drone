#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/time.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h> 
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
using namespace cv;
 
int main(int argc , char *argv[])
{
	// ############ Start OpenCV Variable ############ 
	int nBytes, x;
	char video_data[100000];
	vector<uchar> buff;
	vector<int> param = vector<int>(2);     
	Mat image_capture;  
	param[0] = CV_IMWRITE_JPEG_QUALITY;			// set tipe encoding 
	param[1] = 50;						// set kualitas encoding
	// ############ End OpenCV Variable ############ 

	// ############  Start Create Socket  ############ 
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

	// ############  End Create Socket  ############ 


	// ############  Start Video Init ############   
	VideoCapture cap(0);			// Membuka Kamera
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);		// Set Lebar gambar
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 360);		// Set tinggi Gambar
	if(!cap.isOpened()){
		cout << "Error\n";
		return -1;
	}
	// ############  End Video Init ############  
	// Keep waiting for connection
	while(1){
		puts("Waiting for incoming connections...");
		c = sizeof(struct sockaddr_in);
		 
		client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c);
		if (client_sock < 0){
			perror("accept failed");
			return 1;
		}
		puts("Connection accepted");
		 
		// ############  Start Receiving Data ############ //
		while( (read_size = recv(client_sock , client_message , 2000 , 0)) > 0 ){
			
			//Send the message back to client
			if(client_message[0] == 'v' && client_message[1] == 'f'){
				cap >> image_capture;					// mengambil gambar dari kamera
				cvtColor(image_capture, image_capture, CV_8U);
				
				imencode(".jpeg", image_capture, buff, param);		// meng-encode gambar
			
				nBytes = buff.size();

				for (x = 0; x < nBytes; x++){				// mengubah data gambar dari tipe vector ke array
					video_data[x] = buff[x];
				}
			
				char byteBuffer[20];
				sprintf(byteBuffer, "*AI\n%d\n", nBytes);
				if(send(client_sock, byteBuffer, sizeof(byteBuffer), 0) < 0) {	// send start flag and size
					cout << "Send start flag and size failed " << endl;
					return 1;
				}			
				if(send(client_sock, video_data, nBytes, 0) < 0) {		// send encoded image to client
					cout << "Send image data failed" << endl;
					return 1;
				}
				if(send(client_sock, "\n", 1, 0) < 0) {				// end string
					cout << "Send end string failed" << endl;
					return 1;
				}
			
				cout << nBytes << " Bytes length" <<endl;
				memset(&video_data[0], 0, nBytes);				// men-clear buffer gambar
				//usleep(50000);
			}
			else if(client_message[0] == 's' && client_message[1] == 'c'){
				if(send(client_sock, "OK\n", sizeof("OK\n")-1, 0) < 0) {	// send ok
					cout << "Send ok failed" << endl;
					return 1;
				}
			}
			else if(client_message[0] == 'd' && client_message[1] == 's'){
				if(send(client_sock, "Drone Status\n", sizeof("Drone Status\n")-1, 0) < 0) {	// send Drone Status
					cout << "Send ok failed" << endl;
					return 1;
				}
			}
			else {
				send(client_sock, "ERROR\n", 6, 0);			// if client_message not vf (video fetch)
				cout << "ERROR" <<endl;
			}
		}
		
		// ############  End Receiving Data ############ //
		 
		if(read_size == 0){
			puts("Client disconnected");
			fflush(stdout);
		}
		else if(read_size <= -1){
			perror("recv failed");
		}
	}
	close(socket_desc);
     
    return 0;
}

