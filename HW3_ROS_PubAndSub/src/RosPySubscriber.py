#! /usr/bin/python3

import rospy


from std_msgs.msg import String
from std_msgs.msg import Float32


def callback1(msg):
    
        rospy.loginfo(rospy.get_caller_id()+'I have receive %s',msg.data)
    # elif isinstance(msg.data,Float32):
    #     rospy.loginfo(rospy.get_caller_id()+'The Float Recieve is shown as %d',msg.data)
    # else:
    #     rospy.loginfo('Data Type Error Occur!!!')
def callback2(msg):
     rospy.loginfo(rospy.get_caller_id()+'The Float Recieve is shown as %f',msg.data)

def listener():
    rospy.init_node('listener',anonymous=True)
    rospy.Subscriber('chatter_String',String,callback1)
    rospy.Subscriber('chatter_Float',Float32,callback2)
    #rospy.Subscriber('chatter',Float32,callback)
    rospy.spin()


if __name__ =='__main__':
    listener()
