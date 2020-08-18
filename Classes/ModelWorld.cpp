//
//  ModelWorld.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelWorld.h"

NS_RIY_BEGIN

void ModelWorld::init(bool isWatch){

    baseDuration = 10;
    particlesLastSpawnedAt = 0.0f;
    particleIndex = 0;
    justWarped = false;
    lastLevelIndex = 1002;
    timeToSkipLevels = false;
    bossDelayBetweenFiring = 1;
    badFoodOnTreeFactor = 2.5;
    score = 0;
    projectileId = 0;
    bossLastFiredTime = 0.0;
    GB2ShapeCache::sharedGB2ShapeCache()->addShapesWithFile("res/shapes.plist");
    obstructionRangeFactor = 1.0;
    obstructionStartX = 12.0;
    isGameStopped = false;
    obstructionIndex = 0;
    flowerJetIndex = 0;
    bossId = 0;
    badFoodId = 0;
    totalTime = 0.0f;
    obstructionLastSpawnedTime = 0.0f;
    normalVerticalGap = 3.0; //For now we consider this fixed. Later on keep this as the max
    lastDownObstructionHeight = 0.0;
    lastUpObstructionHeight = 0.0;
    playerAscensionsRate = 5.6;
    ceiling = 10.0f;

    //Creating world
    struct b2Vec2 gravity;
    gravity.Set(0.0f, -32.0f); /// slow value is -5.0
    world = new b2World(gravity);
    world->SetAllowSleeping(true);
    world->SetContinuousPhysics(true);
    world->SetContactListener(&contactListener);

    ModelBadFood *badFood = new ModelBadFood(BAD_PIZZA_FOOD_TYPE,b2Vec2_zero,world,-1);
    badFoodRadius = badFood->radius;
    delete badFood;

    //Creating ground
    ground = new ModelGround(world);

    //Creating ceiling that will kill player on collision.
    if (isWatch) {
        //Lower ceiling
        ukPos = b2Vec2(0.0, 13.5);
    }else{
        //Regular ceiling
        ukPos = b2Vec2(0.0, 16.0);
    }
    upperKill = new ModelUpperKill(world,ukPos);

    //Create player
    player = new ModelPlayer(world,ground->originalHalfHeight);

    if (!GameStatus::savePoints.empty()) {

        absoluteIndex = GameStatus::savePoints.back().absoluteIndex;
        majorIndex = GameStatus::savePoints.back().majorIndex;
        currentSegmentIndex = GameStatus::savePoints.back().segmentIndex;
        aSegmentIndex = GameStatus::savePoints.back().aSegmentIndex;
        bSegmentIndex = GameStatus::savePoints.back().bSegmentIndex;

        if (GameStatus::savePoints.back().isWarp) {
            score = GameStatus::savePoints.back().score;
            float playerPosX = GameStatus::savePoints.back().playerPosX;
            float playerPosY = GameStatus::savePoints.back().playerPosY;
            player->body->SetTransform(b2Vec2(playerPosX, playerPosY), 0);
            justWarped = true;
            CCLOG("Just warped levels");
        }

        GameStatus::savePoints.pop_back();
    }else{
        absoluteIndex = 0;
        majorIndex = 0;
        currentSegmentIndex = -1;
        aSegmentIndex = -1;
        bSegmentIndex = -1;
    }

    createObstructionRatiosList();
    nextSegment();
    updateArtChunks();

    CCLOG("Starting at at: a:%d m%d s%d a%d b%d",absoluteIndex,majorIndex,currentSegmentIndex,aSegmentIndex,bSegmentIndex);

    //Boss levels at 61,119,178,237,295...
    //TODO For testing we have put this in a loop. Remove it later. Used for skipping levels.
//    while(aSegmentIndex < lastLevelIndex -2){ // This will show the last level immediately after killing the boss. lastLevelIndex -2 is the trees before the boss
//        nextSegment();
//    }
//    while(absoluteIndex < 4){
//        nextSegment();
//    }

}

void ModelWorld::updateArtChunks(){
    currentArt = ModelArt::getInstance()->artChunkForASegmentIndex(aSegmentIndex);
    nextArt = ModelArt::getInstance()->artChunkForASegmentIndex(aSegmentIndex + 1);
}

void ModelWorld::createObstructionRatiosList(){
    for (int obsInt = IGNORED_FIRST_OBSTRUCTION_GROUND_CHOICE + 1; obsInt!=IGNORED_OBSTRUCTION_GROUP_CHOICE; obsInt++){
        OBSTRUCTION_GROUP_CHOICE choice = static_cast<OBSTRUCTION_GROUP_CHOICE>(obsInt);
        listOfObstructionChoices.push_back(choice);
    }
}

void ModelWorld::updateJumpyFoods(){
    for (auto pair : badFoodNodes){
        ModelBadFood *bf = pair.second;

        if (bf->isJumpy) {
            if (bf->timeSinceLastJump < totalTime - 1) {
                bf->body->ApplyLinearImpulse(b2Vec2(0, 16), bf->body->GetPosition(), true);
                bf->timeSinceLastJump = totalTime;
            }
        }
    }
}

