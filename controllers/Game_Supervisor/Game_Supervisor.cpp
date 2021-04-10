#include <webots/Supervisor.hpp>
using namespace webots;

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>
#include <algorithm>
#include <math.h>
using namespace std;

#define TIME_STEP 32
#define NUMSCOREITEMS 7

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

int main() {

    Supervisor *supervisor = new Supervisor();

    Node *robot = supervisor->getFromDef("player0");
    Node *checkpointGroup = supervisor->getFromDef("Checkpoints");
    vector<int> path;
    vector<tile> scoringMap;

    ofstream fout("../../worlds/results/result.csv");
    string pathStr = supervisor->getFromDef("Tiles")->getField("children")->getMFNode(0)->getField("description")->getSFString();
    vector<int> scoringElem[NUMSCOREITEMS];
    string header = ",Intersections, Seesaws, Obstacles, Gaps, Ramp, Speed Bump, Checkpoints";
    enum scoreInd {intersection, seesaw, obstacle, gap, ramp, speedBump, checkpoint, liveVic, deadVic, rescueKit, evacLOP};
    int tileNum = 0, ind = 0, evacEntrance = -1, longCat = 0;
    char itemToChar[NUMSCOREITEMS] = { '@', '!', '!', '@', '@', '#', '\0'};
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

    while(supervisor->step(TIME_STEP) != -1) {
        string msg = "";
        tileNum = toGridNum(robot->getField("translation")->getSFVec3f());
        if (tileNum != path[ind]) {
            if (tileNum == path[ind + 1]) {
                string tmp = "S";
                supervisor->wwiSendText(tmp + scoringMap[path[ind]].points);
                if (scoringMap[path[ind]].points != '\0')
                    scoringMap[path[ind]].scored = true;
                ind++;
            }
            else if (path[ind] != evacEntrance) {
                ind = lastInd;
                msg = "L";
                supervisor->wwiSendText(msg);
                scoringElem[checkpoint][checkpointInd]++;
                if (scoringElem[checkpoint][checkpointInd] == 5) { //skip to next checkpoint after 5 LOP
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
            }
        }
        if (ind == (int)path.size() - 1) {
            int initTime = supervisor->getTime();
            while(supervisor->step(TIME_STEP) != -1)
                if (supervisor->getTime() - initTime >= 5)
                    break;
            if (toGridNum(robot->getField("translation")->getSFVec3f()) == path[path.size() - 1])
                finished = true;
        }
        if (finished || supervisor->getTime() > 10) {
            cout << "Completed run!" << endl;
            fout << robot->getField("name")->getSFString() << header << endl;
            for (int i = 0; i < NUMSCOREITEMS; i++)
                if ((int)scoringElem[i].size() > longCat)
                    longCat = scoringElem[i].size();
            for (int i = 0; i < longCat; i++) {
                fout << '#' << i + 1 << ',';
                for (int j = 0; j < NUMSCOREITEMS; j++) {
                    if (i < (int)scoringElem[j].size() && j != checkpoint)
                        fout << scoringMap[scoringElem[j][i]].scored << ',';
                    else if (j == checkpoint) {
                        if (i < checkpointInd)
                            fout << checkpointDist[i] << " tiles - " << scoringElem[j][i] << " LOPs,";
                        else
                            fout << "Did not reach,";
                    }
                    else
                        fout << ',';
                }
                fout << endl;
            }
            fout << endl;
            supervisor->wwiSendText("F");
            while (!((msg = supervisor->wwiReceiveText()).length() > 0));
            string tmp;
            
            fout << "Line Tracing Score,";
            tmp = "";
            ind = 0;
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
            fout << tmp << endl;
            ind++;
            fout << "Total Score:,";
            tmp = "";
            for (; msg[ind] != ','; ind++)
                tmp += msg[ind];
            fout << tmp << endl;
            break;
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
                    supervisor->wwiSendText("C*" + to_string(checkpointInd - 1));
                else if (nextCheckpoint - checkpoints.begin() - 2 == preEvac)
                    supervisor->wwiSendText("C-" + to_string(checkpointInd - 1));
                else
                    supervisor->wwiSendText("C" + to_string(checkpointInd - 1));
                lastInd = ind;
            }
        }
    }

    delete supervisor;
    return 0;
}
