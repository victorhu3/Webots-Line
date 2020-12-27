// File:          rescueLineController.cpp
// Date:
// Description:
// Author:
// Modifications:

// You may need to add webots include files such as
// <webots/DistanceSensor.hpp>, <webots/Motor.hpp>, etc.
// and/or to add some other includes
#include <webots/Robot.hpp>
#include <iostream>
#include <webots/Camera.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>

// All the webots classes are defined in the "webots" namespace
using namespace webots;
using namespace std;

// This is the main program of your controller.
// It creates an instance of your Robot instance, launches its
// function(s) and destroys it at the end of the execution.
// Note that only one instance of Robot should be created in
// a controller program.
// The arguments of the main function can be specified by the
// "controllerArgs" field of the Robot node
int main(int argc, char **argv) {
  // create the Robot instance.
  Robot *robot = new Robot();

  int timeStep = (int)robot->getBasicTimeStep();

  Camera *camLeft = robot->getCamera("cameraLeft");
  Camera *camRight = robot->getCamera("cameraRight");

  Motor *motorLeft = robot->getMotor("left wheel");
  Motor *motorRight = robot->getMotor("right wheel");

  DistanceSensor *ultrasonic = robot->getDistanceSensor("so4");

  camLeft->enable(timeStep);
  camRight->enable(timeStep);

  ultrasonic->enable(timeStep);

  const double Kp = .1;
  double baseSpeed = 2.0;

  // You should insert a getDevice-like function in order to get the
  // instance of a device of the robot. Something like:
  //  Motor *motor = robot->getMotor("motorname");
  //  DistanceSensor *ds = robot->getDistanceSensor("dsname");
  //  ds->enable(timeStep);

  motorLeft->setPosition(INFINITY);
  motorRight->setPosition(INFINITY);
  motorLeft->setVelocity(0);
  motorRight->setVelocity(0);

  // Main loop:
  // - perform simulation steps until Webots is stopping the controller
  while (robot->step(timeStep) != -1) {
    // Read the sensors:
    // Enter here functions to read sensor data, like:
    //  double val = ds->getValue();
	  int leftLight = camLeft->imageGetGray(camLeft->getImage(), 1, 0, 0);
	  int rightLight = camRight->imageGetGray(camRight->getImage(), 1, 0, 0);
	
	  int dist = ultrasonic->getValue();

	 // if (dist < .3) {
		  cout << dist << endl;
	//  }

		  auto imageLeft = camLeft->getImage();
	  cout << "left: " << leftLight << " right: " << rightLight << endl;
	  cout << "left rgb: " << (int)camLeft->imageGetRed(imageLeft, 1, 0, 0) << " " << (int)camLeft->imageGetGreen(imageLeft, 1, 0, 0) << " " << (int)camLeft->imageGetBlue(imageLeft, 1, 0, 0) << endl;
    // Process sensor data here.
	  double leftSpeed = 0, rightSpeed = 0, error = leftLight - rightLight;

	  if (error > 0) {
		  leftSpeed = baseSpeed+abs(error*Kp);
		  rightSpeed = baseSpeed-abs(error*Kp);
	  }
	  else if (error < 0) {
		  leftSpeed = baseSpeed-abs(error*Kp);
		  rightSpeed = baseSpeed+abs(error*Kp);
	  }
	  else {
		  leftSpeed = baseSpeed;
		  rightSpeed = baseSpeed;
	  }

    // Enter here functions to send actuator commands, like:
    //  motor->setPosition(10.0);

	  // motorLeft->setVelocity(leftSpeed);
	  // motorRight->setVelocity(rightSpeed);
  };

  // Enter here exit cleanup code.

  delete robot;
  return 0;
}
