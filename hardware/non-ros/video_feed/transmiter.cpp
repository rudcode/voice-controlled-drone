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
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace std;
using namespace cv;
 
int main(int argc , char *argv[])
{
     
    // ############ Start OpenCV Variable ############ 
    int read_size;
    char server_reply[100000];
    Mat img_stream;
    // ############ End OpenCV Variable ############ 


     
    // ############  Start Create Socket  ############ 
    int sock;
    struct sockaddr_in server;
    char message[1000];
    sock = socket(AF_INET , SOCK_STREAM , 0);
    if (sock == -1)
    {
        printf("Could not create socket");
    }
    puts("Socket created");
     
    server.sin_addr.s_addr = inet_addr("127.0.0.1");
    server.sin_family = AF_INET;
    server.sin_port = htons(50005);
    // ############  End Create Socket  ############ 
 
 
 
    // ############  Start Connection to remote server  ############ 
    if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    {
        perror("connect failed. Error");
        return 1;
    }
     
    puts("Connected\n");
    // ############  End Connection to remote server   ############ 
    
	namedWindow("Server Streamer", WINDOW_NORMAL);
	
    //keep communicating with server
    while(1)
    {
		printf("Enter message : ");			//	Comment line 64 and 65 to have a continuous
		fgets(message,1000,stdin);			//	video feedback, and change line 69 to have 
											//  send(sock , "vf" , 2 , 0) function
        
        //Send request data
        if( send(sock , message , strlen(message) , 0) < 0){
            puts("Send failed");
            return 1;
        }
        
        //Receive a reply from the server
        read_size = recv(sock , server_reply , 100000 , 0);
        if( read_size < 0){
            puts("recv failed");
            break;
        }
        
        if(server_reply[0] == 'E' &&									// if server_relpy error, don't update opencv frame
			server_reply[1] == 'R' &&
			server_reply[2] == 'R' &&
			server_reply[3] == 'O' &&
			server_reply[4] == 'R'){
				
			puts("Server reply : ERROR");
		}
		
		else{
			cout << read_size << " Bytes Received" << endl;
				
			Mat img_buff(Size(640, 360), CV_8U, server_reply);			// deklarasi tempat untuk frame gambar
			img_stream = imdecode(img_buff, CV_LOAD_IMAGE_COLOR);		// men decode image data yang diterima
			if (!img_stream.empty()){
				imshow("Server Streamer", img_stream);					// menampilkan gambar
			}
		}
			
		if (waitKey(30) == 27){											// kill opencv window
			cout << "esc key is pressed by user" << endl;
			break; 
		}
        
        if(message[0] == 'x'){											// input 'x' to kill connection
			break;
		}
    }
     
    close(sock);
    return 0;
}
