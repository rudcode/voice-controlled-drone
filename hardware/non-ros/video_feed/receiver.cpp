#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
// ./server_udp 7891

using namespace std;
using namespace cv;

	int udpSocket, nBytes, server_port;
	char buffer[100000];
	struct sockaddr_in serverAddr, clientAddr;
	struct sockaddr_storage serverStorage;
	socklen_t addr_size, client_addr_size;
	Mat img_stream;
	
int main(int argc, char** argv){
	
	/* Inisialisasi Socket Server */
	server_port = atoi(argv[1]);					// Port Server
	udpSocket = socket(AF_INET, SOCK_DGRAM, 0);		// membuka koneksi UDP
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(server_port);		// membuka port
	serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");	
	memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  
	bind(udpSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
	addr_size = sizeof serverStorage;
	/* Inisialisasi Socket Server */
	
	namedWindow("Server Streamer", WINDOW_NORMAL);
	
  
	while(1){
    
		nBytes = recvfrom(udpSocket,buffer,100000,0,(struct sockaddr *)&serverStorage, &addr_size); // Menerima Data dari client
		
		cout << nBytes << " Bytes Received" << endl;
		Mat img_buff(Size(640, 360), CV_8U, buffer);				// deklarasi tempat untuk frame gambar
		img_stream = imdecode(img_buff, CV_LOAD_IMAGE_COLOR);		// men decode image data yang diterima
		if (!img_stream.empty()){
			imshow("Server Streamer", img_stream);					// menampilkan gambar
		}
		
		if (waitKey(30) == 27){
			cout << "esc key is pressed by user" << endl;
			break; 
		}

	}

  return 0;
}
