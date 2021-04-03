#include <webots/Supervisor.hpp>
using namespace webots;

#include <string>
#include <cstring>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

#define TIME_STEP 32

const double SOUTH[] = {0, 1, 0, 0};
const double EAST[] = {0, 1, 0, 1.57};
const double NORTH[] {0, 1, 0, 3.15};
const double WEST[] = {0, 1, 0, 4.72};
int width = 0, height = 0;

typedef struct {
    int num;
    char points;
} tile;

int checkpointCmp(Node* a, Node* b) {

   return a->getField("name")->getSFString() < b->getField("name")->getSFString(); 
}

double findDistance(const double a[], const double b[]) {

    return sqrt(pow(a[0]-b[0],2) + pow(a[2]-b[2], 2));
}

int toGridNum(const double *coord) {
    return int((coord[2] + 0.15) / 0.3) * width + int((coord[0] + 0.15) / 0.3);
}

bool inTriangle(double x, double y, double Ax, double Ay, double Bx, double By, double Cx, double Cy){
  double A = fabs(Ax*(By-Cy)+Bx*(Cy-Ay)+Cx*(Ay-By))/2;
  double A1 = fabs(x*(By-Cy)+Bx*(Cy-y)+Cx*(y-By))/2;
  double A2 = fabs(Ax*(y-Cy)+x*(Cy-Ay)+Cx*(Ay-y))/2;
  double A3 = fabs(Ax*(By-y)+Bx*(y-Ay)+x*(Ay-By))/2;
  if(fabs(A-A1-A2-A3) < 0.001)
    return true;
  return false;
}