void ModelWorld::simulatePhysics(float dt,bool isDemo){
    static double UPDATE_INTERVAL = 1.0f/60.0f;
    static double MAX_CYCLES_PER_FRAME = 5; //Reduced from 5 to make it smooth. with 2 sometimes frame crash bug
    static double timeAccumulator = 0;

    timeAccumulator += dt;
    if (timeAccumulator > (MAX_CYCLES_PER_FRAME * UPDATE_INTERVAL))
    {
        timeAccumulator = UPDATE_INTERVAL;
    }

    int32 velocityIterations = 30;
    int32 positionIterations = 30;
    while (timeAccumulator >= UPDATE_INTERVAL)
    {
        timeAccumulator -= UPDATE_INTERVAL;
        this->world->Step(UPDATE_INTERVAL,
                          velocityIterations, positionIterations);
        this->world->ClearForces();
    }

    //Other stuff I added that doesn't have to do with simulating physics, but rather with the game
    if (!isDemo) {
        totalTime+=dt;
    }

    //Remove obstructions that are off-screen.
    std::vector<int>idsToRemove;
    for (auto obstructionData : obstructions){
        ModelObstruction *obstruction = obstructionData.second;
        b2Vec2 position = obstruction->body->GetPosition();
        float halfWidth = obstruction->originalHalfWidth;
        if (position.x + halfWidth < 0.0f) {
            //Removing this obstruction from the world
            int obstructionId = obstructionData.first;
            idsToRemove.push_back(obstructionId);
        }
    }
    for (int obstructionId : idsToRemove){
        ModelObstruction *obstruction = obstructions[obstructionId];
        delete obstruction;
        this->obstructions.erase(obstructionId);
        this->removedObstructionIds.insert(obstructionId);
    }

    //Remove badfoods that have been made contact with flowerjets
    for (auto badFoodId: this->contactListener.badFoodIdsToRemove) {
        //
        this->removedBadFoodIds.insert(badFoodId);
        ModelBadFood* badFood = badFoodNodes[badFoodId];

        delete badFood;
        this->badFoodNodes.erase(badFoodId);

        if (GameStatus::savePoints.size() > 0) {
            ModelSavePoint savePoint (GameStatus::savePoints.back().absoluteIndex,GameStatus::savePoints.back().majorIndex,GameStatus::savePoints.back().segmentIndex,GameStatus::savePoints.back().aSegmentIndex,GameStatus::savePoints.back().bSegmentIndex);
            savePoint.isWarp = true;
            savePoint.playerPosX = player->body->GetPosition().x;
            savePoint.playerPosY = player->body->GetPosition().y;
            GameStatus::savePoints.push_back(savePoint);
        }

        GameStatus::birdsBlasted++;
    }
    contactListener.badFoodIdsToRemove.clear();

    //Remove bosses that have been made contact with flowerjets
    for (auto bossId: this->contactListener.bossIdHitWithFlowerJet) {

        ModelBoss* boss = bosses[bossId];
        boss->health -= 1; // Reduce by one for each hit

        if (boss->health <= 0) {
            delete boss;
            this->bosses.erase(bossId);
            this->removedBosses.insert(bossId);
            segment.isBossDead = true;
        }
    }
    contactListener.bossIdHitWithFlowerJet.clear();

    //Remove oneups that have been made contact with
    for (auto oneUpId: this->contactListener.oneUpsMadeContactWith) {
        //
        this->removedOneUps.insert(oneUpId);
        ModelOneUp* oneUp = oneUps[oneUpId];

        ModelSavePoint savePoint (oneUp->absoluteId,oneUp->majorId,oneUp->segmentId,oneUp->aSegment,oneUp->bSegment);
        savePoint.isWarp = true;
        savePoint.playerPosX = player->body->GetPosition().x;
        savePoint.playerPosY = player->body->GetPosition().y;
        GameStatus::savePoints.push_back(savePoint);

        CCLOG("Saving at: a:%d m%d s%d a%d b%d",oneUp->absoluteId,oneUp->majorId,oneUp->segmentId,oneUp->aSegment,oneUp->bSegment);

        delete oneUp;
        this->oneUps.erase(oneUpId);
    }
    contactListener.oneUpsMadeContactWith.clear();

    //Remove one ups that are offscreen
    std::vector<int>oneupnodeIdsToRemove;
    for (auto oneupdata : oneUps){
        ModelOneUp *oneup = oneupdata.second;
        b2Vec2 position = oneup->body->GetPosition();
        if (position.x < 0.0f) {
            //Removing this obstruction from the world
            int oneupid = oneupdata.first;
            this->removedOneUps.insert(oneupid);
            oneupnodeIdsToRemove.push_back(oneupid);
        }
    }
    for (int oneupid : oneupnodeIdsToRemove){
        ModelOneUp* oneup = oneUps[oneupid];
        delete oneup;
        this->oneUps.erase(oneupid);
    }

    //Remove flowerjets that have made contact with ground or boss or bad food
    for (auto flowerJetId: this->contactListener.flowerJetIdsMadeContactWith) {
        //
        this->removedFlowerJets.insert(flowerJetId);
        ModelFlowerJet* flowerJet = flowerJets[flowerJetId];
        delete flowerJet;
        this->flowerJets.erase(flowerJetId);
    }
    contactListener.flowerJetIdsMadeContactWith.clear();

    //Remove skiplevels that have made contact with player
    bool hitASkip = false;
    for (auto skipId: this->contactListener.skipsMadeContactWith) {
        //
        this->removedSkipLevels.insert(skipId);
        ModelSkipLevel* skipLevel = skipLevels[skipId];
        delete skipLevel;
        this->skipLevels.erase(skipId);

        hitASkip = true;
    }
    contactListener.skipsMadeContactWith.clear();
    if (hitASkip) {
        //
        timeToSkipLevels = true;
        int originAbsoluteIndex = absoluteIndex;
        int currentA = aSegmentIndex;
        int currentC = currentSegmentIndex;
        currentC = currentC + 1 - 1;

        while (aSegmentIndex < currentA + 2) {
            nextSegment();
        }
        CCLOG("Skipping from current:%d to:%d",currentC,currentSegmentIndex);
        ModelSavePoint savePoint (absoluteIndex,majorIndex,currentSegmentIndex,aSegmentIndex,bSegmentIndex);
        savePoint.isWarp = true;
        savePoint.score = score;
        savePoint.playerPosX = player->body->GetPosition().x;
        savePoint.playerPosY = player->body->GetPosition().y;
        savePoint.originAbsoluteIndex = originAbsoluteIndex;
        savePoint.originASegmentIndex = currentA;
        GameStatus::savePoints.push_back(savePoint);
    }

    //Remove skips that are offscreen
    std::vector<int>skipIdsToRemove;
    for (auto skipdata : skipLevels){
        ModelSkipLevel *skip = skipdata.second;
        b2Vec2 position = skip->body->GetPosition();
        if (position.x < 0.0f) {
            //Removing this obstruction from the world
            int skipid = skipdata.first;
            skipIdsToRemove.push_back(skipid);
            removedSkipLevels.insert(skipid);
        }
    }
    for (int skipid : skipIdsToRemove){
        ModelSkipLevel* skip = skipLevels[skipid];
        delete skip;
        this->skipLevels.erase(skipid);
    }


    //Remove badfoods that are offscreen on left (passed / missed).
    std::vector<int>badFoodIdsToRemove;
    for (auto badFoodData : badFoodNodes){
        ModelBadFood *badFood = badFoodData.second;
        b2Vec2 position = badFood->body->GetPosition();
        float radius = badFood->radius;
        if (position.x + radius < 0.0f) {
            //Removing this obstruction from the world
            int badFoodId = badFoodData.first;
            badFoodIdsToRemove.push_back(badFoodId);
        }
    }
    for (int badFoodId : badFoodIdsToRemove){
        ModelBadFood* badFoodNode = badFoodNodes[badFoodId];
        delete badFoodNode;
        this->badFoodNodes.erase(badFoodId);
        this->removedBadFoodIds.insert(badFoodId);
    }

    std::vector<int>projectilesToRemove;
    for (auto projData : projectileHolders){
        ModelProjectileHolder *proj = projData.second;
        b2Vec2 pos = proj->body->GetPosition();
        if (pos.x < -2.0) {
            int projId = projData.first;
            projectilesToRemove.push_back(projId);
        }
    }
    for (int projId : projectilesToRemove){
        ModelProjectileHolder* proj = projectileHolders[projId];
        delete proj;
        this->projectileHolders.erase(projId);
    }

    std::vector<int>particlesToRemove;
    for (auto parDa : particles){
        ModelParticle *particle = parDa.second;
        b2Vec2 pos = particle->body->GetPosition();
        if (pos.x < -2.0 || pos.y < 0) {
            int partId = parDa.first;
            particlesToRemove.push_back(partId);
        }
    }
    for (int partId : particlesToRemove){
        ModelParticle* particle = particles[partId];
        delete particle;
        this->particles.erase(partId);
        this->removedParticles.insert(partId);
    }

    b2Vec2 playerCurrentPosition = player->body->GetPosition();
    if (player->body->GetPosition().x != 3) {
        player->body->SetTransform(b2Vec2(3, playerCurrentPosition.y), player->body->GetAngle());
    }
}

