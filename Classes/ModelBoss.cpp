//
//  ModelBoss.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelBoss.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN

void ModelBoss::init(BOSS_TYPE type,b2Vec2 position,b2World* world,int bosId){

    this->type = type;
    this->typeOfCollidable = BOSS_COLLIDABLE_TYPE;
    this->bossId = bosId;
    this->health = 50;

    if (type == BOSS_PIZZA_TYPE) {
        // Any specific stuff.
        this->health = 20;
    }

    b2BodyDef obstructionBodyDef;
    obstructionBodyDef.position.Set(position.x,position.y);
    obstructionBodyDef.type = b2_kinematicBody;
    body = world->CreateBody(&obstructionBodyDef);


    obstructionBodyDef.type = b2_dynamicBody;
    stem = world->CreateBody(&obstructionBodyDef);

    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(body, Constants::Shapes::boss);
    sc->addFixturesToBody(stem, Constants::Shapes::bossstem);

    radius = getOriginalHalfWidthAndHeightForBody(body).y;

    b2Vec2 nP = body->GetPosition();
    stem->SetTransform(b2Vec2(nP.x,nP.y - radius), 0);

    b2RevoluteJointDef revoluteJointDef;
    revoluteJointDef.enableLimit = true;
    revoluteJointDef.lowerAngle = 0;
    revoluteJointDef.upperAngle =  0;
    revoluteJointDef.Initialize(body, stem, nP);
    world->CreateJoint(&revoluteJointDef);

    this->body->SetUserData(this);
    this->stem->SetUserData(this);
}

NS_RIY_END
