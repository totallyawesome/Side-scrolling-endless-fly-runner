//
//  ModelProjectileHolder.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelProjectileHolder.h"

NS_RIY_BEGIN

void ModelProjectileHolder::init(b2Vec2 position,b2World* world,int projId){

    this->projectileId = projId;

    b2BodyDef obstructionBodyDef;
    obstructionBodyDef.position.Set(position.x,position.y);
    obstructionBodyDef.type = b2_kinematicBody;
    body = world->CreateBody(&obstructionBodyDef);

    this->body->SetUserData(this);
}

NS_RIY_END
