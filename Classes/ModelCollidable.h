//
//  ModelCollidable.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelCollidable__
#define __SiderScroller__ModelCollidable__

#include <Box2D/Box2D.h>
#include "AppConstants.h"
#include "AppMacros.h"
#include "GameMacros.h"

NS_RIY_BEGIN

class ModelCollidable {
public:
    COLLIDABLE_TYPE typeOfCollidable;
    b2Vec2 getOriginalHalfWidthAndHeightForBody(b2Body *body);
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelCollidable__) */
