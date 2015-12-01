#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{	const char* command_detected;
	const char* known_command[8];
	int meter;
	
	known_command[0] = "naik";
	known_command[1] = "woynaik 102 123 meter sekarang";
	command_detected = strstr(known_command[1],known_command[0]);
	if(command_detected == NULL){
		return -1;
	}
	
	meter = strtol (command_detected+strlen(known_command[0]),NULL,10);
	
	printf("%s\n",known_command[1]);
	printf("%s\n",command_detected);
	printf ("The detected distance is: %d.\n", meter);
	return 0;
}
