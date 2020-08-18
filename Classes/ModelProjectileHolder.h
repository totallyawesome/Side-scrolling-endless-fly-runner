//
//  ModelProjectileHolder.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelProjectileHolder__
#define __SiderScroller__ModelProjectileHolder__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelProjectileHolder {

private:


public:
    b2Body *body;
    int projectileId;

    void init(b2Vec2 position,b2World* world,int projId);

    ModelProjectileHolder(b2Vec2 position,b2World* world,int projId){
        init(position,world,projId);
    };


    ~ModelProjectileHolder()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelProjectileHolder__) */
