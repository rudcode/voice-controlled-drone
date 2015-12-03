#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include "geometry_msgs/TwistStamped.h"
#include "geometry_msgs/PoseStamped.h"
// ################ new mavros msg lib is in mavros_msgs ################ 
//#include "mavros/SetMode.h"
//#include "mavros/State.h"
//#include "mavros/OverrideRCIn.h"
//#include "mavros/RCIn.h"
#include "mavros_msgs/SetMode.h"
#include "mavros_msgs/State.h"
#include "mavros_msgs/OverrideRCIn.h"
#include "mavros_msgs/RCIn.h"
// ################ new mavros msg lib is in mavros_msgs ################ 
#include "std_msgs/Int16.h"
#include "std_msgs/Float64.h"
#include "std_msgs/String.h"
#include <unistd.h>
#define COMMAND_LIST 9
#define DISTANCELIMIT 10

void positionReceiver(const geometry_msgs::PoseStamped& local_recv);
void mainMovementController(const std_msgs::String& main_fmc);
void altReceiver(const std_msgs::Float64& alt_msg);
// ################ new mavros msg lib is in mavros_msgs ################ 
//void rcinReceiver(const mavros::RCIn& rc_in_data);
//void stateReceiver(const mavros::State& state_recv);
void stateReceiver(const mavros_msgs::State& state_recv);
void rcinReceiver(const mavros_msgs::RCIn& rc_in_data);
// ################ new mavros msg lib is in mavros_msgs ################
void moveDrone(char axis, int location);
int changeFlightMode(const char* mode);
void sendReply(const char* reply_message);

using namespace std;

int command_detected;
const char* command_search;
int command_list = COMMAND_LIST;
const char* known_command[COMMAND_LIST];

int pos_x = 0;
int pos_y = 0;
int pos_z = 0;
string flight_mode = "AUTO";
int rc_in_channel_7 = 0;
float rel_alt = 0;
int channel_7_mid = 1500;
// channel_7_off = 987 | channel_7_on = 2010
	
ros::Publisher pub_incoming_reply;
ros::Publisher pub_quad_pos;
ros::Publisher pub_rc_override;	
ros::ServiceClient client;

// ################ new mavros msg lib is in mavros_msgs ################ 
//mavros::SetMode flight;
//mavros::OverrideRCIn rc_command;
mavros_msgs::SetMode flight;
mavros::OverrideRCIn rc_override;

// ################ new mavros msg lib is in mavros_msgs ################ 
geometry_msgs::PoseStamped quad_pos;
std_msgs::String incoming_reply;
