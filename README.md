# Emergency Stop
Emergency stop project to remotely halt the Building Wide Intelligence (BWI) segbots of the University of Texas at Austin. This is the final project for the CS 309 FRI class, and was created in collaboration with Jessie Chen. Below is an image of the stop button and the server handler.

<p align="center">
  <img src="http://i.imgur.com/TdE5kII.jpg" width="400"/>
</p>

## Installation

This project was built for ROS Indigo on Ubuntu 16.04, and with the NodeMCU v1.0 (ESP8266 12-E DEV KIT) microcontroller. To install our project, follow these steps.

First, open a terminal and go to the source directory of your ROS workspace, then clone the project directly into there.

```
$ cd /catkin_ws/src
$ git clone https://github.com/MontyPylon/emergency_stop.git
```

Next, we need to make sure these two packages are installed:

```
$ sudo apt-get install ros-indigo-rosserial-arduino
$ sudo apt-get install ros-indigo-rosserial
```

## Running the program

To start the program, connect the NodeMCU server to the segbot, and launch the following node, being sure to change /dev/ttyUSB0 to the correct port where the NodeMCU is connected.

```
$ rosrun rosserial_python serial_node.py /dev/ttyUSB0
```

Finally, you can launch either the normal stop_base node, or the kill_vel node which will kill anything publishing to /cmd_vel. Here is how to launch those nodes:

```
$ rosrun emergency_stop stop_base
$ rosrun emergency_stop kill_vel
```

