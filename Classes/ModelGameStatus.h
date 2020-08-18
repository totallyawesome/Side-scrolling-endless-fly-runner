//
//  ModelGameStatus.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelGameStatus__
#define __SiderScroller__ModelGameStatus__

#include <stdio.h>
#include "AppMacros.h"
#include "GameMacros.h"
#include "ModelSavePoint.h"

NS_RIY_BEGIN
namespace GameStatus{
    extern std::vector<ModelSavePoint>savePoints;
    extern int birdsBlasted;
};

NS_RIY_END
#endif /* defined(__SiderScroller__ModelGameStatus__) */
