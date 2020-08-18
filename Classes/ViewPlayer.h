//
//  ViewPlayer.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ViewPlayer__
#define __SiderScroller__ViewPlayer__

#include <Box2D/Box2D.h>
#include "AppConstants.h"
#include "AppMacros.h"
#include "cocos2d.h"
#include "GameMacros.h"


NS_RIY_BEGIN
using namespace cocos2d;
class ViewPlayer {

private:
    void init(Layer *scene);
    bool _isFlying;


    Action *walkingAction;

    void startWalking();

public:
    Sprite *sprite;

    ViewPlayer(Layer *scene){
        init(scene);
    };


    ~ViewPlayer()
    {
        sprite->removeFromParent();
    };

};

NS_RIY_END
#endif /* defined(__SiderScroller__ViewPlayer__) */