void  ModelWorld::stopGame(){
    for (auto pair : obstructions){
        ModelObstruction *obstruction = pair.second;
        obstruction->body->SetLinearVelocity(b2Vec2(0, 0));
    }

    for (auto pair : bosses){
        ModelBoss *boss = pair.second;
        boss->body->SetLinearVelocity(b2Vec2(0, 0));
        bossState = BOSS_NON_STATE;
    }

    for (auto pair : oneUps){
        ModelOneUp *oneUp = pair.second;
        oneUp->body->SetLinearVelocity(b2Vec2(0, 0));
    }

    for (auto pair : skipLevels){
        ModelSkipLevel *skip = pair.second;
        skip->body->SetLinearVelocity(b2Vec2(0, 0));
    }

    isGameStopped = true;
}

void ModelWorld::tick(float dt,std::vector<TouchEvent> &touchEvents,bool isDemo){
    this->newObstructionIds.clear();
    this->newFlowerJets.clear();
    this->newBadFoodIds.clear();
    this->newBosses.clear();
    this->newOneUps.clear();
    this->newSkipLevels.clear();
    this->newParticles.clear();

    this->removedObstructionIds.clear();
    this->removedFlowerJets.clear();
    this->removedBadFoodIds.clear();
    this->removedBosses.clear();
    this->removedOneUps.clear();
    this->removedParticles.clear();

    this->simulatePhysics(dt,isDemo);

    if(!isGameStopped){
        spawnObstructions();

        this->processTaps(touchEvents);

        BOSS_TYPE bossType = segment.getBossType();
        if (bossType != NO_BOSS ) {
            if (!segment.isBossSpawned) {
                segment.isBossSpawned = true;
                ModelBoss *boss = createBoss(bossType, b2Vec2(12,7.5));
                boss->body->SetLinearVelocity(b2Vec2(0 - 1.0, 0));
                bossState = BOSS_ENTERING_STATE;
            }
        }
        this->updateJumpyFoods();
        this->updateObstructionBehavior();
        this->updateBossBehavior();

        //Apply wind to flowerjets and falling
        int fwindValue = 3;
        for (auto pair : flowerJets){
            ModelFlowerJet *flowerJet = pair.second;
            flowerJet->body->ApplyForceToCenter(b2Vec2(0-fwindValue,0), true);
        }

        if (totalTime > duration ) {
            if (segment.isBossSpawned) {
                if (segment.isBossDead) {
                    nextSegment();
                }
            }else{
                nextSegment();
            }
        }

        spawnParticles();
        updateParticles();
    }
}

void ModelWorld::spawnParticles(){
    if (totalTime - particlesLastSpawnedAt > currentArt.pSD) {
        particlesLastSpawnedAt = totalTime;
        float yOffset = (winSize.height / currentArt.noP)/PTM_RATIO;
        int num = arc4random()%currentArt.noP;
        float xOff = winSize.width*currentArt.xSK;
        for (int i=0; i< num; i++){
            int pId = getParticleId();
            float xR = ((arc4random()%480));
            ModelParticle* particle = new ModelParticle(b2Vec2((xOff +xR)/ PTM_RATIO, winSize.height/PTM_RATIO + yOffset*i), world, pId);
            this->particles[pId] = particle;
            this->newParticles.insert(pId);
        }
    }
}

void ModelWorld::updateParticles(){

    for (auto pID : particles){
        ModelParticle* particle = pID.second;
        float yPush = (arc4random()%currentArt.yPL)/100.0f;
        float xPush = (arc4random()%currentArt.xPL)/100.0f;
        particle->body->ApplyForceToCenter(b2Vec2(0 - 1 - xPush, 0 - 1 + yPush), true);
    }
}

