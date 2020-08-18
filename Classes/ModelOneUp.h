//
//  ModelOneUp.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelOneUp__
#define __SiderScroller__ModelOneUp__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelOneUp : public ModelCollidable{

private:


public:
    b2Body *body;
    int oneUpId;
    float radius;
    int segmentId; // Stores this in some constant when on contact so we know where to start the next level from.
    int absoluteId;
    int majorId;
    int aSegment;
    int bSegment;

    void init(b2Vec2 position,b2World* world,int onepId,int segId,int absId,int majId,int aSeg,int bSeg);

    ModelOneUp(b2Vec2 position,b2World* world,int onepId,int segId,int absId,int majId,int aSeg,int bSeg){
        init(position,world,onepId,segId,absId,majId,aSeg,bSeg);
    };


    ~ModelOneUp()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelOneUp__) */
