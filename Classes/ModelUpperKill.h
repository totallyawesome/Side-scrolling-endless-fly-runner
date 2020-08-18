//
//  ModelUpperKill.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelUpperKill__
#define __SiderScroller__ModelUpperKill__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelUpperKill : public ModelCollidable{

private:


public:
    b2Body *body;
    void init(b2World* world,b2Vec2 position);
    float groundHeight;

    ModelUpperKill(b2World* world,b2Vec2 position){
        init(world,position);
    };


    ~ModelUpperKill()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelUpperKill__) */
