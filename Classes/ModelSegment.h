//
//  ModelSegment.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelSegment__
#define __SiderScroller__ModelSegment__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"


NS_RIY_BEGIN

class ModelSegment {

private:
    b2Vec2 groundVelocity;
    std::vector<int> obstructionRatios;
    float amplitude;
    float frequency;
    float verticalGap;
    float obstructionSpawnDelay;
    float duration;
    BOSS_TYPE bossType;
    std::unordered_map<int,int> obstructionGroupChoiceToRatio;
    std::vector<OBSTRUCTION_GROUP_CHOICE> listOfObstructionChoices;
    float bossInitialWaitingTime;
    float bossAdjustingHeightTime;
    float bossFiringTime;
    float bossAdjustingHeightVelocity;
    float bossChargeVelocity;
    float bossWaitingAfterChargingTime;

public:
    b2Vec2 getGroundVelocity();
    std::vector<int> getObstructionRatios();
    std::tuple<float,float> getAmplitudeAndFrequency();
    float getVerticalGap();
    float getObstructionSpawnDelay();
    BOSS_TYPE getBossType();
    float getDuration();
    float getAmplitude();
    float getFrequency();
    //Items that must be reset
    bool isBossSpawned;
    bool isBossDead;
    //End Items that must be reset

    float getBossInitialWaitingTime();
    float getBossAdjustingHeightTime();
    float getBossFiringTime();
    float getBossAdjustingHeightVelocity();
    float getBossChargeVelocity();
    float getBossWaitingAfterChargingTime();

    void init(b2Vec2 gv,float amp, float freq,float vgap, float spdel, float dur, BOSS_TYPE bt, std::unordered_map<int,int> ogcToRatio,float bossInitWaitTime,float bossAdjustHeightTime,float bossFireTime,float bossAdjustHeightVelocity, float bossChargeVel,float bossWaitingAfterTime);
    void reset();

    ModelSegment(){
        init(b2Vec2(-3.0,0), 0, 0, 3, 1.3, 8, NO_BOSS, {{PAIR_OBSTRUCTION__GROUP_CHOICE,1}},3,1,3,5,3.33,1);
    }

    ModelSegment(b2Vec2 gv,float amp, float freq,float vgap, float spdel, float dur, BOSS_TYPE bt, std::unordered_map<int,int> ogcToRatio,float bossInitWaitTime,float bossAdjustHeightTime,float bossFireTime,float bossAdjustHeightVelocity,float bossChargeVel,float bossWaitingAfterTime){
        init(gv,amp, freq, vgap, spdel, dur,bt, ogcToRatio,bossInitWaitTime,bossAdjustHeightTime,bossFireTime,bossAdjustHeightVelocity,bossChargeVel,bossWaitingAfterTime);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelSegment__) */
