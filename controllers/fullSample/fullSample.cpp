// File:   fullSample.cpp
// Date:   5/4/2021
// Description: Probing/using all features necessary for the challenge
// Author:  Victor Hu and Jeffrey Cheng
// Modifications:

// Include Webots header files
#include <webots/Robot.hpp>
#include <webots/Camera.hpp>
#include <webots/DistanceSensor.hpp>
#include <webots/Motor.hpp>
using namespace webots;

// Other includes
#include <iostream>
using namespace std;

// Define macro for the black-white threshold
#define THRESHOLD 100



// Make robot object global so all functions have access
Robot *robot = new Robot();

// Get motors
Motor *leftMotor = robot->getMotor("left wheel");
Motor *rightMotor = robot->getMotor("right wheel");

// Get timestep of the world
int timeStep = (int)robot->getBasicTimeStep();



void delay(int ms) {
  float initTime = robot->getTime();
  while (robot->step(timeStep) != -1)
    if ((robot->getTime() - initTime) * 1000 > ms)
      break;
}

void setSpeed(float lSpeed, float rSpeed) {
  leftMotor->setVelocity(lSpeed);
  rightMotor->setVelocity(rSpeed);
}



int main(int argc, char **argv) {

  // Get color sensors (implemented in Webots as Camera sensors)
  Camera *leftColor = robot->getCamera("leftColor");
  Camera *rightColor = robot->getCamera("rightColor");
  Camera *frontColor = robot->getCamera("frontColor");

  // Get front distance sensor
  DistanceSensor *frontDist = robot->getDistanceSensor("frontDist");
  
  // Get scoop motor
  Motor *scoop = robot->getMotor("scoop");

  // Get the height and width in pixels of the color sensors
  const int CAM_HEIGHT = leftColor->getHeight();
  const int CAM_WIDTH = leftColor->getWidth();



  // Enable the sensors
  leftColor->enable(timeStep);
  rightColor->enable(timeStep);
  frontColor->enable(timeStep);
  frontDist->enable(timeStep);
  
  // Set motors to velocity-control mode by setting position to INFINITY
  leftMotor->setPosition(INFINITY);
  rightMotor->setPosition(INFINITY);
  scoop->setPosition(INFINITY);

  // Keep motors still - use setVelocity to control motors (max speed = 12.2)
  setSpeed(0.0, 0.0);
  scoop->setVelocity(0.0);

  // Line-tracing + obstacle variables
  int frontGray, leftGray, rightGray, leftGreen, leftRed, rightGreen, rightRed;
  char frontLight = ' ', leftLight = ' ', rightLight = ' ';
  bool obstacle;

  bool evacRoom = false;



  // Timestep loop must periodically be called for simulation to progress
  while (robot->step(timeStep) != -1) {

    // Get the darkness/lightness and green/red of the middle pixel of the color sensors
    frontGray = leftColor->imageGetGray(frontColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);
    leftGray = leftColor->imageGetGray(leftColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);
    rightGray = rightColor->imageGetGray(rightColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);
    leftGreen = leftColor->imageGetGreen(leftColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);
    leftRed = leftColor->imageGetRed(leftColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);
    rightGreen = rightColor->imageGetGreen(rightColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);
    rightRed = rightColor->imageGetRed(rightColor->getImage(), CAM_WIDTH, CAM_WIDTH/2, CAM_HEIGHT/2);



    // Check for black/white
    if (frontGray > THRESHOLD)
      frontLight = 'W';
    else
      frontLight = 'B';
    if (leftGray > THRESHOLD)
      leftLight = 'W';
    else
      leftLight = 'B';
    if (rightGray > THRESHOLD)
      rightLight = 'W';
    else
      rightLight = 'B';

    // Check for green - don't care about front sensor green
    if ((float)leftGreen / leftRed > 1.3)
      leftLight = 'G';
    if ((float)rightGreen / rightRed > 1.3)
      rightLight = 'G';

    // Check for evac entrance (gray strip) or exit (green strip)
    if (leftGray > 155 && leftGray < 175 && rightGray > 155 && rightGray < 175) {
      evacRoom = true;
      cout << "Entered Evacuation Zone\n";
    }
    if (evacRoom && (float)leftGreen / leftRed > 1.3 && (float)rightGreen / rightRed > 1.3) {
      evacRoom = false;
      cout << "Exited Evacuation Zone\n";
    }



    // Check for obstacle if not evac
    if (!evacRoom && frontDist->getValue() < 20)
      obstacle = true;
    else
      obstacle = false;

    // Check for victim if in evac
    if (evacRoom && frontDist->getValue() < 20) {
      // Raise and lower scoop
      scoop->setVelocity(1);
      delay(1000);
      scoop->setVelocity(-1);
      delay(1000);
      scoop->setVelocity(0);
    }

    cout << CAM_HEIGHT << endl;
    //cout << "L: " << leftGray << " F: " << frontLight << " R: " << rightGray << "   O: " << obstacle << endl;
  };

  // Cleanup code
  delete robot;
  return 0;
}