void ModelWorld::segmentForIndex(){

    float baseVelocityX = -3.0f;
    float maxVelocityX = -3.5f;

    float baseAmplitude = 0.0;
    float maxAmplitude = 1.0;

    float baseFrequency = 20;
    float maxFrequency = 2.5;

    float baseObSpawnDelay = 1.3;
    float minObSpawnDelay = 0.43; //0.517, 0.43

    float baseVerticalGap = 3.0;
    float minVerticalGap = 2.5;

    int numberOfSlices = 10;
    float changeInSpawnDelay = (baseObSpawnDelay - minObSpawnDelay)/numberOfSlices;
    float changeInAmplitude = (maxAmplitude - baseAmplitude) / numberOfSlices;
    float changeInFrequency = (baseFrequency - maxFrequency) / numberOfSlices;
    float changeInVerticalGap = (baseVerticalGap - minVerticalGap) / numberOfSlices;
    float changeInVelocityX = (maxVelocityX - baseVelocityX) / numberOfSlices;

    int freqTimes = 0;
    int ampTimes = 0;
    int times = 0 ;
    int vertTimes = 0;
    int velTimes = 0;
    int refIndex = 0;
    switch (currentSegmentIndex%2) {
        case 0:
        {
            //Increasing freq, amp, del like ss::mm::hh
            refIndex = aSegmentIndex;
            CCLOG("Art should change");
            updateArtChunks();
            freqTimes = aSegmentIndex++;

            if (freqTimes > numberOfSlices) {
                ampTimes = freqTimes / numberOfSlices;

                freqTimes = freqTimes % numberOfSlices;
            }

            if (ampTimes > numberOfSlices) {

                times = ampTimes / numberOfSlices;
                ampTimes = ampTimes % numberOfSlices;
            }

            break;
        }
        case 1:
        {
            //Increasing freq, amp, del like ss::mm::hh
            refIndex = aSegmentIndex;
            ampTimes = bSegmentIndex++;

            if (ampTimes > numberOfSlices) {
                freqTimes = ampTimes / numberOfSlices;

                ampTimes = ampTimes % numberOfSlices;
            }

            if (freqTimes > numberOfSlices) {
                times = freqTimes / numberOfSlices;
                freqTimes = freqTimes % numberOfSlices;
            }

            break;
        }

        default:
            break;
    }

    if (times > numberOfSlices) {
        vertTimes = times / numberOfSlices;

        times = times % numberOfSlices;
    }

    if (vertTimes > numberOfSlices){
        velTimes = vertTimes / numberOfSlices;

        vertTimes = vertTimes % numberOfSlices;
    }

    if (velTimes > numberOfSlices) {
        velTimes = velTimes % numberOfSlices;
    }

    //Keep decreasing gap, then inc freq, amp, vert gap etc.
    float adjustedSpawnDelay = baseObSpawnDelay - times*changeInSpawnDelay;
    float adjustedFrequency = baseFrequency - freqTimes*changeInFrequency;
    float adjustedAmplitude = baseAmplitude + ampTimes*changeInAmplitude;
    float adjustedVerticalGap = baseVerticalGap - vertTimes*changeInVerticalGap;
    float adjustedVelocityX = baseVelocityX + velTimes*changeInVelocityX;
    float adjustedDuration = baseDuration;



    if (aSegmentIndex >= 1001) {
        adjustedSpawnDelay = 10000;
        adjustedDuration = 3;
    }

//    LONE_DOWN_OBSTRUCTION_GROUP_CHOICE,
//    LONE_UP_OBSTRUCTION_GROUP_CHOICE,
//    PAIR_OBSTRUCTION__GROUP_CHOICE,
//    BAD_FOOD_PAIR_OBSTRUCTION_GROUP_CHOICE,
//    SINGLE_BAD_FOOD_JUMPING_FROM_GROUND_AND_TREE_ABOVE_CHOICE,
//    PAIR_BAD_FOOD_WITHOUT_TREES_GROUP_CHOICE,

//    int refIn = refIndex % 100;
    int luogc = 1;
    int ldogc = 1;
    int pogc = 20;
    int bfpogc = 5;
    int sbfjfgatac = 5;
    int pbfwtgc = 2;

    std::unordered_map<int,int> ratio = {{LONE_UP_OBSTRUCTION_GROUP_CHOICE,luogc},{LONE_DOWN_OBSTRUCTION_GROUP_CHOICE,ldogc},{PAIR_OBSTRUCTION__GROUP_CHOICE,pogc},{BAD_FOOD_PAIR_OBSTRUCTION_GROUP_CHOICE,bfpogc},{SINGLE_BAD_FOOD_JUMPING_FROM_GROUND_AND_TREE_ABOVE_CHOICE,sbfjfgatac},{PAIR_BAD_FOOD_WITHOUT_TREES_GROUP_CHOICE,pbfwtgc}};
    segment = ModelSegment(b2Vec2(adjustedVelocityX, 0), adjustedAmplitude, adjustedFrequency, adjustedVerticalGap, adjustedSpawnDelay, adjustedDuration, NO_BOSS, ratio, 0, 0, 0, 0, 0, 0);
}

