#include "ros/ros.h"
#include "geometry_msgs/Vector3Stamped.h"
#include "sensor_msgs/Temperature.h"
#include "sensor_msgs/FluidPressure.h"
#include "std_msgs/Float64.h"
#include "std_msgs/String.h"
#include "mavros_msgs/State.h"
#include "mavros_msgs/BatteryStatus.h"
#include "mavros_msgs/OverrideRCIn.h"
#include "mavros_msgs/RCIn.h"
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
int rc_failsafe_override_flag = 0;
int rc_in_data_channel[8];
int channel_7_mid = 1500;
// channel_7_off = 987 | channel_7_on = 2010

void debugging(string drone_status_debug);
void rcinReceiver(const mavros_msgs::RCIn& rc_in_data);
void altReceiver(const std_msgs::Float64& alt_recv);
void compassReceiver(const std_msgs::Float64& compass_recv);
void velocityReceiver(const geometry_msgs::Vector3Stamped& velocity_recv);
void temperatureReceiver(const sensor_msgs::Temperature& temperature_recv);
void pressureReceiver(const sensor_msgs::FluidPressure& pressure_recv);
void stateReceiver(const mavros_msgs::State& state_recv);
void batteryReceiver(const mavros_msgs::BatteryStatus& battery_recv);
void vDataMonitor(const std_msgs::String& vData);
ros::Publisher pub_rc_override;
ros::Publisher pub_incoming_reply;
mavros_msgs::OverrideRCIn rc_override_data;

int main(int argc, char **argv)
{

	ros::init(argc, argv, "system_monitor");
	ros::NodeHandle sys_mon;
	ros::Subscriber sub_state = sys_mon.subscribe("mavros/state", 100, stateReceiver);
	ros::Subscriber sub_rel_alt = sys_mon.subscribe("/mavros/global_position/rel_alt", 1, altReceiver );
	ros::Subscriber sub_compass = sys_mon.subscribe("/mavros/global_position/compass_hdg", 10, compassReceiver );
	ros::Subscriber sub_vel = sys_mon.subscribe("/mavros/global_position/gp_vel", 10, velocityReceiver );
	ros::Subscriber sub_temperature = sys_mon.subscribe("/mavros/imu/temperature", 10, temperatureReceiver );
	ros::Subscriber sub_pressure = sys_mon.subscribe("/mavros/imu/atm_pressure", 10, pressureReceiver );
	ros::Subscriber sub_battery = sys_mon.subscribe("/mavros/battery", 10, batteryReceiver );
	ros::Subscriber rc_in_sub 	= sys_mon.subscribe("/mavros/rc/in", 100, rcinReceiver);
	ros::Subscriber sub_vd = sys_mon.subscribe("art_vrd/voice_data", 10, vDataMonitor);
	pub_incoming_reply = sys_mon.advertise<std_msgs::String>("art_vrd/incoming_reply", 100);
	pub_rc_override	= sys_mon.advertise<mavros_msgs::OverrideRCIn>("/mavros/rc/override", 1, true);
	ROS_INFO("Starting System Monitor.");
	// set initial state for rc override take over.
	if(rc_in_data_channel[6] < channel_7_mid){
			rc_failsafe_override_flag = 0;
	}
	else if (rc_in_data_channel[6] > channel_7_mid){
			rc_failsafe_override_flag = 1;
	}
	
	ros::spin();
  
  return 0;
}


void stateReceiver(const mavros_msgs::State& state_recv){
	
	flight_mode = state_recv.mode;
	arm_state = state_recv.armed;
}

void rcinReceiver(const mavros_msgs::RCIn& rc_in_data){
	int x;
	for (x = 0; x<8;x++){
		rc_in_data_channel[x] = rc_in_data.channels[x];
	}
	if(rc_in_data_channel[6] < channel_7_mid && rc_failsafe_override_flag == 0){
		for(int i=0; i < 8; i++) rc_override_data.channels[i] = 0;
		pub_rc_override.publish(rc_override_data);
		rc_failsafe_override_flag = 1;
		ROS_ERROR_STREAM( "[SM] RC is now taking over!") ;
	}
	else if (rc_in_data_channel[6] > channel_7_mid && rc_failsafe_override_flag == 1){
		rc_failsafe_override_flag = 0;
		ROS_ERROR_STREAM( "[SM] Drone is now taking over") ;
	}
	
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
void vDataMonitor(const std_msgs::String& vData){
	std_msgs::String incoming_reply;
	if (vData.data[0] == 's' && vData.data[1] == 'c'){
		ROS_INFO_STREAM( "[SM] It's a sc command") ;	
	}
	
	else if(vData.data[0] == 'd' && vData.data[1] == 's'){
		ROS_INFO_STREAM( "[SM] It's a ds command") ;	
	}
	
	else if (vData.data[0] == 'v' && vData.data[1] == 'f'){
		ROS_INFO_STREAM( "[SM] It's a vf command") ;	
	}
	
	else{
		
		incoming_reply.data = "gi:error_unkown_command";
		pub_incoming_reply.publish(incoming_reply);
		ROS_ERROR_STREAM( "[SM] It's not a ds, sc, or vf command");	
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
