// File:   exampleLineTracingController.cpp
// Date:   1/31/2021
// Description: A very simple example line tracing controller.
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

// Define macro for the black-white threshold
#define THRESHOLD 100

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

  // Timestep loop must periodically be called for simulation to progress
  while (robot->step(timeStep) != -1) {

    // Get the darkness/lightness of the middle pixel of the color sensors
    int leftLight = leftColor->imageGetGray(leftColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);
    int rightLight = rightColor->imageGetGray(rightColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);

    cout << "left gray value: " << leftLight << " right gray value: " << rightLight << endl;

    // If left color sensor sees black and right color sensor sees white, point turn to the left
    if(leftLight <= THRESHOLD && rightLight > THRESHOLD) {
        leftMotor->setVelocity(-3);
        rightMotor->setVelocity(3);
    }
    // If left color sensor sees white and right color sensor sees black, point turn to the right
    else if(leftLight > THRESHOLD && rightLight <= THRESHOLD) {
        leftMotor->setVelocity(3);
        rightMotor->setVelocity(-3);
    }
    // Otherwise move forward
    else {
        leftMotor->setVelocity(5);
        rightMotor->setVelocity(5);
    }
  };

  // Cleanup code
  delete robot;
  return 0;
}
