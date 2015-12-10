#include "ros/ros.h"
#include "geometry_msgs/Vector3Stamped.h"
#include "sensor_msgs/Temperature.h"
#include "sensor_msgs/FluidPressure.h"
#include "std_msgs/Float64.h"
#include "std_msgs/String.h"
#include "mavros_msgs/State.h"
#include "mavros_msgs/BatteryStatus.h"
#include <string>
#include <string.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>


using namespace std;


bool arm_state = 0;
string flight_mode = "AUTO";
float rel_alt = 0;
float compass = 0;
float vel_x = 0;
float vel_y = 0;
float vel_z = 0;
float temperature = 0;
float pressure = 0;
float battery = 0;


void debugging(string drone_status_debug);
void altReceiver(const std_msgs::Float64& alt_recv);
void compassReceiver(const std_msgs::Float64& compass_recv);
void velocityReceiver(const geometry_msgs::Vector3Stamped& velocity_recv);
void temperatureReceiver(const sensor_msgs::Temperature& temperature_recv);
void pressureReceiver(const sensor_msgs::FluidPressure& pressure_recv);
void stateReceiver(const mavros_msgs::State& state_recv);
void batteryReceiver(const mavros_msgs::BatteryStatus& battery_recv);
void mainStatus(const std_msgs::String& vData);
ros::Publisher pub_incoming_reply;

int main(int argc, char **argv)
{

	ros::init(argc, argv, "drone_status");
	ros::NodeHandle status;
	ros::Subscriber sub_state	 	= status.subscribe("mavros/state", 100, stateReceiver);
	ros::Subscriber sub_rel_alt 	= status.subscribe("/mavros/global_position/rel_alt", 1, altReceiver );
	ros::Subscriber sub_compass		= status.subscribe("/mavros/global_position/compass_hdg", 10, compassReceiver );
	ros::Subscriber sub_vel 		= status.subscribe("/mavros/global_position/gp_vel", 10, velocityReceiver );
	ros::Subscriber sub_temperature = status.subscribe("/mavros/imu/temperature", 10, temperatureReceiver );
	ros::Subscriber sub_pressure 	= status.subscribe("/mavros/imu/atm_pressure", 10, pressureReceiver );
	ros::Subscriber sub_battery 	= status.subscribe("/mavros/battery", 10, batteryReceiver );
	ros::Subscriber sub_vd 			= status.subscribe("art_vrd/voice_data", 10, mainStatus);
	pub_incoming_reply 				= status.advertise<std_msgs::String>("art_vrd/incoming_reply", 100);
	ros::spin();
  
  return 0;
}

void stateReceiver(const mavros_msgs::State& state_recv){
	
	flight_mode = state_recv.mode;
	arm_state = state_recv.armed;
}

void altReceiver(const std_msgs::Float64& alt_recv){
	
	rel_alt = alt_recv.data;
}

void compassReceiver(const std_msgs::Float64& compass_recv){
	
	compass = compass_recv.data;
}

void velocityReceiver(const geometry_msgs::Vector3Stamped& velocity_recv){
	
	vel_x = velocity_recv.vector.x;
	vel_y = velocity_recv.vector.y;
	vel_z = velocity_recv.vector.z;
	
}

void temperatureReceiver(const sensor_msgs::Temperature& temperature_recv){
	
	temperature = temperature_recv.temperature;
	
}

void pressureReceiver(const sensor_msgs::FluidPressure& pressure_recv){
	
	pressure = pressure_recv.fluid_pressure;
}

void batteryReceiver(const mavros_msgs::BatteryStatus& battery_recv){
	
	battery = battery_recv.voltage;
	
}


void mainStatus(const std_msgs::String& vData){
	string s_arm_state	 = static_cast<ostringstream*>( &(ostringstream() << arm_state) )->str();
	string s_rel_alt	 = static_cast<ostringstream*>( &(ostringstream() << rel_alt) )->str();
	string s_compass	 = static_cast<ostringstream*>( &(ostringstream() << compass) )->str();
	string s_vel_x		 = static_cast<ostringstream*>( &(ostringstream() << vel_x) )->str();
	string s_vel_y		 = static_cast<ostringstream*>( &(ostringstream() << vel_y) )->str();
	string s_vel_z		 = static_cast<ostringstream*>( &(ostringstream() << vel_z) )->str();
	string s_temperature = static_cast<ostringstream*>( &(ostringstream() << temperature) )->str();
	string s_pressure	 = static_cast<ostringstream*>( &(ostringstream() << pressure) )->str();
	string s_battery	 = static_cast<ostringstream*>( &(ostringstream() << battery) )->str();
	
	std_msgs::String incoming_reply;
	if ( vData.data[0] == 'd' && vData.data[1] == 's' ){
		
		incoming_reply.data = "ds:"+s_arm_state+";"+flight_mode+";"+s_rel_alt+";"+s_compass+";"+s_vel_x+";"+s_vel_y+";"+s_vel_z+";"+s_temperature+";"+s_pressure+";"+s_battery+";\n";
		//debugging(incoming_reply.data);
		pub_incoming_reply.publish(incoming_reply);
	}	
}

void debugging(string drone_status_debug){
	
		//Debugging Purpose
		cout << "\n\nDrone Status String" << endl;
		cout << drone_status_debug << endl;
		cout << endl;
		
		cout << "== Drone Status ==\n" << endl;	
			
		cout << "Arm Mode\t= " << arm_state << endl;	
		cout << "Flight Mode\t= " << flight_mode << endl;
		cout << "Altitude\t= " << rel_alt << endl;	
		cout << "Compass\t\t= " << compass << endl;	
		cout << "Velocity X\t= " << vel_x << endl;	
		cout << "Velocity Y\t= " << vel_y << endl;	
		cout << "Velocity Z\t= " << vel_z << endl;	
		cout << "Temperature\t= " << temperature << endl;	
		cout << "Pressure\t= " << pressure << endl;	
		cout << "Battery\t\t= " << battery << endl;		
		
}
