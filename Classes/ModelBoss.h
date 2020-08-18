//
//  ModelBoss.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelBoss__
#define __SiderScroller__ModelBoss__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelBoss : public ModelCollidable{

private:


public:
    b2Body *body;
    b2Body *stem;
    BOSS_TYPE type;
    int bossId;
    float radius;
    int health;

    void init(BOSS_TYPE type,b2Vec2 position,b2World* world,int bosId);

    ModelBoss(BOSS_TYPE type,b2Vec2 position,b2World* world,int bosId){
        init(type,position,world,bosId);
    };


    ~ModelBoss()
    {
        body->GetWorld()->DestroyBody(body);
        stem->GetWorld()->DestroyBody(stem);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelBoss__) */
