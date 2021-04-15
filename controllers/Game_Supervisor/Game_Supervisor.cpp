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
    double rescuedColor[3] = {0,1,1};
    Node *rescueZone = supervisor->getFromDef("RescueZone");
    Node *victims = rescueZone->getField("children")->getMFNode(0);
    //find delivery zone rotation/location and rescue kit location    
    supervisor->wwiSendText("P1");    
    Node *deliveryZone = NULL, *rescueKit = NULL;
    for(int i = 1; i < rescueZone->getField("children")->getCount(); i++) {
        string s1 = "delivery";
        if(rescueZone->getField("children")->getMFNode(i)->getField("description") != NULL)
          if(0 == s1.compare(rescueZone->getField("children")->getMFNode(i)->getField("description")->getSFString()))//delivery zone node found
            deliveryZone = rescueZone->getField("children")->getMFNode(i);
        s1 = "rescueKit";
        if(rescueZone->getField("children")->getMFNode(i)->getField("name") != NULL)
          if(0 == s1.compare(rescueZone->getField("children")->getMFNode(i)->getField("name")->getSFString()))//delivery zone node found
            rescueKit = rescueZone->getField("children")->getMFNode(i);
    }
    const double *kitT = rescueKit->getField("translation")->getSFVec3f();//relative to rescue zone
    const double *deliveryRot = deliveryZone->getField("rotation")->getSFRotation();
    const double *deliveryT = deliveryZone->getField("translation")->getSFVec3f();
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

    bool vicCounted[20] = {0};
    int rescueKitState = 0;//0: not rescued; 1: rescued, not recorded; 2: rescued, recorded

    while(supervisor->step(TIME_STEP) != -1) {
        
        //VICTIM
        numSVic = 0;
        numBVic = 0;
        //get scoop info
        double currPos[3], currRot[4], relScoopPos[3], scoopPos[3];
        memcpy(currPos, robot->getField("translation")->getSFVec3f(), sizeof(currPos));
        memcpy(currRot, robot->getField("rotation")->getSFRotation(), sizeof(currRot)); 
        /*base off scoop location instead of center of robot: doesn't work
        memcpy(relScoopPos, robot->getField("children")->getMFNode(0)->getField("translation")->getSFVec3f(), sizeof(relScoopPos));
        scoopPos[0] = currPos[0] - sin(currRot[3])*(0.12)*currRot[1];//actual rel location at 0.12; ***not correctly compensating for robot rotation when factoring in scoop relative position
        scoopPos[2] = currPos[2] - cos(currRot[3])*(0.12)*currRot[1];
        supervisor->wwiSendText("P" + to_string(scoopPos[0]) + "," + to_string(scoopPos[2]));
        supervisor->wwiSendText("P" + to_string(cos(currRot[3])*(-1*relScoopPos[2])) + "," + to_string(sin(currRot[3])*relScoopPos[2]));
       supervisor->wwiSendText("P Robot:" + to_string(currPos[0]) + "," + to_string(currPos[2]));
       */
       
        //***IMPORTNT NOTEs: see if victim in evac + robot at least 30 cm away (once robot was 30cm away for an instant and victim in evac zone, then that victim will always count even when the robot re-enters 30 cm range)
        //  DON'T USE RESCUE KIT PROTO: CAN'T CHANGE COLOR AFTER RESCUED
     
     
        //Find evacuated victims: vicT and deliveryPts relative to rescue zone
        bool newVic = false;
        for(int i = 0; i < victims->getField("children")->getCount(); i++) {
          //victim already scored
          if(vicCounted[i]){
            if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("silver") == 0)
                  numSVic += 1;
                else 
                  numBVic += 1; 
            continue;  
          }       
          const double *vicT = victims->getField("children")->getMFNode(i)->getField("translation")->getSFVec3f();
          const double *rescueZoneT = rescueZone->getField("translation")->getSFVec3f();
          const double *rescueZoneR = rescueZone->getField("rotation")->getSFRotation();
          /*if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("black") == 0){
            supervisor->wwiSendText("P" + to_string(vicT[0]) + "," + to_string(vicT[2]) + "," + to_string(deliveryPts[0]) + "," + to_string(deliveryPts[1]) + "," + to_string(deliveryPts[2]) + "," + to_string(deliveryPts[3]) + "," + to_string(deliveryPts[4]) + "," + to_string(deliveryPts[5]));
            supervisor->wwiSendText("P" + to_string(inTriangle(vicT[0],vicT[2],deliveryPts[0],deliveryPts[1],deliveryPts[2],deliveryPts[3],deliveryPts[4],deliveryPts[5])));
          }*/
          double absVicT[3];
          absVicT[0] = rescueZoneT[0] + cos(rescueZoneR[3])*vicT[0] + sin(rescueZoneR[3])*vicT[2];
          absVicT[2] = rescueZoneT[2] + cos(rescueZoneR[3])*vicT[2] - sin(rescueZoneR[3])*vicT[0];
          //if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("black") == 0)
          //  supervisor->wwiSendText("P Black Vic:" + to_string(absVicT[0]) + "," + to_string(absVicT[2]));
          //see if victim meets conditions for rescue
          if(inTriangle(vicT[0],vicT[2],deliveryPts[0],deliveryPts[1],deliveryPts[2],deliveryPts[3],deliveryPts[4],deliveryPts[5])){
            if(findDistance(currPos,absVicT) > 0.3){//robot center 30 cm away
              //rescue victim
              if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("silver") == 0)
                numSVic += 1;
              else 
                numBVic += 1;
              //not working: can't double stack nodes
              //Field *vicColor = victims->getField("children")->getMFNode(i)->getField("children")->getMFNode(0)->getSFNode("appearance")->getField("baseColor")->setSFColor(rescuedColor);//change color of rescued victim
              vicCounted[i] = true;
              newVic = true;
            }
          }
        }   
        if(newVic)
          supervisor->wwiSendText("V" + to_string(numSVic) + " " + to_string(numBVic));
          
       //Find rescue kit
       if(rescueKitState != 2){
          const double *rescueZoneT = rescueZone->getField("translation")->getSFVec3f();
          const double *rescueZoneR = rescueZone->getField("rotation")->getSFRotation();
          double absKitT[3];
          absKitT[0] = rescueZoneT[0] + cos(rescueZoneR[3])*kitT[0] + sin(rescueZoneR[3])*kitT[2];
          absKitT[2] = rescueZoneT[2] + cos(rescueZoneR[3])*kitT[2] - sin(rescueZoneR[3])*kitT[0];
          //see if rescue kit meets conditions for rescue
          if(inTriangle(kitT[0],kitT[2],deliveryPts[0],deliveryPts[1],deliveryPts[2],deliveryPts[3],deliveryPts[4],deliveryPts[5])){
            if(findDistance(currPos,absKitT) > 0.3){//robot center 30 cm away
              rescueKitState = 1;
              //Set color like victim
            }
          }
        }   
        if(rescueKitState == 1){
          rescueKitState = 2;
          supervisor->wwiSendText("R");  
        }

        string msg = "";
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
