//
//  ModelPlayer.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelPlayer__
#define __SiderScroller__ModelPlayer__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelPlayer : public ModelCollidable{

private:

public:
    b2Body *body;
    void init(b2World* world,float groundHalfHeight);

    //Note this is before any rotation. After rotation this will not be valid.
    float originalHalfWidth;
    float originalHalfHeight;

    ModelPlayer(b2World* world,float groundHalfHeight){
        init(world,groundHalfHeight);
    };


    ~ModelPlayer()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelPlayer__) */
