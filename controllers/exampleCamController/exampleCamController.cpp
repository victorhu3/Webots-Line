// File:   exampleCamController.cpp
// Date:   5/14/2021
// Description: Sample code for using the camera with opencv (finding blue ball in image)
// Author:  Julian Lee
// Modifications:

// Include Webots header files
#include <webots/Robot.hpp>
#include <webots/Camera.hpp>
#include <webots/Motor.hpp>
using namespace webots;

//camera includes
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;

// Other includes
#include <iostream>
using namespace std;

bool compCont(vector<Point> contour1, vector<Point> contour2){
  double i = contourArea(contour1,false);
  double j = contourArea(contour2,false);
  return (i > j);
}

int main(int argc, char **argv) {

  Robot *robot = new Robot();

  // Get camera
  Camera *cam = robot->getCamera("cam");

  // Get the height and width in pixels of the camera
  const int CAM_HEIGHT = cam->getHeight();
  const int CAM_WIDTH = cam->getWidth();

  // Get timestep of the world
  int timeStep = (int)robot->getBasicTimeStep();

  // Enable camera
  cam->enable(timeStep);

  // Timestep loop must periodically be called for simulation to progress
  while (robot->step(timeStep) != -1) {
     
     // Convert the camera image into an openCV Mat. CV_8UC4 means image is in RGBA format.
    Mat frame(CAM_HEIGHT, CAM_WIDTH, CV_8UC4, (void*)cam->getImage());
    cvtColor(frame,frame,COLOR_RGBA2RGB);//convert image to RGB
    
    //will display the frame seen by the camera in a new window named "original frame"; use for debugging
    imshow("original frame", frame);
    
    //blurring the image can help reduce the noise of an image. 
    //visit https://docs.opencv.org/master/d4/d13/tutorial_py_filtering.html for examples/an explanation of the different types of blurs, 
    //heree, we are applying a gaussian blur with kernel size (5,5) to our frame. Increasing the kernel size results in a larger blur, but the function will take longer to run.
    GaussianBlur(frame,frame,Size(3,3),0,0);
    
    //threshold a colored image: pixels between bounds are turned black
    //first scalar is lower bound, second is upper bound Scalar(red val, green val, blue val)
    Mat threshImg;
    inRange(frame,Scalar(50,0,0),Scalar(255,255,255),threshImg);
    //invert colors of image so tha tobjects of interest appear as white
    bitwise_not(threshImg,threshImg);
    imshow("thresh", threshImg);
    
    //detect contours (groups of white pixels) in the image
      //some of these contours will represent our red ball. Smaller contours are noise and need to be filtered out
    //more on how to use contours: https://docs.opencv.org/master/d3/d05/tutorial_py_table_of_contents_contours.html
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    findContours(threshImg,contours,hierarchy,RETR_LIST,RETR_TREE);
    if(contours.size() > 0){//at least one contour detected
      sort(contours.begin(),contours.end(),compCont);//sort contours by size
      int currIdx = 0;
      while(currIdx < (int)contours.size() && contourArea(contours[currIdx]) > 10){
        //we have detected a contour big enough to be a red ball
        Point2f center;
        float radius;
        minEnclosingCircle(contours[currIdx],center,radius);//find center of contour
        cout << "Potential red ball at (" << center.x << "," << center.y << ")" << endl;
        circle(frame,center,2,Scalar(255,0,0),FILLED);//add blue dot to ball location in the colored image
        currIdx++;
      }
    }
    
    imshow("labeled balls", frame);
    
    
    //keep this command in the while loop when using the imshow command to get a continuous feed of images
    waitKey(1);
        
    //other useful commands
    //cvtColor(frame,frame,COLOR_BGR2GRAY); //grayscale: used to simplify image, especially when attempting to detect noncolored objects
    //threshold(frame,frame,80,255,THRESH_BINARY); //threshold a grayscaled image (meaning converting the image to black and white pixels): types of thresholding shown here: https://docs.opencv.org/master/d7/d4d/tutorial_py_thresholding.html

  };

  // Cleanup code
  delete robot;
  return 0;
}
