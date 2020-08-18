//
//  ModelSkipLevel.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelSkipLevel__
#define __SiderScroller__ModelSkipLevel__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelSkipLevel : public ModelCollidable{

private:


public:
    b2Body *body;
    int skipLevelId;
    float radius;

    void init(b2Vec2 position,b2World* world,int skipId);

    ModelSkipLevel(b2Vec2 position,b2World* world,int skipId){
        init(position,world,skipId);
    };

    ~ModelSkipLevel()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelSkipLevel__) */
