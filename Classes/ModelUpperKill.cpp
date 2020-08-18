//
//  ModelUpperKill.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelUpperKill.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN

void ModelUpperKill::init(b2World* world,b2Vec2 position){
    typeOfCollidable = UPPER_KILL_COLLIDABLE_TYPE;

    groundHeight = 1;

    b2BodyDef groundBodyDef;
    groundBodyDef.position.Set(position.x, position.y);
    groundBodyDef.type = b2_staticBody;
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
}

NS_RIY_END
