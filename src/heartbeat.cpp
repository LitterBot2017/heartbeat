#include "ros/ros.h"

#include "navigation/Arduino.h"
#include "heartbeat/Running.h"

#include "restclient-cpp/restclient.h"
#include "restclient-cpp/connection.h"

#include <string>
#include <ros/console.h>
#include <iostream>
using namespace std;

float mLatitude = 40.442222; // Default location in case no Arduino msgs
float mLongitude = -79.945563;
int mBinFullness = 90;
int mBatteryLevel = 70;

ros::Publisher running_pub;
heartbeat::Running running_msg;

string delimiter = "isRunning\": ";

std::string makeJSONHeartbeat() {
	std::stringstream heartbeat;
	heartbeat << "{\"robotID\": 1, \"lat\": ";
	heartbeat << mLatitude << ", \"lng\": " << mLongitude << ", \"batteryLevel\": ";
	heartbeat << mBatteryLevel << ", \"signalStrength\": 100, \"binFullness\": " << mBinFullness << " }";
	return heartbeat.str();
}

void arduinoCallback(const navigation::Arduino::ConstPtr& msg) {
	mBinFullness = msg->bin_fullness;
	mBatteryLevel = msg->battery;
	mLatitude = msg->curr_lat;
	mLongitude = msg->curr_long;
}

int main(int argc, char** argv) {

	ros::init(argc, argv, "heartbeat");

	ros::NodeHandle n;

	ros::Rate loop_rate(1);

	ros::Subscriber arduinoSub = n.subscribe("arduino", 5, arduinoCallback);

	running_pub = n.advertise<heartbeat::Running>("running", 5);

	RestClient::init();

	while (ros::ok()) {
		cout << "latitude = " << mLatitude << " and longitude = " << mLongitude << endl;
		cout << "JSON = " << makeJSONHeartbeat() << endl;

		RestClient::Connection* connection = new RestClient::Connection("https://obscure-spire-79030.herokuapp.com");

		RestClient::HeaderFields headers;
		headers["Accept"] = "application/json";
		connection -> SetHeaders(headers);

		RestClient::Response r = connection->post("/api/heartbeat", makeJSONHeartbeat());

		cout << "r.response status = " << r.code << endl;
		cout << "r.response body = " << r.body << endl;
		string body = r.body;
		int startPos = body.find(delimiter) + delimiter.length();
		cout << "is_running = " << body.substr(startPos, 4) << endl;
		if (!body.substr(startPos, 4).compare("true")) {
			running_msg.is_running = true;
		} else {
			running_msg.is_running = false;
		}

		running_pub.publish(running_msg);

		ros::spinOnce();
		loop_rate.sleep();
	}
	
	return -1;
}