void ModelWorld::processLevelProgress(){

    absoluteIndex++;

    if (absoluteIndex%4 == 0 && absoluteIndex>0 && aSegmentIndex < lastLevelIndex - 2) { // Save level

        std::unordered_map<int,int> ratio = {{LONE_UP_OBSTRUCTION_GROUP_CHOICE,1},{LONE_DOWN_OBSTRUCTION_GROUP_CHOICE,1},{SAVE_GAME_OBSTRUCTION_GROUP_CHOICE,10}};
        segment = ModelSegment(segment.getGroundVelocity(), segment.getAmplitude(), segment.getFrequency(), segment.getVerticalGap(), segment.getObstructionSpawnDelay(), 100, NO_BOSS, ratio, 0, 0, 0, 0, 0, 0);
        CCLOG("Save level at ab:%d m:%d c:%d a:%d b:%d",absoluteIndex,majorIndex,currentSegmentIndex,aSegmentIndex,bSegmentIndex);

    }else{
        if (majorIndex%20 == 0 && majorIndex>0) {
            int rem = (aSegmentIndex + bSegmentIndex)%10;
            if (rem!=0) {
                currentSegmentIndex++;
                segmentForIndex();
            }else{ // Boss level
                majorIndex++;
                std::unordered_map<int,int> ratio = {{LONE_UP_OBSTRUCTION_GROUP_CHOICE,1},{LONE_DOWN_OBSTRUCTION_GROUP_CHOICE,1}};

                CCLOG("Boss level at ab:%d m:%d c:%d a:%d b:%d",absoluteIndex,majorIndex,currentSegmentIndex,aSegmentIndex,bSegmentIndex);

                float bossDefaultFiringDelay = 1;
                float bossInitWaitTime = 3;
                float bossAdjustHeightTime = 1;
                float bossFireTime = 3;
                float bossAdjustHeightVelocity = 5;
                float bossChargeVel = 3.33;
                float bossWaitingAfterTime = 1;

                float minBossInitWaitTime = 1;
                float maxBossFireTime = 10;
                float maxBossChargeVel = 6.66;
                float minBossDelayBetweenFiring = 0.7;

                float changeInBossFiringDelay = (bossDefaultFiringDelay - minBossDelayBetweenFiring)/1000;
                bossDelayBetweenFiring = bossDefaultFiringDelay - changeInBossFiringDelay*aSegmentIndex;

                float changeInInitWaitTime = (bossInitWaitTime - minBossInitWaitTime)/1000;
                bossInitWaitTime = bossInitWaitTime - changeInInitWaitTime*aSegmentIndex;

                float changeInBossFireTime = (maxBossFireTime - bossFireTime)/1000;
                bossFireTime = bossFireTime + changeInBossFireTime*aSegmentIndex;

                float changeInBossChargeVel = (maxBossChargeVel - bossChargeVel)/1000;
                bossChargeVel = bossChargeVel + changeInBossChargeVel*aSegmentIndex;

                segment = ModelSegment(segment.getGroundVelocity(), segment.getAmplitude(), segment.getFrequency(), segment.getVerticalGap(), segment.getObstructionSpawnDelay(), 0, BOSS_PIZZA_TYPE, ratio, bossInitWaitTime, bossAdjustHeightTime, bossFireTime, bossAdjustHeightVelocity, bossChargeVel, bossWaitingAfterTime);
            }
        }else if (majorIndex%5 == 0 && majorIndex>0 && aSegmentIndex<950){

            int rem = (aSegmentIndex + bSegmentIndex)%10;
            if (rem!=0) {
                currentSegmentIndex++;
                segmentForIndex();
            }else{
                majorIndex++;

                CCLOG("Skip level at ab:%d m:%d c:%d a:%d b:%d",absoluteIndex,majorIndex,currentSegmentIndex,aSegmentIndex,bSegmentIndex);

                //Put the skipping level segment.
                std::unordered_map<int,int> ratio = {{LONE_UP_OBSTRUCTION_GROUP_CHOICE,1},{LONE_DOWN_OBSTRUCTION_GROUP_CHOICE,1},{SKIP_GAME_OBSTRUCTION_GROUP_CHOICE,10}};
                segment = ModelSegment(segment.getGroundVelocity(), segment.getAmplitude(), segment.getFrequency(), segment.getVerticalGap(), segment.getObstructionSpawnDelay(), 100, NO_BOSS, ratio, 0, 0, 0, 0, 0, 0);
            }

        }else{
            majorIndex++;
            currentSegmentIndex++;
            segmentForIndex();
        }
    }
}

void ModelWorld::nextSegment(){
    totalTime = 0.0f;
    particlesLastSpawnedAt = 0.0f;
    obstructionLastSpawnedTime = 0.0f;

    processLevelProgress();

    groundVelocity = segment.getGroundVelocity();
    obSpawnDelay = segment.getObstructionSpawnDelay();
    verticalGap = segment.getVerticalGap();
    obstructionRatios = segment.getObstructionRatios();
    amplitude = segment.getAmplitude();
    frequency = segment.getFrequency();
    duration = segment.getDuration();
    isMovingUpDown = true;
    if (amplitude <=0 || frequency <=0) {
        isMovingUpDown = false;
    }

    bossInitialWaitTime = segment.getBossInitialWaitingTime();
    bossAdjustingHeightTime = segment.getBossAdjustingHeightTime();
    bossFiringTime = segment.getBossFiringTime();
    bossAdjustingHeightVelocity = segment.getBossAdjustingHeightVelocity();
    bossChargeVelocity = segment.getBossChargeVelocity();
    bossWaitingAfterChargingTime = segment.getBossWaitingAfterChargingTime();

    CCLOG("%s NS i:%d osd:%f gxv:%f a:%f f:%f vg:%f du:%f",currentDateTime().c_str(), currentSegmentIndex,obSpawnDelay,groundVelocity.x,amplitude,frequency,verticalGap,duration);
}

void ModelWorld::updateBossBehavior(){
    //Statemachine
    b2Vec2 playerPos = player->body->GetPosition();
    for (auto pair : bosses){
        ModelBoss *boss = pair.second;
        b2Vec2 bossPos = boss->body->GetPosition();
        //First enter into scene at extreme right
        if (bossState == BOSS_ENTERING_STATE) {
            // Calculate velcoity required for the boss to reach start point in x seconds.

            if (bossPos.x <= 9 - boss->radius) {
                boss->body->SetLinearVelocity(b2Vec2(0,0));
                bossState = BOSS_ENTERED_WAITING_STATE;
                bossWaitStartTime = totalTime;
            }
        }else if (bossState == BOSS_ENTERED_WAITING_STATE){
            //Wait one second.
            if (totalTime - bossWaitStartTime > bossInitialWaitTime) {
                bossState = BOSS_ADJUSTING_HEIGHT_STATE;
                bossWaitStartTime = totalTime;
            }
        }else if (bossState == BOSS_ADJUSTING_HEIGHT_STATE){
            // Adjust height to same height as player

            if (playerPos.y > bossPos.y) {
                boss->body->SetLinearVelocity(b2Vec2(0, bossAdjustingHeightVelocity));
            }else if(playerPos.y < bossPos.y && bossPos.y > ground->originalHalfHeight*2){
                boss->body->SetLinearVelocity(b2Vec2(0, 0 - bossAdjustingHeightVelocity));
            }else {
                boss->body->SetLinearVelocity(b2Vec2(0, 0));
            }

            if (totalTime - bossWaitStartTime > bossAdjustingHeightTime) {
                bossState = BOSS_CHARGING_STATE;
            }
        }else if (bossState == BOSS_CHARGING_STATE){
            // Then charge into player

            if (bossPos.x > boss->radius){
                boss->body->SetLinearVelocity(b2Vec2(0 - bossChargeVelocity, 0));
            }else{
                boss->body->SetLinearVelocity(b2Vec2(0, 0));
                bossWaitStartTime = totalTime;
                bossState = BOSS_WAITING_AFTER_CHARGING_STATE;
            }
        }else if (bossState == BOSS_WAITING_AFTER_CHARGING_STATE){
            if (totalTime - bossWaitStartTime > bossWaitingAfterChargingTime) {
                bossState = BOSS_RETREATING_STATE;
            }
        }else if (bossState == BOSS_RETREATING_STATE){
            // Retreat back to right -> Repeat random number of times
            boss->body->SetLinearVelocity(b2Vec2(bossChargeVelocity, 0));

            if (bossPos.x > 9 - boss->radius) {
                boss->body->SetLinearVelocity(b2Vec2(0, 0));

                int randomChoice = arc4random()%2;
                if (randomChoice == 0) {
                    bossState = BOSS_FIRING_STATE;
                }else {
                    bossState = BOSS_ADJUSTING_HEIGHT_STATE;
                }

                bossWaitStartTime = totalTime;
            }
        }else if (bossState == BOSS_FIRING_STATE){
            // Aim for player
            // Fire at player a few times -> Repeat random number of times

            if (totalTime - bossLastFiredTime > bossDelayBetweenFiring) {
                bossLastFiredTime = totalTime;
                ModelBadFood *badFood = createBadFood(badFoodTypeForCurrentTotalTime(), bossPos);
                badFood->hasStem = true;

                //Calculate position of player and velocity to cause impact in 1 second.
                b2Vec2 badVel = b2Vec2(playerPos.x - bossPos.x, playerPos.y - bossPos.y);

                ModelProjectileHolder *proj = createProjectileHolder(bossPos);
                b2RevoluteJointDef revoluteJointDef;
                revoluteJointDef.Initialize(proj->body, badFood->body, bossPos);
                world->CreateJoint(&revoluteJointDef);

                proj->body->SetLinearVelocity(badVel);
                badFood->rotation = 0;
            }

            if (totalTime - bossWaitStartTime > bossFiringTime) {
                // Repeat from adjust height.
                int randomChoice = arc4random()%2;
                if (randomChoice == 0) {
                    bossState = BOSS_FIRING_STATE;
                }else{
                    bossState = BOSS_ADJUSTING_HEIGHT_STATE;
                }
            }
        }
    }
}

