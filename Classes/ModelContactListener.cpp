//
//  ModelContactListener.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelContactListener.h"

void ModelContactListener::BeginContact(b2Contact* contact) {
    void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
    if (bodyUserDataA) {

        ModelCollidable *collidableA = static_cast<ModelCollidable*>(bodyUserDataA);
        void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
        if(bodyUserDataB){
            ModelCollidable *collidableB = static_cast<ModelCollidable*>(bodyUserDataB);

            if (collidableA->typeOfCollidable == PLAYER_COLLIDABLE_TYPE) {
                processPlayerBeginCollision(collidableA, collidableB);
            }else if(collidableB->typeOfCollidable == PLAYER_COLLIDABLE_TYPE){
                processPlayerBeginCollision(collidableB, collidableA);
            }

            if (collidableA->typeOfCollidable == FLOWERJET_COLLIDABLE_TYPE) {
                processFlowerJetBeginCollision(collidableA, collidableB);
            }else if(collidableB->typeOfCollidable == FLOWERJET_COLLIDABLE_TYPE){
                processFlowerJetBeginCollision(collidableB, collidableA);
            }
        }
    }
}

void ModelContactListener::processFlowerJetBeginCollision(ModelCollidable *flowerJetCollidable, ModelCollidable *collidableB){

    ModelFlowerJet *flowerJet = (ModelFlowerJet*)flowerJetCollidable;
    int flowerJetId = flowerJet->flowerJetId;
    flowerJetIdsMadeContactWith.insert(flowerJetId);

    switch (collidableB->typeOfCollidable) {
        case BAD_FOOD_COLLIDABLE_TYPE:
        {
            ModelBadFood *badFood = (ModelBadFood*)collidableB;
            int badFoodId = badFood->badFoodId;
            badFoodIdsToRemove.insert(badFoodId);
            break;
        }
        case BOSS_COLLIDABLE_TYPE:
        {
            ModelBoss *boss = (ModelBoss*)collidableB;
            int bossId = boss->bossId;

            if (b2Distance(boss->body->GetPosition(), flowerJet->body->GetPosition()) <= 1.1*(boss->radius + flowerJet->radius) ) {
                bossIdHitWithFlowerJet.insert(bossId);
            }
            break;
        }

        default:
            break;
    }
}

void ModelContactListener::processPlayerBeginCollision(ModelCollidable *playerCollidable,ModelCollidable *collidableB){

    switch (collidableB->typeOfCollidable) {

        case ONE_UP_COLLIDABLE_TYPE:
        {
            ModelOneUp *oneUp = (ModelOneUp*)collidableB;
            int oneUpId = oneUp->oneUpId;
            oneUpsMadeContactWith.insert(oneUpId);
            break;
        }
        case SKIP_LEVEL_COLLIDABLE_TYPE:{
            ModelSkipLevel *skip = (ModelSkipLevel*)collidableB;
            int skipId = skip->skipLevelId;
            skipsMadeContactWith.insert(skipId);
            break;
        }
        case OBSTRUCTION_COLLIDABLE_TYPE:
        case BAD_FOOD_COLLIDABLE_TYPE:
        case BOSS_COLLIDABLE_TYPE:
        case UPPER_KILL_COLLIDABLE_TYPE:
            contacts.push_back(PLAYER_OBSTRUCTION_CONTACT_EVENT);
            break;
        default:
            break;
    }
}

void ModelContactListener::processPlayerEndCollision(ModelCollidable *playerCollidable,ModelCollidable *collidableB){

    switch (collidableB->typeOfCollidable) {

        default:
            break;
    }

}

void ModelContactListener::EndContact(b2Contact* contact) {
    void* bodyUserDataA = contact->GetFixtureA()->GetBody()->GetUserData();
    if (bodyUserDataA) {

        ModelCollidable *collidableA = static_cast<ModelCollidable*>(bodyUserDataA);
        void* bodyUserDataB = contact->GetFixtureB()->GetBody()->GetUserData();
        if(bodyUserDataB){
            ModelCollidable *collidableB = static_cast<ModelCollidable*>(bodyUserDataB);
            if (collidableA->typeOfCollidable == PLAYER_COLLIDABLE_TYPE) {
                processPlayerEndCollision(collidableA, collidableB);
            }else if (collidableB->typeOfCollidable == PLAYER_COLLIDABLE_TYPE){
                processPlayerEndCollision(collidableB, collidableA);
            }
        }
    }
}
