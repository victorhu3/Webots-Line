#!/bin/bash

NUM_OF_TEAMS=6

cd auto_input
CWD=$PWD

for ((i=2; i<=NUM_OF_TEAMS; i++))
do
    cd $CWD

    rm -r team$i
    cp -r team1 team$i
    
    cd team$i/controllers
    mv team1 team$i
    cd team$i
    mv team1.cpp team$i.cpp

    timestep=$((32*i))
    sed -i "s/timeStep = 32/timeStep = $timestep/g" team$i.cpp

done