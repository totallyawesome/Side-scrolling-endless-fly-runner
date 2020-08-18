//
//  ModelObstruction.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelObstruction.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN

void ModelObstruction::init(std::string type,b2Vec2 position,b2World* world,int obstructionId){
    hasBadFood = false;
    isBadFoodUp = false;
    isMovingUpDown = false;
    isInvisibleAndUncollidable = false;
    typeOfCollidable = OBSTRUCTION_COLLIDABLE_TYPE;
    relativeObstructionMovementRate = 1.0f; // - moves left faster, + moves right
    this->type = type;
    this->obstructionId = obstructionId;
    b2BodyDef obstructionBodyDef;
    obstructionBodyDef.position.Set(position.x,position.y);
    obstructionBodyDef.type = b2_kinematicBody;
    body = world->CreateBody(&obstructionBodyDef);
    this->body->SetUserData(this);


    //Based on the type, we need to create a rectangle of the correct size, to match the obstruction.
    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(body, type);

    b2Vec2 orhw = getOriginalHalfWidthAndHeightForBody(body);

    this->originalHalfWidth = orhw.x;
    this->originalHalfHeight = orhw.y;
}

void ModelObstruction::makeInvisibleUncollidable(){
    isInvisibleAndUncollidable = true;
    for ( b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext() ) {
        b2Filter filter = f->GetFilterData( );
        filter.categoryBits = NOTHING_PHYSICS_CATEGORY;
        f->SetFilterData(filter);
    }
}

void ModelObstruction::update(){

    if (isMovingUpDown) {
        b2Vec2 pos = body->GetPosition();
        if (pos.y > upperLimitY) {
            b2Vec2 oldVelocity = body->GetLinearVelocity();
            body->SetLinearVelocity(b2Vec2(oldVelocity.x, 0 - velocityY));
        }else if (pos.y < lowerLimitY){
            b2Vec2 oldVelocity = body->GetLinearVelocity();
            body->SetLinearVelocity(b2Vec2(oldVelocity.x, velocityY));
        }
    }
}

NS_RIY_END
