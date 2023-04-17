
#include"ros/ros.h"
//#include "std_msgs/Variant.h"
#include "std_msgs/String.h"
#include "std_msgs/Float32.h"
#include <sstream>


int main(int argc,char**argv){
    ros::init(argc,argv,"talker");

    ros::NodeHandle n;
    ros::Publisher chatter_pub_s= n.advertise<std_msgs::String>("chatter_String",1000);
    ros::Publisher chatter_pub_f = n.advertise<std_msgs::Float32>("chatter_Float",1000);
    ros::Rate loop_rate(10);
    int count=0;
    std_msgs::Float32 floatData;
    std_msgs::Float32 floatDelta;

    floatData.data =3.13;
    floatDelta.data=0.01;
    while(ros::ok()){
        std_msgs::String msg;
        std::stringstream ss;
        ss<<"Hello World"<<count;

        msg.data = ss.str();

        ROS_INFO("%s",msg.data.c_str());
        
        chatter_pub_s.publish(msg);

        float sum=floatData.data+floatDelta.data;
        floatData.data = sum;

        ROS_INFO("The Float arg is:%f",floatData.data);

        chatter_pub_f.publish(floatData);



        ros::spinOnce();
        loop_rate.sleep();
        count++;
    }
}