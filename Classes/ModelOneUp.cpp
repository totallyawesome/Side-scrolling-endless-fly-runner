//
//  ModelOneUp.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelOneUp.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN

void ModelOneUp::init(b2Vec2 position,b2World* world,int onepId,int segId,int absId,int majId,int aSeg,int bSeg){
    this->typeOfCollidable = ONE_UP_COLLIDABLE_TYPE;
    this->oneUpId = onepId;
    this->segmentId = segId;
    this->absoluteId = absId;
    this->majorId = majId;
    this->aSegment = aSeg;
    this->bSegment = bSeg;

    b2BodyDef obstructionBodyDef;
    obstructionBodyDef.position.Set(position.x,position.y);
    obstructionBodyDef.type = b2_kinematicBody;
    body = world->CreateBody(&obstructionBodyDef);


    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(body, Constants::Shapes::oneUp);

    this->radius = getOriginalHalfWidthAndHeightForBody(body).y;

    this->body->SetUserData(this);
}

NS_RIY_END
