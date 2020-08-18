//
//  ModelObstruction.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelObstruction__
#define __SiderScroller__ModelObstruction__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelObstruction : public ModelCollidable{

private:

public:
    bool hasBadFood;
    bool isBadFoodUp;
    bool isMovingUpDown;
    float upperLimitY;
    float lowerLimitY;
    float velocityY;
    void update();

    b2Body *body;
    std::string type;
    bool isUp;
    int obstructionId;
    float originalHalfWidth;
    float originalHalfHeight;
    float32 relativeObstructionMovementRate;
    void init(std::string type,b2Vec2 position,b2World* world,int obstructionId);
    bool isInvisibleAndUncollidable;

    void makeInvisibleUncollidable();
    ModelObstruction(std::string type,b2Vec2 position,b2World* world,int obstructionId){
        init(type,position,world,obstructionId);
    };


    ~ModelObstruction()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END

#endif /* defined(__SiderScroller__ModelObstruction__) */
