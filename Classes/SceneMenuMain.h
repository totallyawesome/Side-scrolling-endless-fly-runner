#ifndef __muchly__SceneMenuMain__
#define __muchly__SceneMenuMain__

#include "AppMacros.h"
#include "cocos2d.h"
#include "ViewPlayer.h"
#include "ModelArt.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "GameCenterHandlerC.h"
#endif

USING_NS_CC;
NS_RIY_BEGIN
class SceneMenuMain : public LayerColor
{

protected:
    void update(float dt);

private:    
    Sprite* audioButton;
    Texture2D* audioButtonOnTex;
    Texture2D* audioButtonOffTex;
    Sprite* audioButtonSelected;
    
    Sprite* background;
    Label *gameNameLabel;
    Size visibleSize;
    Vec2 origin;
    ViewPlayer *player;
    bool isStoreLoaded;
    Menu *menu;
    void playCallback(Ref* pSender);
    void rankingCallback(Ref* pSender);
    void rateCallback(Ref* pSender);
    void creditsCallBack(Ref* pSender);
    void colorsCallBack(Ref* pSender);

    MenuItemSprite* createMenuItemSprite(std::string normalFrameName,std::string selectedFrameName,const ccMenuCallback& callback,float xPosition, float yPosition);
    Node* mainMenuLayer;
    
    std::vector<Sprite*>lowerGroundPieces; //closest to observer
    std::vector<Sprite*>midGroundPieces; //field by road
    std::vector<Sprite*>topGroundPieces; //equivalent of road
    std::vector<Sprite*>aboveGroundPieces; //behind the player
    std::vector<Sprite*>groundFlowers;//Flowers on topGround and above ground
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    Menu *iapMenuButton;
    Menu *iapMenu;
    bool hideIAP;
    void createIapMenu();
    void openIapMenuCallback(Ref* pSender);
    void backCallback(Ref* pSender);
    void removeAdsCallback(Ref* pSender);
    void restorePurchasesCallback(Ref* pSender);

    GameCenterHandlerC *gch;
#endif
    
    void showHelpCallback(Ref* pSender);
    void toggleSFXCallback(Ref* pSender);
    void tintWithTime(Sprite* sprite,float time,Color3B color);
    void createMainMenu();
    void updatePlayer();
    void createTrees(bool onTop);
    void groundPieces(ArtChunk chunk);
    
public:
    ~SceneMenuMain()
    {
    }
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static Scene* createScene();

    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();  
    
    // implement the "static create()" method manually
    CREATE_FUNC(SceneMenuMain);
};

NS_RIY_END

#endif // /* defined(__muchly__SceneMenuMain__) */
