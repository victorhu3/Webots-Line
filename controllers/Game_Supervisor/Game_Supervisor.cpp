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

int checkpointCmp(Node* a, Node* b) {

   return a->getField("name")->getSFString() < b->getField("name")->getSFString(); 
}

double findDistance(const double a[], const double b[]) {

    return sqrt(pow(a[0]-b[0],2) + pow(a[2]-b[2], 2));
}

const double SOUTH[] = {-.577, .577, .577, 2.095};
const double EAST[] = {0, 0.707, 0.707, -3.1415};
const double NORTH[] {-0.5774, -0.5774, -0.5772, 2.0946};
const double WEST[] = {-1, 0, 0, 1.5712};

int main() {

    Supervisor *supervisor = new Supervisor();

    Node *robot = supervisor->getFromDef("player0");
    Node *checkpointGroup = supervisor->getFromDef("Checkpoints");

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

    vector<Node*>::iterator nextCheckpoint = checkpoints.begin();

    //store robot's pose at beginning of game
    double initialPos[3], initialRot[4];
    memcpy(initialPos, robot->getField("translation")->getSFVec3f(), sizeof(initialPos));
    memcpy(initialRot, robot->getField("rotation")->getSFRotation(), sizeof(initialRot)); 

    while(supervisor->step(TIME_STEP) != -1) {
         
        string msg;
        if((msg = supervisor->wwiReceiveText()).length() > 0) {
           
            if(msg == "L") {
                //Lack of progress button has been pushed
                cout << "Lack of Progress: ";

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
        }

        if(nextCheckpoint != checkpoints.end()) {

            double checkpointTilePos[3];
            memcpy(checkpointTilePos, (*nextCheckpoint)->getField("translation")->getSFVec3f(), sizeof(checkpointTilePos));
            checkpointTilePos[0] += .12;
            checkpointTilePos[2] += .12;
            if(findDistance(checkpointTilePos, robot->getField("translation")->getSFVec3f()) < .15) {
                
                cout << "Reached checkpoint" << endl;
                nextCheckpoint++;
            }
        }

    }

    delete supervisor;
    return 0;
}
