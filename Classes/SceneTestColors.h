//
//  SceneTestColors.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__SceneTestColors__
#define __SiderScroller__SceneTestColors__

#include "AppConstants.h"
#include "AppMacros.h"
#include "cocos2d.h"
#include "ModelArt.h"
#include "ViewPlayer.h"

using namespace cocos2d;
NS_RIY_BEGIN
class SceneTestColors : public cocos2d::LayerColor{
protected:
    void update(float dt);
private:

    Size visibleSize;
    Vec2 origin;
    Size winSize;

    Menu *menu;

    template <typename T>
    std::string numberTostring(T value);

    std::vector<Sprite*>lowerGroundPieces; //closest to observer
    std::vector<Sprite*>midGroundPieces; //field by road
    std::vector<Sprite*>topGroundPieces; //equivalent of road
    std::vector<Sprite*>aboveGroundPieces; //behind the player
    std::vector<Sprite*>groundFlowers;//Flowers on topGround and above ground
    Sprite* background;
    Label *scoreLabel;
    std::vector<Sprite*>stars;
    Sprite* sun;
    Sprite* moon;
    Sprite* bird;


    Sprite* tree;
    Sprite* boss;

    ViewPlayer* player;
    Sprite* save;
    Sprite* skip;
    Sprite* pause;

    //Dont care
    Sprite* cloud;
    Sprite* particle;
    Sprite* flowerjet;
    //End don't care

    std::vector<std::string> allSets;
    int currentColorIndex;

    std::string totalSelected;

    void createSceneFromColorSet(int index);
    void groundPieces();
    void tintWithTime(Sprite* sprite,float time,Color3B color);
    std::vector<std::string> splitString(std::string input, char delim);
    MenuItemSprite* createMenuItemSprite(std::string normalFrameName,std::string selectedFrameName,const ccMenuCallback& callback,float xPosition, float yPosition);
    void next(Ref* pSender);
    void prev(Ref* pSender);
    void selectNext(Ref* pSender);
    void logAll(Ref* pSender);


public:

    ~SceneTestColors()
    {
        delete player;
    }

    static cocos2d::Scene* createScene();

    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(SceneTestColors);
};


NS_RIY_END
#endif /* defined(__SiderScroller__SceneTestColors__) */
