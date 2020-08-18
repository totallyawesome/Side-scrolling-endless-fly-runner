//
//  ModelParticle.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelParticle__
#define __SiderScroller__ModelParticle__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelParticle : public ModelCollidable{

private:


public:
    b2Body *body;
    int particleId;
    float radius;
    void init(b2Vec2 position,b2World* world,int pId);

    ModelParticle(b2Vec2 position,b2World* world,int pId){
        init(position,world,pId);
    };


    ~ModelParticle()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelParticle__) */
