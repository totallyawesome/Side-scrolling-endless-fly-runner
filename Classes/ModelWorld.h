//
//  ModelWorld.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __Farterson__ModelWorld__
#define __Farterson__ModelWorld__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include <unordered_set>
#include "AppMacros.h"
#include "GameMacros.h"
#include "GB2ShapeCache-x.h"
#include "ModelObstruction.h"
#include "ModelBadFood.h"
#include "ModelContactListener.h"
#include "ModelFlowerJet.h"
#include "ModelGround.h"
#include "ModelPlayer.h"
#include "ModelBoss.h"
#include "ModelProjectileHolder.h"
#include "ModelUpperKill.h"
#include "TouchEvent.h"
#include "ModelSegment.h"
#include "ModelOneUp.h"
#include "ModelGameStatus.h"
#include "ModelSkipLevel.h"
#include "ModelArt.h"
#include "ModelParticle.h"
NS_RIY_BEGIN

class ModelWorld {

private:

    int lastLevelIndex;
    ModelSegment segment;
    //Things that change often
    int currentSegmentIndex;
    int aSegmentIndex;
    int bSegmentIndex;
    float obSpawnDelay;
    float verticalGap;
    std::vector<int>obstructionRatios;
    float amplitude;
    float frequency;
    bool isMovingUpDown;
    float duration;
    float bossInitialWaitTime;
    float bossAdjustingHeightTime;
    float bossFiringTime;
    float bossWaitingAfterChargingTime;
    float bossDelayBetweenFiring;
    //end things that change often

    b2Vec2 ukPos;
    float badFoodRadius;
    float badFoodOnTreeFactor;
    int majorIndex;
    int absoluteIndex;

    int projectileId;
    float bossAdjustingHeightVelocity;// = 5.0; // 480 / 32 = 15. To do 15 in 3 seconds we need velocity of 5.
    float bossChargeVelocity;// = 3.33; // 320 / 32 = 10. To do 10 in 3 seconds we need velocity of 3.33
    BOSS_STATE bossState;
    float bossWaitStartTime;
    float bossLastFiredTime;
    bool isPizzaBossSpawned;
    int bossId;
    int badFoodId;
    bool wasLastObstructionDown;
    bool isGameStopped;

    float obstructionRangeFactor; // how much % of the obstruction length we use in calculating the y position of the obstruciton
    float obstructionStartX;
    float obstructionLastSpawnedTime;
    float normalVerticalGap;
    float lastDownObstructionHeight;
    float lastUpObstructionHeight;
    float playerAscensionsRate; //Rate at which player can fly
    float ceiling;
    int obstructionIndex;
    int flowerJetIndex;

    void init(bool isWatch);
    void simulatePhysics(float dt,bool isDemo);
    void spawnObstructions();
    void createSingleObstruction(bool isUp);
    void updateObstructionBehavior();

    void spawnParticles();
    void updateParticles();

    float particlesLastSpawnedAt;
    int particleIndex;
    int getParticleId();


    int getBadFoodId();
    int getFlowerJetId();
    int getObstructionId();
    int getBossId();
    int getProjectileId();
    int getScoreNodeId();
    BAD_FOOD_TYPE badFoodTypeForCurrentTotalTime();
    ModelObstruction* createObstruction(float x, float y,bool isUp);
    b2Vec2 obstructionDownPosition(float originalHalfHeight,float32 relativeMovementRate);

    void createObstructionRatiosList();
    ModelBadFood* createBadFood(BAD_FOOD_TYPE type,b2Vec2 position);
    ModelBoss* createBoss(BOSS_TYPE type, b2Vec2 position);
    ModelProjectileHolder* createProjectileHolder(b2Vec2 position);

    void updateBossBehavior();
    void updateJumpyFoods();
    void nextSegment();
    void segmentForIndex();
    void processLevelProgress();
    void segmentOnlyDelayChanging(int sliceIndex,float amp,float freq,float gv,float del,float dur,int totalSlices,float baseSpawnDelay,float minSpawnDelay);
public:
    float baseDuration;
    Size winSize;
    float totalTime;
    bool timeToSkipLevels;
    bool justWarped;
    b2World *world;
    ModelGround *ground;
    ModelUpperKill *upperKill;
    ModelPlayer *player;
    ModelContactListener contactListener;
    int score;
    b2Vec2 groundVelocity;

    std::unordered_map<int,ModelObstruction*>obstructions;
    std::unordered_set<int>newObstructionIds;
    std::unordered_set<int>removedObstructionIds;

    std::unordered_map<int,ModelBadFood*>badFoodNodes;
    std::unordered_set<int>newBadFoodIds;
    std::unordered_set<int>removedBadFoodIds;

    std::unordered_map<int,ModelFlowerJet*>flowerJets;
    std::unordered_set<int>newFlowerJets;
    std::unordered_set<int>removedFlowerJets;

    std::unordered_map<int,ModelBoss*>bosses;
    std::unordered_set<int>newBosses;
    std::unordered_set<int>removedBosses;

    std::unordered_map<int,ModelOneUp*>oneUps;
    std::unordered_set<int>newOneUps;
    std::unordered_set<int>removedOneUps;

    std::unordered_map<int,ModelSkipLevel*>skipLevels;
    std::unordered_set<int>newSkipLevels;
    std::unordered_set<int>removedSkipLevels;

    std::unordered_map<int,ModelParticle*>particles;
    std::unordered_set<int>newParticles;
    std::unordered_set<int>removedParticles;

    std::unordered_map<int,ModelProjectileHolder*>projectileHolders;

    std::vector<OBSTRUCTION_GROUP_CHOICE>listOfObstructionChoices;

    void tick(float dt,std::vector<TouchEvent> &touchEvents,bool isDemo);
    void processTaps(std::vector<TouchEvent> &touchEvents);
    void stopGame();
    float groundMovementRateWithRespectToObstructionMovementRate();
    int getAbsoluteIndex();
    bool isLastLevelReached();
    int getASegment();
    int getBSegment();

    ArtChunk currentArt;
    ArtChunk nextArt;
    void updateArtChunks();
    const std::string currentDateTime();

    ModelWorld(bool isWatch){
        init(isWatch);
    };


    ~ModelWorld()
    {
        delete upperKill;
        delete ground;
        delete player;

        for (auto pair : obstructions ) {
            ModelObstruction *obstruction = obstructions[pair.first];
            delete obstruction; //TODO Test this
        }

        for (auto pair : flowerJets) {
            ModelFlowerJet *flowerJet = flowerJets[pair.first];
            delete flowerJet;
        }

        for (auto pair : oneUps) {
            ModelOneUp *oe = oneUps[pair.first];
            delete oe;
        }

        for (auto pair : skipLevels) {
            ModelSkipLevel *sk = skipLevels[pair.first];
            delete sk;
        }

        for (auto pair : particles) {
            ModelParticle *pk = particles[pair.first];
            delete pk;
        }

        delete world;
    };
};

NS_RIY_END
#endif /* defined(__Farterson__ModelWorld__) */
