//
//  ModelColor.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelColor__
#define __SiderScroller__ModelColor__

#include <stdio.h>
#include "GameMacros.h"

NS_RIY_BEGIN

class ModelColor {

public:
    unsigned char r;
    unsigned char g;
    unsigned char b;

    ModelColor(unsigned char red,unsigned char green,unsigned char blue) : r(red),g(green),b(blue){

    };
};

NS_RIY_END


#endif /* defined(__SiderScroller__ModelColor__) */
