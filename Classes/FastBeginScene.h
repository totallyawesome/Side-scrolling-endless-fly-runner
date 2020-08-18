//
//  FastBeginScene.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__FastBeginScene__
#define __SiderScroller__FastBeginScene__

#include "AppConstants.h"
#include "AppMacros.h"
#include "cocos2d.h"

USING_NS_CC;
NS_RIY_BEGIN

class FastBeginScene : public LayerColor {

protected:
    void update(float dt);

private:
    bool isFading;
    bool loadedSprites;
    float totalTime;
    std::string en(std::string s);
public:
    ~FastBeginScene()
    {
    }

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    CREATE_FUNC(FastBeginScene);

};

NS_RIY_END


#endif /* defined(__SiderScroller__FastBeginScene__) */
