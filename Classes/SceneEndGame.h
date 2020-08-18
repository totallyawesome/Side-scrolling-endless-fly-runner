//
//  SceneEndGame.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__SceneEndGame__
#define __SiderScroller__SceneEndGame__

#include "AppMacros.h"
#include "cocos2d.h"
#include "ViewPlayer.h"
#include "ModelArt.h"

USING_NS_CC;
NS_RIY_BEGIN
class SceneEndGame : public LayerColor
{

protected:
    void update(float dt);

private:
    Sprite* background;
    Label *gameNameLabel;
    Size visibleSize;
    Vec2 origin;
    ViewPlayer *player;
    Menu *menu;
    void playCallback(Ref* pSender);

    MenuItemSprite* createMenuItemSprite(std::string normalFrameName,std::string selectedFrameName,const ccMenuCallback& callback,float xPosition, float yPosition);
    Node* mainMenuLayer;

    std::vector<std::string> congMessages;

    std::vector<Sprite*>lowerGroundPieces; //closest to observer
    std::vector<Sprite*>midGroundPieces; //field by road
    std::vector<Sprite*>topGroundPieces; //equivalent of road
    std::vector<Sprite*>aboveGroundPieces; //behind the player
    std::vector<Sprite*>groundFlowers;//Flowers on topGround and above ground

    void tintWithTime(Sprite* sprite,float time,Color3B color);
    void createMainMenu();
    void updatePlayer();
    void createTrees(bool onTop);
    void groundPieces(ArtChunk chunk);
    void loadCongMessages();

    float timeSinceLastMessageChange;
    float gapBetweenMessageChanges;
    int messageIndex;
public:
    ~SceneEndGame()
    {
    }

    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(SceneEndGame);
};

NS_RIY_END
#endif /* defined(__SiderScroller__SceneEndGame__) */
