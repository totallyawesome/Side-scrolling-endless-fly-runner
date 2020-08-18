//
//  ModelBadFood.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelBadFood__
#define __SiderScroller__ModelBadFood__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelBadFood : public ModelCollidable{

private:


public:
    bool hasStem;
    b2Body *body;
    float rotation;
    BAD_FOOD_TYPE type;
    int badFoodId;
    float radius;
    bool isJumpy;
    float timeSinceLastJump;
    void init(BAD_FOOD_TYPE type,b2Vec2 position,b2World* world,int badFdId);

    ModelBadFood(BAD_FOOD_TYPE type,b2Vec2 position,b2World* world,int badFdId){
        init(type,position,world,badFdId);
    };


    ~ModelBadFood()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelBadFood__) */
