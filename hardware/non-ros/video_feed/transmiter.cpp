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
	// Declare Variable 
    int sock,nBytes,x;
    struct sockaddr_in server;
    char message[100000] , server_reply[2000];
    vector<uchar> buff;
    vector<int> param = vector<int>(2); 
    
    
    // declare image sent configuration value
    param[0] = CV_IMWRITE_JPEG_QUALITY;			// set tipe encoding 
	param[1] =90;					// set kualitas encoding
    
    //Create socket
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(50005);
 
    //Connect to remote server
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
    
    // ######################### Start video transmit ########################### 
    VideoCapture cap(0);			// Membuka Kamera
	cap.set(CV_CAP_PROP_FRAME_WIDTH, 640);		// Set Lebar gambar
	cap.set(CV_CAP_PROP_FRAME_HEIGHT, 360);		// Set tinggi Gambar


	if(!cap.isOpened())
	{
		cout << "Error\n";
		return -1;
	}

	Mat image_capture;

	while(1){
		cap >> image_capture;				// mengambil gambar dari kamera
		
		cvtColor(image_capture, image_capture, CV_8U);
			
		imencode(".jpeg",image_capture, buff,param);	// meng-encode gambar
			  
		for (x = 0; x < buff.size(); x++){				// merubah data gambar dari tipe vector ke array
			message[x] = buff[x];
		}
		
		nBytes = buff.size();
		
		if( send(sock , message , nBytes , 0) < 0)
        {
            puts("Send failed");
            return 1;
        }
	
		cout<<buff.size()<<" Bytes Sent"<<endl;
		memset(&message[0], 0, nBytes);			// men-clear buffer gambar
	}
     
    close(sock);
    return 0;
}
