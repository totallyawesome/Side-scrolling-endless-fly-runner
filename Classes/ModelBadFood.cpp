//
//  ModelBadFood.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelBadFood.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN

void ModelBadFood::init(BAD_FOOD_TYPE type,b2Vec2 position,b2World* world,int badFdId){
    this->type = type;
    this->typeOfCollidable = BAD_FOOD_COLLIDABLE_TYPE;
    this->badFoodId = badFdId;
    this->hasStem = false;
    this->isJumpy = false;
    this->timeSinceLastJump = 0.0;

    b2BodyDef obstructionBodyDef;
    obstructionBodyDef.position.Set(position.x,position.y);
    obstructionBodyDef.type = b2_dynamicBody;
    body = world->CreateBody(&obstructionBodyDef);

    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(body, Constants::Shapes::badFood);
    radius = getOriginalHalfWidthAndHeightForBody(body).y;
    this->body->SetUserData(this);
}

NS_RIY_END