void ModelWorld::updateObstructionBehavior(){
    for (auto pair : obstructions){
        ModelObstruction *obstruction = pair.second;
        obstruction->update();
    }
}

void ModelWorld::processTaps(std::vector<TouchEvent> &touchEvents){
    if (touchEvents.size() > 0){
        player->body->SetLinearVelocity(b2Vec2(0,0));
        player->body->ApplyLinearImpulse(b2Vec2(0, 7.0), player->body->GetPosition(), true); // slow value is 2.0
        touchEvents.clear();

        b2Vec2 pos = player->body->GetPosition();

        float ran = 0.01f;
        int nof = 4;
        float scale = 1.0f;
        //Create a set of flower jets here, with random x and y velocities
        for (int i = 0 ; i < nof; i++) {
            ModelFlowerJet *flower = new ModelFlowerJet(pos,world,getFlowerJetId());
            float xv = (0 - ran*((i+2)*(i+3)*(i+4)))/3;
            b2Vec2 vel = b2Vec2(xv,(xv));
            flower->body->ApplyLinearImpulse(vel, pos, true);
            flowerJets[flower->flowerJetId] = flower;
            flower->scale = scale;
            scale = scale*1.2;
            newFlowerJets.insert(flower->flowerJetId);
        }
    }
}



void ModelWorld::spawnObstructions(){

    if (segment.getBossType() == NO_BOSS) {
        if (totalTime - obstructionLastSpawnedTime > obSpawnDelay) {

            obstructionLastSpawnedTime = totalTime;
            unsigned long size = obstructionRatios.size();
            int total = 0;
            for (unsigned long i = 0; i<size; i++) {
                total += obstructionRatios[i];
            }

            int randomChoice = arc4random()%total;
            bool choiceFound = false;
            OBSTRUCTION_GROUP_CHOICE choice = IGNORED_FIRST_OBSTRUCTION_GROUND_CHOICE;

            int cumulativeRatio = 0;
            for (int i=0; i<size; i++) {
                cumulativeRatio += obstructionRatios[i];
                if (randomChoice < cumulativeRatio ) {
                    choice = listOfObstructionChoices[i];
                    choiceFound = true;
                    break;
                }
            }

            if (!choiceFound) {
                choice = PAIR_OBSTRUCTION__GROUP_CHOICE;
                CCLOG("Couldn't find right obstruction pair choice. Something is wrong, so using a default");
            }

            bool isThisObstructionHavingBadFood = false;
            bool isTheBadFoodOnTop = false;
            float badFoodRadius = 0;
            if (choice == BAD_FOOD_PAIR_OBSTRUCTION_GROUP_CHOICE){
                isThisObstructionHavingBadFood = true;
                int badFoodUp = arc4random()%2;
                if (badFoodUp) {
                    isTheBadFoodOnTop = true;
                }
            }

            float x = obstructionStartX;
            float y = 0.0;
            ModelObstruction *down = createObstruction(x, y,false);
            b2Vec2 newPos = obstructionDownPosition(down->originalHalfHeight, down->relativeObstructionMovementRate);
            float obsDownHeight = newPos.y;
            if ((isThisObstructionHavingBadFood) || choice == PAIR_BAD_FOOD_WITHOUT_TREES_GROUP_CHOICE){
                obsDownHeight = newPos.y - badFoodRadius*badFoodOnTreeFactor;
            }

            down->isUp = false;
            down->body->SetTransform(b2Vec2(newPos.x, obsDownHeight), 0);

            float groundAndDownOriginalHalfHeight = ground->groundHeight + down->originalHalfHeight;
            float upMinusVgapMinusDownOrgHeight = ukPos.y - 1 - verticalGap - down->originalHalfHeight;

            float maxUpPosition = min(groundAndDownOriginalHalfHeight,upMinusVgapMinusDownOrgHeight);
            float maxDownPosition = ground->groundHeight*1.1 - down->originalHalfHeight;
            float initialYVelocity;
            if (frequency <=0 || amplitude <=0) {
                initialYVelocity = 0;
            }else{
                initialYVelocity = (maxUpPosition - maxDownPosition)/frequency;
            }

            down->upperLimitY = amplitude * maxUpPosition;
            down->lowerLimitY = amplitude * maxDownPosition;
            b2Vec2 originalVelocity = down->body->GetLinearVelocity();
            b2Vec2 newVel = b2Vec2(originalVelocity.x,initialYVelocity);
            down->body->SetLinearVelocity(newVel);
            down->velocityY = initialYVelocity;
            down->isMovingUpDown = isMovingUpDown;


            ModelObstruction *up = createObstruction(x, y,true);
            float obstructionUpHeight = newPos.y + down->originalHalfHeight + verticalGap + up->originalHalfHeight;
            if ((isThisObstructionHavingBadFood) || choice == PAIR_BAD_FOOD_WITHOUT_TREES_GROUP_CHOICE){
                obstructionUpHeight = obstructionUpHeight + badFoodRadius*badFoodOnTreeFactor;
            }
            up->body->SetTransform(b2Vec2(newPos.x, obstructionUpHeight), 0);
            up->isUp = true;
            up->upperLimitY = down->upperLimitY + down->originalHalfHeight + verticalGap + up->originalHalfHeight;
            up->lowerLimitY = down->lowerLimitY + down->originalHalfHeight + verticalGap + up->originalHalfHeight;
            up->body->SetLinearVelocity(newVel);
            up->isMovingUpDown = isMovingUpDown;
            up->velocityY = initialYVelocity;

            if (choice == LONE_UP_OBSTRUCTION_GROUP_CHOICE) {
                down->makeInvisibleUncollidable();
            }else if (choice == LONE_DOWN_OBSTRUCTION_GROUP_CHOICE){
                up->makeInvisibleUncollidable();
            }

            if (choice == SKIP_GAME_OBSTRUCTION_GROUP_CHOICE) {

                int randomMisleading = arc4random()%2;
                b2Vec2 skipLevelPosition;
                if (randomMisleading == 0) {
                    //Put it where the lower should have been
                    skipLevelPosition = down->body->GetPosition();
                    skipLevelPosition.y = skipLevelPosition.y + down->originalHalfHeight/2;
                    down->makeInvisibleUncollidable();

                }else if(randomMisleading == 1){
                    skipLevelPosition = up->body->GetPosition();
                    skipLevelPosition.y = skipLevelPosition.y - up->originalHalfHeight/2;
                    up->makeInvisibleUncollidable();
                }

                if (skipLevelPosition.y < ground->groundHeight) {
                    skipLevelPosition.y = ground->groundHeight;
                }

                ModelSkipLevel *skipLevel = new ModelSkipLevel(skipLevelPosition,world,down->obstructionId);
                skipLevel->body->SetLinearVelocity(originalVelocity);
                skipLevels[skipLevel->skipLevelId] = skipLevel;
                newSkipLevels.insert(skipLevel->skipLevelId);

                nextSegment();
                CCLOG("Created skiplevel with ID:%d",skipLevel->skipLevelId);

            }else if (choice == SAVE_GAME_OBSTRUCTION_GROUP_CHOICE) {

                int randomMisleading = arc4random()%2;
                b2Vec2 oneUpPosition;
                if (randomMisleading == 0) {
                        //Put it where the lower should have been
                        oneUpPosition = down->body->GetPosition();
                        oneUpPosition.y = oneUpPosition.y + down->originalHalfHeight/2;
                        down->makeInvisibleUncollidable();

                }else if(randomMisleading == 1){
                        oneUpPosition = up->body->GetPosition();
                        oneUpPosition.y = oneUpPosition.y - up->originalHalfHeight/2;
                        up->makeInvisibleUncollidable();
                }

                if (oneUpPosition.y < ground->groundHeight) {
                    oneUpPosition.y = ground->groundHeight;
                }

                ModelOneUp *oneUp = new ModelOneUp(oneUpPosition,world,down->obstructionId,currentSegmentIndex,absoluteIndex,majorIndex,aSegmentIndex,bSegmentIndex);
                oneUp->body->SetLinearVelocity(originalVelocity);
                oneUps[oneUp->oneUpId] = oneUp;
                newOneUps.insert(oneUp->oneUpId);

                nextSegment();
                CCLOG("Created OneUp with ID:%d for abs:%d",oneUp->oneUpId,absoluteIndex);

            }else if (choice == BAD_FOOD_PAIR_OBSTRUCTION_GROUP_CHOICE){

                b2Vec2 badFoodPosition;
                float rotation = 0;
                if (isTheBadFoodOnTop) {
                    badFoodPosition = up->body->GetPosition();
                    badFoodPosition.y = badFoodPosition.y - up->originalHalfHeight;
                }else{
                    badFoodPosition = down->body->GetPosition();
                    badFoodPosition.y = badFoodPosition.y + down->originalHalfHeight;
                }


                ModelBadFood *bf = createBadFood(badFoodTypeForCurrentTotalTime(), badFoodPosition);
                bf->rotation = rotation;
                b2RevoluteJointDef revoluteJointDef;

                if (isTheBadFoodOnTop) {
                    revoluteJointDef.Initialize(bf->body, up->body, badFoodPosition);

                }else{
                    revoluteJointDef.Initialize(bf->body, down->body, badFoodPosition);
                }

                world->CreateJoint(&revoluteJointDef);
            }else if (choice == PAIR_BAD_FOOD_WITHOUT_TREES_GROUP_CHOICE){
                down->makeInvisibleUncollidable();
                up->makeInvisibleUncollidable();

                b2Vec2 upBadFoodPosition = up->body->GetPosition();
                upBadFoodPosition.y = upBadFoodPosition.y - up->originalHalfHeight;

                b2Vec2 downBadFoodPosition = down->body->GetPosition();
                downBadFoodPosition.y = downBadFoodPosition.y + down->originalHalfHeight;

                ModelBadFood *ubf = createBadFood(badFoodTypeForCurrentTotalTime(), upBadFoodPosition);
                ubf->hasStem = true;
                ModelBadFood *dbf = createBadFood(badFoodTypeForCurrentTotalTime(), downBadFoodPosition);
                dbf->hasStem = true;

                ubf->rotation = 0;
                dbf->rotation = 0;

                b2RevoluteJointDef upRevoluteJointDef;
                upRevoluteJointDef.Initialize(ubf->body, up->body, upBadFoodPosition);
                world->CreateJoint(&upRevoluteJointDef);

                b2RevoluteJointDef downRevoluteJointDef;
                downRevoluteJointDef.Initialize(dbf->body, down->body, downBadFoodPosition);
                world->CreateJoint(&downRevoluteJointDef);
            }else if (choice == SINGLE_BAD_FOOD_JUMPING_FROM_GROUND_AND_TREE_ABOVE_CHOICE){
                down->makeInvisibleUncollidable();
                b2Vec2 downBadFoodPosition = down->body->GetPosition();
                downBadFoodPosition.y = downBadFoodPosition.y + down->originalHalfHeight;

                ModelBadFood *dbf = createBadFood(badFoodTypeForCurrentTotalTime(), downBadFoodPosition);
                dbf->hasStem = true;
                dbf->isJumpy = true;
                dbf->rotation = 0;
                b2PrismaticJointDef prismaticJointDef;
                prismaticJointDef.Initialize(down->body, dbf->body, downBadFoodPosition, b2Vec2(0,1));
                world->CreateJoint(&prismaticJointDef);

                b2RopeJointDef ropeJointDef;
                ropeJointDef.bodyA = down->body;
                ropeJointDef.bodyB = dbf->body;
                ropeJointDef.localAnchorA = b2Vec2(0, 0);
                ropeJointDef.localAnchorB = b2Vec2(0, 0);
                ropeJointDef.maxLength = down->originalHalfHeight;
                ropeJointDef.collideConnected = false;
                world->CreateJoint(&ropeJointDef);
            }
        }
    }
}

