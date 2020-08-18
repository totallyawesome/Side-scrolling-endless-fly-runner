//
//  ModelSavePoint.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelSavePoint__
#define __SiderScroller__ModelSavePoint__

#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"

NS_RIY_BEGIN

class ModelSavePoint{

private:


public:
    int absoluteIndex;
    int majorIndex;
    int segmentIndex;
    int aSegmentIndex;
    int bSegmentIndex;
    bool isWarp;
    int score;
    float playerPosX;
    float playerPosY;

    int originAbsoluteIndex;
    int originASegmentIndex;

    ModelSavePoint(int absI, int majI, int segI,int aSeg,int bSeg){
        isWarp = false;
        score = 0;
        playerPosX = 0;
        playerPosY = 0;
        originAbsoluteIndex = 0;
        originASegmentIndex = 0;

        absoluteIndex = absI;
        majorIndex = majI;
        segmentIndex = segI;
        aSegmentIndex = aSeg;
        bSegmentIndex = bSeg;
    };
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelSavePoint__) */
