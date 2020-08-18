//
//  ModelSkipLevel.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelSkipLevel.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN

void ModelSkipLevel::init(b2Vec2 position,b2World* world,int skipId){
    this->typeOfCollidable = SKIP_LEVEL_COLLIDABLE_TYPE;
    this->skipLevelId = skipId;


    b2BodyDef obstructionBodyDef;
    obstructionBodyDef.position.Set(position.x,position.y);
    obstructionBodyDef.type = b2_kinematicBody;
    body = world->CreateBody(&obstructionBodyDef);


    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(body, Constants::Shapes::skip);

    this->radius = getOriginalHalfWidthAndHeightForBody(body).y;

    this->body->SetUserData(this);
}

NS_RIY_END