BAD_FOOD_TYPE ModelWorld::badFoodTypeForCurrentTotalTime(){
    return BAD_PIZZA_FOOD_TYPE;
}

ModelProjectileHolder* ModelWorld::createProjectileHolder(b2Vec2 position){
    ModelProjectileHolder *projectile = new ModelProjectileHolder(position,world,projectileId);
    projectileHolders[projectile->projectileId] = projectile;
    return projectile;
}

ModelBadFood* ModelWorld::createBadFood(BAD_FOOD_TYPE type, b2Vec2 position){
    ModelBadFood *badFood = new ModelBadFood(type,position,world,getBadFoodId());

    badFoodNodes[badFood->badFoodId] = badFood;
    newBadFoodIds.insert(badFood->badFoodId);
    return badFood;
}

ModelBoss* ModelWorld::createBoss(BOSS_TYPE type, b2Vec2 position){
    ModelBoss *boss = new ModelBoss(type,position,world,getBossId());

    bosses[boss->bossId] = boss;
    newBosses.insert(boss->bossId);
    return boss;
}

b2Vec2 ModelWorld::obstructionDownPosition(float originalHalfHeight, float32 relativeMovementRate){
    float x = obstructionStartX;
    float y = 0.0;

    //Check if there was a last obstruction
    bool lastObstructionStillExists = false;
    int lastObstructionId = obstructionIndex - 1; //Since we always want the down obstruction only.
    std::unordered_map<int,ModelObstruction*>::iterator it = obstructions.find(lastObstructionId);
    if (it != obstructions.end()) {
        lastObstructionStillExists = true;
    }

    if (lastObstructionStillExists) {
        //If there was make sure one can transition to this new one (min / max ranges)
        ModelObstruction *lastOb = obstructions[lastObstructionId];

        //Find out where the last obstruction was
        b2Vec2 lastPos = lastOb->body->GetPosition();

        //How long will it take for the new obstruction to reach the same position
        b2Vec2 newVelocity =  b2Vec2(groundVelocity.x + relativeMovementRate,groundVelocity.y);
        float time = (x - lastPos.x) / newVelocity.x;

        //In that time how low can the player drop -> min height + halfheight
        float maxHeightDrop = lastOb->originalHalfHeight + lastPos.y + 0.5 * world->GetGravity().y*time*time; //1/2gt^2 //down's upper limit - how much he will fall (gravity)
        float minHeight = max(float(maxHeightDrop - originalHalfHeight),float(0.0f - originalHalfHeight + ground->groundHeight*1.1)) ; //subtract original half height, because of the anchor location at 0.5

        //In that time how high can he fly -> max height - halfheight
        float maxHeightRise = playerAscensionsRate*time + lastOb->originalHalfHeight + lastPos.y + verticalGap; // upper's lower limit, + how much it can climb
        //add original half height, because of anchor location at 0.5

        float maxHeight = min(maxHeightRise - originalHalfHeight,originalHalfHeight*obstructionRangeFactor + ground->groundHeight);

        //Actual height is a random number somewhere between min height and max height
        int randomRange = (int)((maxHeight - minHeight)*100.0f);
        int randomChoice = arc4random()%randomRange;
        float adjustedRandom = (randomChoice/100.0f);
        float finalHeight = adjustedRandom + minHeight;

        if (finalHeight + originalHalfHeight + verticalGap > ukPos.y - 1) {
            finalHeight = ukPos.y - 1 - verticalGap - originalHalfHeight;
        }

        y = finalHeight;

    }else{
        //Create random height. Return it.
        float obstructionDownHeight = 0 - originalHalfHeight + (arc4random()%((int)floorf(originalHalfHeight*200*obstructionRangeFactor)))/100.0f + ground->groundHeight; //200 because 2 * halfheight = full height. * 100, to make it int

        y = obstructionDownHeight;
    }

    return b2Vec2(x, y);
}

