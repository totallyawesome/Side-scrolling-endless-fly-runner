//
//  ModelPlayer.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ModelPlayer.h"
#include "GB2ShapeCache-x.h"

NS_RIY_BEGIN
void ModelPlayer::init(b2World* world,float groundHalfHeight){
    typeOfCollidable = PLAYER_COLLIDABLE_TYPE;

    b2BodyDef playerBodyDef;
    playerBodyDef.position.Set(3.0f, 4.0f);
    playerBodyDef.type = b2_dynamicBody;
    body = world->CreateBody(&playerBodyDef);
    this->body->SetUserData(this);
    this->body->SetFixedRotation(true);
    GB2ShapeCache *sc = GB2ShapeCache::sharedGB2ShapeCache();
    sc->addFixturesToBody(body, "walk1");


    b2Vec2 orhhw = getOriginalHalfWidthAndHeightForBody(body);

    this->originalHalfWidth = orhhw.x;
    this->originalHalfHeight = orhhw.y;

    body->SetTransform(b2Vec2(3.0, originalHalfHeight + groundHalfHeight), 0);

    //Last good mass: 0.80079
    b2MassData *oldMassData = new b2MassData();
    body->GetMassData(oldMassData);
    b2MassData *newMassData = new b2MassData();
    newMassData->center = oldMassData->center;
    newMassData->I = oldMassData->I;
    newMassData->mass = 0.80079;

    body->SetMassData(newMassData);

    delete oldMassData;
    delete  newMassData;
}

NS_RIY_END
