//
//  ModelContactListener.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelContactListener__
#define __SiderScroller__ModelContactListener__

#include <Box2D/Box2D.h>
#include <unordered_set>
#include "AppMacros.h"
#include "ModelCollidable.h"
#include "ModelFlowerJet.h"
#include "ModelBadFood.h"
#include "ModelBoss.h"
#include "ModelOneUp.h"
#include "ModelSkipLevel.h"

using namespace rahuliyer;

class ModelContactListener : public b2ContactListener {

public:
    std::vector<CONTACT_EVENT_TYPE>contacts;
    std::unordered_set<int>eatableIdsMadeContactWith;
    std::unordered_set<int>flowerJetIdsMadeContactWith;
    std::unordered_set<int>badFoodIdsToRemove;
    std::unordered_set<int>bossIdHitWithFlowerJet;
    std::unordered_set<int>scoreNodeIdsMadeContactWith;
    std::unordered_set<int>oneUpsMadeContactWith;
    std::unordered_set<int>skipsMadeContactWith;

    void BeginContact(b2Contact* contact);

    void processFlowerJetBeginCollision(ModelCollidable *flowerJetCollidable, ModelCollidable *collidableB);

    void processPlayerBeginCollision(ModelCollidable *playerCollidable,ModelCollidable *collidableB);

    void processPlayerEndCollision(ModelCollidable *playerCollidable,ModelCollidable *collidableB);

    void EndContact(b2Contact* contact);
};

#endif /* defined(__SiderScroller__ModelContactListener__) */