ModelObstruction* ModelWorld::createObstruction(float x, float y,bool isUp){
    int obstructionId = getObstructionId();
    ModelObstruction *obstruction = new ModelObstruction(currentArt.obstructionFrame + "t", b2Vec2(x, y),world,obstructionId);
    b2Vec2 velocityForNewObstructionDown = b2Vec2(groundVelocity.x * obstruction->relativeObstructionMovementRate, groundVelocity.y);
    obstruction->isUp = false;

    obstructions[obstructionId] = obstruction;
    newObstructionIds.insert(obstructionId);
    obstruction->body->SetLinearVelocity(velocityForNewObstructionDown);

    return obstruction;
}

int ModelWorld::getProjectileId(){
    return projectileId++;
}

int ModelWorld::getObstructionId(){
    return obstructionIndex++;
}

int ModelWorld::getFlowerJetId(){
    return flowerJetIndex++;
}

int ModelWorld::getBadFoodId(){
    return badFoodId++;
}

int ModelWorld::getBossId(){
    return bossId++;
}

int ModelWorld::getAbsoluteIndex(){
    return absoluteIndex;
}

int ModelWorld::getParticleId(){
    return particleIndex++;
}

bool ModelWorld::isLastLevelReached(){
    if (aSegmentIndex == lastLevelIndex) {
        return true;
    }

    return false;
}

const std::string ModelWorld::currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}

int ModelWorld::getASegment(){
    return aSegmentIndex;
}

int ModelWorld::getBSegment(){
    return bSegmentIndex;
}

NS_RIY_END
