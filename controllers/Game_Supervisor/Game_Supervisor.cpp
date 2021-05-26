#include <webots/Supervisor.hpp>
#include <webots/Node.hpp>
using namespace webots;

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <cassert>
using namespace std;

#define TIME_STEP 32
#define NUMSCOREITEMS 10

const double SOUTH[] = {0, 1, 0, 0};
const double EAST[] = {0, 1, 0, 1.57};
const double NORTH[] {0, 1, 0, 3.15};
const double WEST[] = {0, 1, 0, 4.72};
int width = 0, height = 0;

typedef struct {
    char points;
    bool scored;
} tile;

int checkpointCmp(Node* a, Node* b) {

   return a->getField("name")->getSFString() < b->getField("name")->getSFString(); 
}

double findDistance(const double a[], const double b[]) {

    return sqrt(pow(a[0]-b[0],2) + pow(a[2]-b[2], 2));
}

int toGridNum(const double *coord) {
    if (coord[2] + 0.15 < 0 || coord[0] + 0.15 < 0)
        return -1;
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

void freeze(Supervisor *s) {
    while (s->step(TIME_STEP) != -1);
}

int main() {

    Supervisor *supervisor = new Supervisor();

    Node *robot = supervisor->getFromDef("player0");

    //supervisor->movieStartRecording("../worlds/results/movie.mp4", 480, 320, 0, 90, 1, true);
	
	
    //checkpoints
    Node *checkpointGroup = supervisor->getFromDef("Checkpoints");
    vector<int> path;
    vector<tile> scoringMap;

    ofstream fout("../../worlds/results/result.csv");
    string pathStr = supervisor->getFromDef("Tiles")->getField("children")->getMFNode(0)->getField("description")->getSFString();
    vector<int> scoringElem[NUMSCOREITEMS];
    string header = ",Intersections,Seesaws,Obstacles,Gaps,Ramp,Speed Bump,Checkpoints,Live Victim,Dead Victim,Rescue Kit,";
    enum scoreInd {intersection, seesaw, obstacle, gap, ramp, speedBump, checkpoint, liveVic, deadVic, kit};
    int tileNum = 0, ind = 0, evacEntrance = -1, longCat = 0;
    char itemToChar[NUMSCOREITEMS] = { '@', '!', '!', '@', '@', '#', '\0', '\0', '\0', '\0'};
    for (; pathStr[ind] != ','; ind++)
        height = height * 10 + pathStr[ind] - '0';
    ind++;
    for (; pathStr[ind] != ','; ind++)
        width = width * 10 + pathStr[ind] - '0';
    ind += 2;
    for (int i = 0; i < height * width; i++)
        scoringMap.push_back(tile{ '\0', false});
    for (; pathStr[ind] != ';'; ind++) {
        if (pathStr[ind] == ',') {
            path.push_back(tileNum);
            tileNum = 0;
        }
        else if (pathStr[ind] == '*')
            evacEntrance = tileNum;
        else
            tileNum = tileNum * 10 + pathStr[ind] - '0';
    }
    ind++;
    tileNum = 0;
    for (int i = 0; i < NUMSCOREITEMS; i++, ind++) {
        for (; pathStr[ind] != ';'; ind++) {
            if (pathStr[ind] == ',') {
                scoringElem[i].push_back(tileNum);
                scoringMap[tileNum].points = itemToChar[i];
                tileNum = 0;
            }
            else
                tileNum = tileNum * 10 + pathStr[ind] - '0';
        }
    }

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
    vector<int> checkpointDist;
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
        checkpointDist.push_back(stoi(tmp.substr(1, finalInd)));
        scoringElem[checkpoint].push_back(0);
    }
    supervisor->wwiSendText(checkpointVal);

    vector<Node*>::iterator nextCheckpoint = checkpoints.begin();
    int checkpointInd = 0;

    //store robot's pose at beginning of game
    double initialPos[3], initialRot[4];
    memcpy(initialPos, robot->getField("translation")->getSFVec3f(), sizeof(initialPos));
    memcpy(initialRot, robot->getField("rotation")->getSFRotation(), sizeof(initialRot)); 

    bool finished = false;
    int lastInd = 0;
    tileNum = 0, ind = 0;

    //victims
    //IMPORTANT: victims group currently needs to be first in children table; only supports up to 9 silver and 9 black victism currently (easy fix if need be)
    //int numSVic = 0, numBVic = 0;//victim color counts
    //double rescuedColor[3] = {0,1,1};
    Node *rescueZone = NULL;
    if (supervisor->getFromDef("RescueZone") != NULL)
        rescueZone = supervisor->getFromDef("RescueZone")->getField("children")->getMFNode(0);
    Node *victims = NULL;
    //find delivery zone rotation/location and rescue kit location    
    supervisor->wwiSendText("P1");    
    Node *deliveryZone = NULL, *rescueKit = NULL;
    if (rescueZone != 0) {
        victims = rescueZone->getProtoField("children")->getMFNode(0);
        for(int i = 1; i < rescueZone->getProtoField("children")->getCount(); i++) {
            string s1 = "deliveryzone";
            Node *n = rescueZone->getProtoField("children")->getMFNode(i);
            if(n->isProto() && n->getProtoField("name") != NULL)
                if(0 == s1.compare(n->getField("name")->getSFString()))//delivery zone node found
                    deliveryZone = n;
        }
        rescueKit = supervisor->getFromDef("RescueZone")->getField("children")->getMFNode(1);
    }
    const double *kitT = NULL;//relative to rescue zone
    const double *deliveryRot;
    double deliveryT[3];
    //find delivery zone triangle (4 possible rotations)
    float deliveryPts[6] = {0};
    int level = 0, kitLevel = 1;
    //supervisor->wwiSendText("V" + to_string(deliveryRot[3]));)
    if (rescueZone != 0) {
        kitT = rescueKit->getField("translation")->getSFVec3f();
        deliveryRot = rescueZone->getField("rotation")->getSFRotation();
        memcpy(deliveryT, deliveryZone->getField("translation")->getSFVec3f(), sizeof(deliveryT));
        if(fabs(deliveryRot[3] - 0) < 0.1){
            float pts[6] = {0.15,0.15,-0.15,0.15,-0.15,-0.15};
            for(int i = 0; i < 6; i++)
            deliveryPts[i] = pts[i];
        }
        else if(fabs(fabs(deliveryRot[3]) - 3.14) < 0.1){
            float pts[6] = {0.15,0.15,0.15,-0.15,-0.15,-0.15};
            for(int i = 0; i < 6; i++)
            deliveryPts[i] = pts[i];
            deliveryT[0] *= -1;
            deliveryT[2] *= -1;
        }
        else if((fabs(deliveryRot[3] - 1.57) < 0.1 && deliveryRot[1] == 1) || (fabs(deliveryRot[3] + 1.57) < 0.1 && deliveryRot[1] == -1)){
            float pts[6] = {-0.15,0.15,0.15,0.15,0.15,-0.15};
            for(int i = 0; i < 6; i++)
            deliveryPts[i] = pts[i];
            double tmp = deliveryT[0];
            deliveryT[0] = deliveryT[2];
            deliveryT[2] = tmp * -1;
        }
        else{
            float pts[6] = {-0.15,0.15,-0.15,-0.15,0.15,-0.15};
            for(int i = 0; i < 6; i++)
            deliveryPts[i] = pts[i];
            double tmp = deliveryT[0];
            deliveryT[0] = deliveryT[2] * -1;
            deliveryT[2] = tmp;
        }
        //add translation
        const double* zoneT = rescueZone->getField("translation")->getSFVec3f();
        for(int i = 0; i < 6; i+=2){
            deliveryPts[i] += deliveryT[0] + zoneT[0];//x translation
            deliveryPts[i+1] += deliveryT[2] + zoneT[2];//z translation
        }  
        if (rescueZone->getField("advancedZone")->getSFBool())
            level = 1;
        supervisor->wwiSendText("I" + to_string(level));
        if (findDistance(robot->getField("translation")->getSFVec3f(), kitT) < 0.2)
            kitLevel = 0;
    }

    bool vicCounted[20] = {0};
    int rescueKitState = 0;//0: not rescued; 1: rescued, not recorded; 2: rescued, recorded
    int liveVicNum = 0;

    bool autoScore = true;
    string scoringSetting = supervisor->getFromDef("GameSupervisor")->getField("description")->getSFString();
    string manual = "MANUAL";
    for (int i = 0; i < (int)scoringSetting.size(); i++)
        if (scoringSetting[i] >= 'a' && scoringSetting[i] <= 'z')
            scoringSetting[i] += 'A' - 'a';
    if (manual.compare(scoringSetting) == 0)
        autoScore = false;

    while(supervisor->step(TIME_STEP) != -1) {
        //numSVic = 0;
        //numBVic = 0;
        //get scoop info
        double currPos[3], currRot[4];// relScoopPos[3], scoopPos[3];
        bool newVic = false;
        const double *vicT;
        const double *rescueZoneT;
        const double *rescueZoneR;
        double absVicT[3];
        if (autoScore && rescueZone != 0) {
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
            for(int i = 0; i < victims->getField("children")->getCount(); i++) {
                //victim already scored
                if(vicCounted[i])
                    continue;  
                vicT = victims->getField("children")->getMFNode(i)->getField("translation")->getSFVec3f();
                rescueZoneT = rescueZone->getProtoField("translation")->getSFVec3f();
                rescueZoneR = rescueZone->getProtoField("rotation")->getSFRotation();
                /*if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("black") == 0){
                    supervisor->wwiSendText("P" + to_string(vicT[0]) + "," + to_string(vicT[2]) + "," + to_string(deliveryPts[0]) + "," + to_string(deliveryPts[1]) + "," + to_string(deliveryPts[2]) + "," + to_string(deliveryPts[3]) + "," + to_string(deliveryPts[4]) + "," + to_string(deliveryPts[5]));
                    supervisor->wwiSendText("P" + to_string(inTriangle(vicT[0],vicT[2],deliveryPts[0],deliveryPts[1],deliveryPts[2],deliveryPts[3],deliveryPts[4],deliveryPts[5])));
                }*/
                absVicT[0] = rescueZoneT[0] + cos(rescueZoneR[3])*vicT[0] + sin(rescueZoneR[3])*vicT[2];
                absVicT[2] = rescueZoneT[2] + cos(rescueZoneR[3])*vicT[2] - sin(rescueZoneR[3])*vicT[0];
                //if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("black") == 0)
                //  supervisor->wwiSendText("P Black Vic:" + to_string(absVicT[0]) + "," + to_string(absVicT[2]));
                //see if victim meets conditions for rescue
                if(inTriangle(absVicT[0],absVicT[2],deliveryPts[0],deliveryPts[1],deliveryPts[2],deliveryPts[3],deliveryPts[4],deliveryPts[5])){
                    if(findDistance(currPos,absVicT) > 0.3){//robot center 30 cm away
                        //rescue victim
                        if(victims->getField("children")->getMFNode(i)->getField("description")->getSFString().compare("silver") == 0)
                            scoringElem[liveVic][liveVicNum++]++;
                        else 
                            scoringElem[deadVic][0]++;
                        //not working: can't double stack nodes
                        //Field *vicColor = victims->getField("children")->getMFNode(i)->getField("children")->getMFNode(0)->getSFNode("appearance")->getField("baseColor")->setSFColor(rescuedColor);//change color of rescued victim
                        vicCounted[i] = true;
                        newVic = true;
                    }
                }
            }   
            if(newVic)
                supervisor->wwiSendText("V" + to_string(scoringElem[liveVic][0] + scoringElem[liveVic][1]) + " " + to_string(scoringElem[deadVic][0]));
            
            //Find rescue kit
            if(rescueKitState != 2){
                kitT = rescueKit->getField("translation")->getSFVec3f();
                /*rescueZoneT = rescueZone->getProtoField("translation")->getSFVec3f();
                rescueZoneR = rescueZone->getProtoField("rotation")->getSFRotation();
                double absKitT[3];
                absKitT[0] = rescueZoneT[0] + cos(rescueZoneR[3])*kitT[0] + sin(rescueZoneR[3])*kitT[2];
                absKitT[2] = rescueZoneT[2] + cos(rescueZoneR[3])*kitT[2] - sin(rescueZoneR[3])*kitT[0];*/
                //see if rescue kit meets conditions for rescue
                if(inTriangle(kitT[0],kitT[2],deliveryPts[0],deliveryPts[1],deliveryPts[2],deliveryPts[3],deliveryPts[4],deliveryPts[5])){
                    if(findDistance(currPos,kitT) > 0.3){//robot center 30 cm away
                        rescueKitState = 1;
                    }
                }
            }   
            if(rescueKitState == 1){
                rescueKitState = 2;
                supervisor->wwiSendText("R" + to_string(kitLevel));
                scoringElem[kit][0] = 1;
            }
        }

        string msg = "";
        tileNum = toGridNum(robot->getField("translation")->getSFVec3f());
        if (autoScore && tileNum != path[ind]) {
            if (tileNum == path[ind + 1]) {
                string tmp = "S";
                supervisor->wwiSendText(tmp + scoringMap[path[ind]].points);
                if (scoringMap[path[ind]].points != '\0')
                    scoringMap[path[ind]].scored = true;
                ind++;
            }
            else if (path[ind] != evacEntrance) {
                msg = "L";
                supervisor->wwiSendText(msg);
                int numSeesaw = supervisor->getFromDef("Seesaw")->getField("children")->getCount();
                Field *seesawGroup = supervisor->getFromDef("Seesaw")->getField("children");
                float initTilt = 0.3489;
                for (int i = 0; i < numSeesaw; i++)
                    seesawGroup->getMFNode(i)->getField("tilt")->setSFFloat(initTilt);
            }
        }
        if (autoScore && ind == (int)path.size() - 1) {
            int initTime = supervisor->getTime();
            while(supervisor->step(TIME_STEP) != -1)
                if (supervisor->getTime() - initTime >= 5)
                    break;
            if (toGridNum(robot->getField("translation")->getSFVec3f()) == path[path.size() - 1])
                finished = true;
        }
        if (autoScore && (finished || supervisor->getTime() > 480)) {
            cout << "Completed run!" << endl;
            fout << robot->getField("name")->getSFString() << header << endl;
            for (int i = 0; i < NUMSCOREITEMS; i++)
                if ((int)scoringElem[i].size() > longCat)
                    longCat = scoringElem[i].size();
            for (int i = 0; i < longCat; i++) {
                fout << '#' << i + 1 << ',';
                for (int j = 0; j < NUMSCOREITEMS; j++) {
                    if (i < (int)scoringElem[j].size() && j < checkpoint)
                        fout << scoringMap[scoringElem[j][i]].scored << ',';
                    else if (j == checkpoint) {
                        if (i < checkpointInd)
                            fout << checkpointDist[i] << " tiles - " << scoringElem[j][i] << " LOPs";
                        else if (i < (int)checkpoints.size())
                            fout << "Did not reach";
                        fout << ",";
                    }
                    else if (i < (int)scoringElem[j].size() && j > checkpoint)
                        fout << scoringElem[j][i] << ',';
                    else
                        fout << ',';
                }
                fout << endl;
            }
            fout << endl;
            supervisor->wwiSendText("F");
            supervisor->step(TIME_STEP);
            while (!((msg = supervisor->wwiReceiveText()).length() > 0));
            supervisor->step(TIME_STEP);
            string tmp;
            supervisor->step(TIME_STEP);
            fout << "Total Lack of Progresses,";
            tmp = "";
            ind = 0;
            for (; msg[ind] != ','; ind++)
                tmp += msg[ind];
            fout << tmp << endl;
            ind++;
            fout << "Evac Lack of Progresses,";
            tmp = "";
            for (; msg[ind] != ','; ind++)
                tmp += msg[ind];
            fout << tmp << endl;
            ind++;
            fout << "Line Tracing Score,";
            tmp = "";
            for (; msg[ind] != ','; ind++)
                tmp += msg[ind];
            fout << tmp << endl;
            ind++;
            fout << "Exit Bonus,";
            tmp = "";
            for (; msg[ind] != ','; ind++)
                tmp += msg[ind];
            fout << tmp << endl;
            ind++;
            fout << "Evacuation Zone Multiplier,";
            tmp = "";
            for (; msg[ind] != ','; ind++)
                tmp += msg[ind];
            fout << tmp << ",Level " << level + 1 << " Rescue Zone,Level " << kitLevel + 1 << " Rescue Kit" << endl;
            ind++;
            fout << "Total Score,";
            tmp = "";
            for (; msg[ind] != ','; ind++)
                tmp += msg[ind];
            fout << tmp << endl;
            fout << "end" << endl;

            cout << "score: " << tmp << endl;
            cout << "time: " << supervisor->getTime() << endl;

            supervisor->step(TIME_STEP);

             //supervisor->movieStopRecording();
    
            //while(!supervisor->movieIsReady()) 
                //if(supervisor->movieFailed()) 
                    //supervisor->simulationQuit(1);

            supervisor->simulationQuit(0);

            break;
        }
        if(msg.length() > 0 || (msg = supervisor->wwiReceiveText()).length() > 0) {
           
            if(msg == "L") {
                //Lack of progress button has been pushed
                cout << "Lack of Progress: " << endl;

                if (checkpointInd < (int)scoringElem[checkpoint].size()) {
                    ind = lastInd;
                    scoringElem[checkpoint][checkpointInd]++;
                    if (autoScore && scoringElem[checkpoint][checkpointInd] == 5) { //skip to next checkpoint after 5 LOP
                        cout << "Skipped checkpoint" << endl;
                        nextCheckpoint++;
                        checkpointInd++;
                        if (nextCheckpoint - checkpoints.begin() - 1 == preEvac)
                            supervisor->wwiSendText("C*");
                        else if (nextCheckpoint - checkpoints.begin() - 2 == preEvac)
                            supervisor->wwiSendText("C-");
                        else
                            supervisor->wwiSendText("C");
                        int tmp = toGridNum((*(nextCheckpoint- 1))->getField("translation")->getSFVec3f());
                        for (; path[ind] != tmp; ind++);
                        lastInd = ind;
                    }
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
                            checkpointTilePos[2] -= .12;
                        }
                        else if(dir == "E") {
                            robot->getField("rotation")->setSFRotation(EAST);
                            checkpointTilePos[0] += .12;
                        }
                        else if(dir == "S") {
                            robot->getField("rotation")->setSFRotation(SOUTH);
                            checkpointTilePos[2] += .12;
                        }
                        else if(dir == "W") {
                            robot->getField("rotation")->setSFRotation(WEST);
                            checkpointTilePos[0] -= .12;
                        }

                        robot->getField("translation")->setSFVec3f(checkpointTilePos);
                    }
                }
            }
        }

        if(nextCheckpoint != checkpoints.end()) {

            double checkpointTilePos[3];
            memcpy(checkpointTilePos, (*nextCheckpoint)->getField("translation")->getSFVec3f(), sizeof(checkpointTilePos));
            checkpointTilePos[0] += .12;
            checkpointTilePos[2] += .12;
            if(findDistance(checkpointTilePos, robot->getField("translation")->getSFVec3f()) < .15) {
                
                cout << "Reached checkpoint" << endl;
                nextCheckpoint++;
                checkpointInd++;
                
                if (nextCheckpoint - checkpoints.begin() - 1 == preEvac)
                    supervisor->wwiSendText("C*" + to_string(checkpointInd - 1) + to_string((int)autoScore));
                else if (nextCheckpoint - checkpoints.begin() - 2 == preEvac)
                    supervisor->wwiSendText("C-" + to_string(checkpointInd - 1) + to_string((int)autoScore));
                else
                    supervisor->wwiSendText("C" + to_string(checkpointInd - 1) + to_string((int)autoScore));
                lastInd = ind;
            }
        }
    }

    delete supervisor;
    return 0;
}
