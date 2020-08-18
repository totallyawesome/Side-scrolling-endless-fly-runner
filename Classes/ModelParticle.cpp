//
//  ModelParticle.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelParticle.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN

void ModelParticle::init(b2Vec2 position,b2World* world,int pId){
    this->typeOfCollidable = PARTICLE_COLLIDABLE_TYPE;
    this->particleId = pId;
    //Based on the type, we need to create a rectangle of the correct size, to match the obstruction.

    b2BodyDef obstructionBodyDef;
    obstructionBodyDef.position.Set(position.x,position.y);
    obstructionBodyDef.type = b2_dynamicBody;
    obstructionBodyDef.linearDamping = 10;
    body = world->CreateBody(&obstructionBodyDef);

    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(body, Constants::SpriteNames::particle);

    this->radius = getOriginalHalfWidthAndHeightForBody(body).x;

    this->body->SetUserData(this);
}

NS_RIY_END
