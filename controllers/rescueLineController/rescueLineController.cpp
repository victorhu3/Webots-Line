// File:          rescueLineController.cpp
// Date:
// Description:
// Author:
// Modifications:
#include <webots/Robot.hpp>
#include <iostream>
#include <webots/Camera.hpp>
#include <webots/Motor.hpp>
#include <webots/DistanceSensor.hpp>

using namespace webots;
using namespace std;

int main(int argc, char **argv) {

  Robot *robot = new Robot();

  int timeStep = (int)robot->getBasicTimeStep();

  Camera *camLeft = robot->getCamera("cameraLeft");
  Camera *camRight = robot->getCamera("cameraRight");

  Motor *motorLeft = robot->getMotor("left wheel");
  Motor *motorRight = robot->getMotor("right wheel");

  camLeft->enable(timeStep);
  camRight->enable(timeStep);

  ultrasonic->enable(timeStep);

  const double Kp = .1;
  double baseSpeed = 2.0;

  motorLeft->setPosition(INFINITY);
  motorRight->setPosition(INFINITY);

  while (robot->step(timeStep) != -1) {
      
	  int leftLight = camLeft->imageGetGray(camLeft->getImage(), 1, 0, 0);
	  int rightLight = camRight->imageGetGray(camRight->getImage(), 1, 0, 0);
	
	  cout << "left: " << leftLight << " right: " << rightLight << endl;

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

	  motorLeft->setVelocity(leftSpeed);
	  motorRight->setVelocity(rightSpeed);
  };

  delete robot;
  return 0;
}
