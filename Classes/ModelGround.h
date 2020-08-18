//
//  ModelGround.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelGround__
#define __SiderScroller__ModelGround__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelGround : public ModelCollidable{

private:


public:
    b2Body *body;
    void init(b2World* world);
    float groundHeight;
    //Note this is before any rotation. After rotation this will not be valid.
    float originalHalfWidth;
    float originalHalfHeight;
    ModelGround(b2World* world){
        init(world);
    };


    ~ModelGround()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelGround__) */
