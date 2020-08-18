//
//  ModelSegment.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelSegment.h"

NS_RIY_BEGIN

void ModelSegment::init(b2Vec2 gv,float amp, float freq,float vgap, float spdel, float dur, BOSS_TYPE bt, std::unordered_map<int,int> ogcToRatio,float bossInitWaitTime,float bossAdjustHeightTime,float bossFireTime,float bossAdjustHeightVelocity, float bossChargeVel,float bossWaitingAfterTime){

    bossInitialWaitingTime = bossInitWaitTime;
    bossAdjustingHeightTime = bossAdjustHeightTime;
    bossFiringTime = bossFireTime;
    bossAdjustingHeightVelocity = bossAdjustHeightVelocity;
    bossChargeVelocity = bossChargeVel;
    bossWaitingAfterChargingTime = bossWaitingAfterTime;

    groundVelocity = gv;
    for (int obsInt = IGNORED_FIRST_OBSTRUCTION_GROUND_CHOICE + 1; obsInt!=IGNORED_OBSTRUCTION_GROUP_CHOICE; obsInt++){
        OBSTRUCTION_GROUP_CHOICE choice = static_cast<OBSTRUCTION_GROUP_CHOICE>(obsInt);
        listOfObstructionChoices.push_back(choice);
    }

    obstructionGroupChoiceToRatio = ogcToRatio;
    amplitude = amp;
    frequency = freq;
    verticalGap = vgap;
    obstructionSpawnDelay = spdel;
    duration = dur;
    bossType = bt;

    if (bt != NO_BOSS) {
        //
        if (dur!=1) {
            dur = 1;
        }
    }

    unsigned long size = listOfObstructionChoices.size();
    for (unsigned long i = 0; i<size; i++) {
        OBSTRUCTION_GROUP_CHOICE choice = listOfObstructionChoices[i];

        std::unordered_map<int,int>::iterator it = obstructionGroupChoiceToRatio.find(choice);
        int value = 0;
        if(it != obstructionGroupChoiceToRatio.end())
        {
            //element found;
            value = it->second;
        }

        obstructionRatios.push_back(value);
    }

    isBossSpawned = false;
    isBossDead = false;
}

void ModelSegment::reset(){
    isBossSpawned = false;
    isBossDead = false;
}

b2Vec2 ModelSegment::getGroundVelocity(){
    return groundVelocity;
}

std::vector<int> ModelSegment::getObstructionRatios(){
    return obstructionRatios;
}

std::tuple<float,float> ModelSegment::getAmplitudeAndFrequency(){
    return std::make_tuple(amplitude,frequency);
}


float ModelSegment::getVerticalGap(){
    return verticalGap;
}

float ModelSegment::getObstructionSpawnDelay(){
    return obstructionSpawnDelay;
}

BOSS_TYPE ModelSegment::getBossType(){
    return bossType;
}

float ModelSegment::getDuration(){
    return duration;
}

float ModelSegment::getAmplitude(){
    return amplitude;
}

float ModelSegment::getFrequency(){
    return frequency;
}

float ModelSegment::getBossInitialWaitingTime(){
    return bossInitialWaitingTime;
}

float ModelSegment::getBossAdjustingHeightTime(){
    return bossAdjustingHeightTime;
}

float ModelSegment::getBossFiringTime(){
    return bossFiringTime;
}

float ModelSegment::getBossChargeVelocity(){
    return bossChargeVelocity;
}

float ModelSegment::getBossAdjustingHeightVelocity(){
    return bossAdjustingHeightVelocity;
}

float ModelSegment::getBossWaitingAfterChargingTime(){
    return bossWaitingAfterChargingTime;
}

NS_RIY_END
