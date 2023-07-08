# 2023年春nku-ai的机器人软件工程学的作业存档

**以下作业均在Ubuntu20.04和Ros-Noetic环境上实验**
## HW1：使用CMake编译OpenCV显示图片的程序(HW1_OpencvDemo)
    在Ubuntu20,04上编译Opencv进行安装(略)
    源码见src文件夹中的`LoadShowPic.cpp`，配置CMakeLists.txt如下:
```wiki
cmake_minimum_required(VERSION 3.16.3)
project(OpencvDemo)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

find_package(OpenCV REQUIRED)


include_directories(${OpenCV_INCLUDE_DIRS})

add_executable(${PROJECT_NAME} src/LoadShowPics.cpp)
target_link_libraries(${PROJECT_NAME} ${OpenCV_LIBS})
```
使用指令`cmake .`和`make -j8`命令进行编译，在build文件夹下生成可执行文件OpencvDemo

运行命令`./OpencvDemo`即可查看到带有"TheImg"标题的linux企鹅图标
## HW2：使用C语言socket库实现双向通信的简单计算机网络Demo程序

传输数据的服务器程序:`Servers.c`以`Socket_Server.c`为参考,使用`gcc Severs.c -o Servers.o -lpthread`命令进行编译生成可执行文件`Servers.o`

接受数据的服务端程序:`Clients.c`以`Socket_Client.c`为参考，使用`gcc Clients.c -o Clients.o -lpthread`命令进行编译生成可执行文件`Clients.o`

## HW3:使用C++编写ROS消息发布器并且使用Python编写ROS消息订阅器程序

### 如何使用：
使用命令`catkin_create_pkg <Your Ros Pkg Name> std_msgs rospy roscpp`在工作空间文件夹`catkin_ws`中创建包文件并复制文件夹下所有内容到包文件夹空间下。或者参照packages.xml进行配置

建议安装`ros-noetic-python-catkin-tools`使用`catkin build`进行编译，也可以使用`catkin_make`命令
进行包内源码的编译,记得给`RosPySubscriber.py`文件权限`chmox +x RosPySubscriber.py`。

编译成功后启动ros：`roscore`

即可使用命令`rosrun <Your Ros Pkg Name> PersonPublisher`进行消息发布节点的运行

使用命令`rosrun <Your Ros Pkg Name> RosPySubscriber.py`进行消息订阅节点的运行



------

------

Update 2023/6/9/

## 更新：机器人软件工程学期末结课大作业

文件夹为`RbSoftWareFinalProject`

目录结构为

* **doc_pdf**：项目文档
* **my_turtlebot2_demo**：项目源码
* **my_dependencies**:项目依赖项，包含dynamixel补丁，修改后的古月居机器人语音开源包，机械臂配置

此外项目使用需要在ubuntu安装cuda以及tensorflow ,tensoorflow-object-detection-api。请参照教程和本机环境进行配置，以上代码均在tensorflow-gpu-2.4.4环境下运行。此外，还需要对文档提到的问题重视

####  依赖项文件的几点说明

1. dynamixel_motor_Noetic为可以使用python3 rospy的包
2. **robot_voice**基于古月居的guyueclass开源教程，项目修改部分主要是语言听写和语言对话的部分。记住必须先申请科大讯飞的语言听写包然后

​	下载解压到本地并按照教程配置环境变量才可以正常使用，且使用时请吧代码的ippid改为自己的id号，可以在科大讯飞官网查看自己申请的工具的ID号

​	3.其余问题见文档手册

