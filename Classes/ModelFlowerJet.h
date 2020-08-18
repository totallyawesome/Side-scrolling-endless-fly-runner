//
//  ModelFlowerJet.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelFlowerJet__
#define __SiderScroller__ModelFlowerJet__

#include <Box2D/Box2D.h>
#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelCollidable.h"

NS_RIY_BEGIN

class ModelFlowerJet : public ModelCollidable{

private:


public:
    b2Body *body;
    int flowerJetId;
    float radius;
    float scale;
    void init(b2Vec2 position,b2World* world,int flowerId);

    ModelFlowerJet(b2Vec2 position,b2World* world,int flowerId){
        init(position,world,flowerId);
    };


    ~ModelFlowerJet()
    {
        body->GetWorld()->DestroyBody(body);
    };
};

NS_RIY_END


#endif /* defined(__SiderScroller__ModelFlowerJet__) */
