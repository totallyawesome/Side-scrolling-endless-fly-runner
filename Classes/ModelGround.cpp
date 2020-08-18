//
//  ModelGround.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelGround.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN

void ModelGround::init(b2World* world){
    typeOfCollidable = GROUND_COLLIDABLE_TYPE;

    b2BodyDef tempGroundBodyDef;
    tempGroundBodyDef.position.Set(0.0f, 0.0f);
    b2Body *tempBody = world->CreateBody(&tempGroundBodyDef);
    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(tempBody, "ground");

    b2Vec2 torg = getOriginalHalfWidthAndHeightForBody(tempBody);
    world->DestroyBody(tempBody);

    groundHeight = torg.y * 2;

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(0.0f, 0.0f);
    body = world->CreateBody(&groundBodyDef);
    b2PolygonShape groundBox;
    groundBox.SetAsBox(50.0f, groundHeight); ////the half-width and half-height of box
    b2FixtureDef groundFixtureDef;
    groundFixtureDef.friction = 0;
    groundFixtureDef.restitution = 0.2;
    groundFixtureDef.shape = &groundBox;
    groundFixtureDef.filter.categoryBits = GROUND_PHYSICS_CATEGORY;
    body->CreateFixture(&groundFixtureDef);
    this->body->SetUserData(this);

    torg = getOriginalHalfWidthAndHeightForBody(body);
    this->originalHalfWidth = torg.x;
    this->originalHalfHeight = torg.y;
}

NS_RIY_END
