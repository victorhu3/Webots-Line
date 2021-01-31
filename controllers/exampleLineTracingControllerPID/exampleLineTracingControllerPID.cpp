// File:   exampleLineTracingControllerPID.cpp
// Date:   1/31/2021
// Description: A sample line tracing program with a P controller. Allows for smoother line tracing over the simple line tracing controller.
// Author:  Victor Hu
// Modifications:

// Include Webots header files
#include <webots/Robot.hpp>
#include <webots/Camera.hpp>
#include <webots/Motor.hpp>
using namespace webots;

// Other includes
#include <iostream>
using namespace std;

int main(int argc, char **argv) {

  Robot *robot = new Robot();

  // Get color sensors (implemented in Webots as Camera sensors)
  Camera *leftColor = robot->getCamera("leftColor");
  Camera *rightColor = robot->getCamera("rightColor");

  // Get motors
  Motor *leftMotor = robot->getMotor("left wheel");
  Motor *rightMotor = robot->getMotor("right wheel");

  // Get the height and width in pixels of the color sensors
  const int CAM_HEIGHT = leftColor->getHeight();
  const int CAM_WIDTH = leftColor->getWidth();

  // Get timestep of the world
  int timeStep = (int)robot->getBasicTimeStep();

  // Enable the color sensors
  leftColor->enable(timeStep);
  rightColor->enable(timeStep);
  
  // Set motors to velocity-control mode by setting position to INFINITY
  leftMotor->setPosition(INFINITY);
  rightMotor->setPosition(INFINITY);

  // Define our Kp to be .1, and the base speed to be 2.0
  const double Kp = .1;
  const double baseSpeed = 2.0;

  // Timestep loop must periodically be called for simulation to progress
  while (robot->step(timeStep) != -1) {

    // Get the darkness/lightness of the middle pixel of the color sensors
    int leftLight = leftColor->imageGetGray(leftColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);
    int rightLight = rightColor->imageGetGray(rightColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);

    cout << "left gray value: " << leftLight << " right gray value: " << rightLight << endl;

    // Our error is calculated to be the difference in the light levels of the left and right color sensor
    double leftSpeed = 0, rightSpeed = 0;
    double error = leftLight - rightLight;

      // If left color sensor is lighter, curve towards right
	  if (error > 0) {
		  leftSpeed = baseSpeed+abs(error*Kp);
		  rightSpeed = baseSpeed-abs(error*Kp);
	  }
      // If right color sensor is lighter, curve towards left
	  else if (error < 0) {
		  leftSpeed = baseSpeed-abs(error*Kp);
		  rightSpeed = baseSpeed+abs(error*Kp);
	  }
      // Otherwise just move forward
	  else {
		  leftSpeed = baseSpeed;
		  rightSpeed = baseSpeed;
	  }

      // Set our motors to move at the calculated speed
	  leftMotor->setVelocity(leftSpeed);
	  rightMotor->setVelocity(rightSpeed);    
  };

  // Cleanup code
  delete robot;
  return 0;
}
