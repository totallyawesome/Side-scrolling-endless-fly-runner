//
//  JNIBridge.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__JNIBridge__
#define __SiderScroller__JNIBridge__
#include "cocos2d.h"
#include "GameMacros.h"
class JniBridge
{
public:
    static void showAds();
    static void hideAds();
    static void rate();
    static bool isAdShowing;
    static void submitScore(int score);
    static void showLeaderboard();
};

#endif /* defined(__SiderScroller__JNIBridge__) */
