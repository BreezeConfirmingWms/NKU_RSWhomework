#! /usr/bin/env python

import rospy
from std_msgs.msg import String


rospy.init_node('StringPublisher')

chinese_str = "你好"  # 使用Unicode字符串表示中文



pub = rospy.Publisher('/voiceWords', String, queue_size=10)
rate = rospy.Rate(100)
cnt=0
while not rospy.is_shutdown() :
    msg = String()
    msg.data = chinese_str
    msg.data = msg.data.encode('utf-8  ')
    pub.publish(msg)
    rate.sleep()