int main() {

    Supervisor *supervisor = new Supervisor();

    Node *robot = supervisor->getFromDef("player0");
	
	
    //checkpoints
    Node *checkpointGroup = supervisor->getFromDef("Checkpoints");
	vector<tile> path;

    string pathStr = supervisor->getFromDef("Tiles")->getField("children")->getMFNode(0)->getField("description")->getSFString();
    int tileNum = 0, ind;
    char tilePoints = '\0';
    for (ind = 0; pathStr[ind] != ';'; ind++) {
        switch (pathStr[ind]) {
            case ',':
                path.push_back(tile{ tileNum, tilePoints });
                tileNum = 0;
                tilePoints = '\0';
                break;
            case '!':
            case '@':
            case '#':
                tilePoints = pathStr[ind];
                break;
            default:
                tileNum = tileNum * 10 + pathStr[ind] - '0';
        }
    }
    while (pathStr[++ind] != ',')
        height = height * 10 + pathStr[ind] - '0';
    while (pathStr[++ind] != ',')
        width = width * 10 + pathStr[ind] - '0';

    if(robot == NULL) {
        cout << "No player0 robot found" << endl;
    }
    if(checkpointGroup == NULL) {
        cout << "No checkpoints group found" << endl;
    }

    vector<Node*> checkpoints;
    for(int i = 0; i < checkpointGroup->getField("children")->getCount(); i++) {

        checkpoints.push_back(checkpointGroup->getField("children")->getMFNode(i));
    }

    //sort checkpoints by name in ascending order (0, 1, 2, etc.)
    sort(checkpoints.begin(), checkpoints.end(), checkpointCmp);

    //send distances between checkpoints
    string checkpointVal = "D", tmp;
    int finalInd, preEvac = checkpoints.size();
    for (int i = 0; i < (int)checkpoints.size(); i++) {
        tmp = (checkpoints[i]->getField("description")->getSFString());
        finalInd = tmp.length();
        if (tmp.substr(tmp.length() - 1).compare("*") == 0) {
            finalInd--;
            preEvac = i;
        }
        checkpointVal += tmp.substr(1, finalInd) + ",";
    }
    supervisor->wwiSendText(checkpointVal);

    vector<Node*>::iterator nextCheckpoint = checkpoints.begin();



    //store robot's pose at beginning of game
    double initialPos[3], initialRot[4];
    memcpy(initialPos, robot->getField("translation")->getSFVec3f(), sizeof(initialPos));
    memcpy(initialRot, robot->getField("rotation")->getSFRotation(), sizeof(initialRot)); 

    int lastInd = 0;
    tileNum = 0, ind = 0;

      //victims
      //IMPORTANT: victims group currently needs to be first in children table; only supports up to 9 silver and 9 black victism currently (easy fix if need be)
      int numSVic = 0, numBVic = 0;//victim color counts
      Node *rescueZone = supervisor->getFromDef("RescueZone");
      Node *victims = rescueZone->getField("children")->getMFNode(0);
      //find delivery zone rotation        
      Node *deliveryLoc = NULL;
      for(int i = 1; i < rescueZone->getField("children")->getCount(); i++) {
          string s1 = "delivery";
          if(0 == s1.compare(rescueZone->getField("children")->getMFNode(i)->getField("description")->getSFString()))//delivery zone node found
            deliveryLoc = rescueZone->getField("children")->getMFNode(i);
      }
      const double *deliveryRot = deliveryLoc->getField("rotation")->getSFRotation();
      const double *deliveryT = deliveryLoc->getField("translation")->getSFVec3f();
      //find delivery zone triangle (4 possible rotations)
      float deliveryPts[6] = {0};
      //supervisor->wwiSendText("V" + to_string(deliveryRot[3]));
      if(fabs(deliveryRot[3] - 0) < 0.1){
         float pts[6] = {0.15,0.15,0.15,-0.15,-0.15,-0.15};
         for(int i = 0; i < 6; i++)
           deliveryPts[i] = pts[i];
      }
      else if(fabs(fabs(deliveryRot[3]) - 3.14) < 0.1){
         float pts[6] = {0.15,0.15,-0.15,0.15,-0.15,-0.15};
         for(int i = 0; i < 6; i++)
           deliveryPts[i] = pts[i];
      }
      else if((fabs(deliveryRot[3] - 1.57) < 0.1 && deliveryRot[1] == 1) || (fabs(deliveryRot[3] + 1.57) < 0.1 && deliveryRot[1] == -1)){
         float pts[6] = {-0.15,0.15,-0.15,-0.15,0.15,-0.15};
         for(int i = 0; i < 6; i++)
           deliveryPts[i] = pts[i];
      }
      else{
         float pts[6] = {-0.15,0.15,0.15,0.15,0.15,-0.15};
         for(int i = 0; i < 6; i++)
           deliveryPts[i] = pts[i];
      }
        //add translation
      for(int i = 0; i < 6; i+=2){
        deliveryPts[i] += deliveryT[0];//x translation
        deliveryPts[i+1] += deliveryT[2];//z translation
      }  

    while(supervisor->step(TIME_STEP) != -1) {
        string msg = "";
        numSVic = 0;
        numBVic = 0;
        //see if victim in evac
        for(int i = 0; i < victims->getField("children")->getCount(); i++) {
          const double *vicT = victims->getField("children")->getMFNode(i)->getField("translation")->getSFVec3f();
          /*if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("black") == 0){
            supervisor->wwiSendText("P" + to_string(vicT[0]) + "," + to_string(vicT[2]) + "," + to_string(deliveryPts[0]) + "," + to_string(deliveryPts[1]) + "," + to_string(deliveryPts[2]) + "," + to_string(deliveryPts[3]) + "," + to_string(deliveryPts[4]) + "," + to_string(deliveryPts[5]));
            supervisor->wwiSendText("P" + to_string(inTriangle(vicT[0],vicT[2],deliveryPts[0],deliveryPts[1],deliveryPts[2],deliveryPts[3],deliveryPts[4],deliveryPts[5])));
          }*/
          if(inTriangle(vicT[0],vicT[2],deliveryPts[0],deliveryPts[1],deliveryPts[2],deliveryPts[3],deliveryPts[4],deliveryPts[5])){
            if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("silver") == 0)
              numSVic += 1;
            else 
              numBVic += 1;
          }
        }   
        supervisor->wwiSendText("V" + to_string(numSVic) + " " + to_string(numBVic));


        tileNum = toGridNum(robot->getField("translation")->getSFVec3f());
        if (tileNum != path[ind].num) {
            if (tileNum == path[ind + 1].num) {
                string tmp = "S";
                supervisor->wwiSendText(tmp + path[ind].points);
                ind++;
            }
            else {
                ind = max(lastInd - 1, 0);
                msg = "L";
            }
        }

        if(msg.length() > 0 || (msg = supervisor->wwiReceiveText()).length() > 0) {
           
            if(msg == "L") {
                //Lack of progress button has been pushed
                cout << "Lack of Progress: " << endl;

                if(nextCheckpoint == checkpoints.begin()) {
                    //robot hasn't reached the first checkpoint, reset back to pose at start of game
                    robot->getField("translation")->setSFVec3f(initialPos);
                    robot->getField("rotation")->setSFRotation(initialRot);
                    
                    cout << "Resetting back to start" << endl;
                }
                else {
                
                    double checkpointTilePos[3];
                    memcpy(checkpointTilePos, (*(nextCheckpoint- 1))->getField("translation")->getSFVec3f(), sizeof(checkpointTilePos));
                    //adjust for checkpoint marker offset from center of checkpoint tile
                    checkpointTilePos[0] += .12;
                    checkpointTilePos[2] += .12;

                    //keep the same height as robot
                    checkpointTilePos[1] = robot->getField("translation")->getSFVec3f()[1];

                    string dir = (*(nextCheckpoint-1))->getField("description")->getSFString();
                    dir = dir[0];

                    if(dir == "N") {
                        robot->getField("rotation")->setSFRotation(NORTH);
                        checkpointTilePos[2] -= .15;
                    }
                    else if(dir == "E") {
                        robot->getField("rotation")->setSFRotation(EAST);
                        checkpointTilePos[0] += .15;
                    }
                    else if(dir == "S") {
                        robot->getField("rotation")->setSFRotation(SOUTH);
                        checkpointTilePos[2] += .15;
                    }
                    else if(dir == "W") {
                        robot->getField("rotation")->setSFRotation(WEST);
                        checkpointTilePos[0] -= .15;
                    }

                    robot->getField("translation")->setSFVec3f(checkpointTilePos);
                }
            }
            else if (msg == "1")
                cout << "Level 1!" << endl;
            else if (msg == "2")
                cout << "Level 2!" << endl;
        }

        if(nextCheckpoint != checkpoints.end()) {

            double checkpointTilePos[3];
            memcpy(checkpointTilePos, (*nextCheckpoint)->getField("translation")->getSFVec3f(), sizeof(checkpointTilePos));
            checkpointTilePos[0] += .12;
            checkpointTilePos[2] += .12;
            if(findDistance(checkpointTilePos, robot->getField("translation")->getSFVec3f()) < .15) {
                
                cout << "Reached checkpoint" << endl;
                nextCheckpoint++;
                if (nextCheckpoint - checkpoints.begin() - 1 == preEvac)
                    supervisor->wwiSendText("C*");
                else if (nextCheckpoint - checkpoints.begin() - 2 == preEvac)
                    supervisor->wwiSendText("C-");
                else
                    supervisor->wwiSendText("C");
                lastInd = ind;
            }
        }

    }

    delete supervisor;
    return 0;
}
