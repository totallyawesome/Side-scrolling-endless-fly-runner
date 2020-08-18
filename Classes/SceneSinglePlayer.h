//
//  SceneSinglePlayer.h
//  muchly
//
//  Created by Rahul Iyer on 14/06/14.
//
//

#ifndef __muchly__SceneSinglePlayer__
#define __muchly__SceneSinglePlayer__
#include <sstream>
#include "AppConstants.h"
#include "AppMacros.h"
#include "cocos2d.h"
#include "ModelWorld.h"
#include "ViewPlayer.h"
#include "SoundManager.h"
#include "SceneArtHolder.h"

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
#include "GameCenterHandlerC.h"
#endif

NS_RIY_BEGIN
using namespace cocos2d;
class SceneSinglePlayer : public cocos2d::Layer
{
protected:
    void update(float dt);

private:
    Vec2 checkPos;
    Vec2 pausePosition;
    float pauseSpriteWidth;
    float pauseSpriteHeight;

    Sprite* checkpointSpriteLabelInGame;
    Sprite* birdsBlastedSpriteLabelInGame;
    Label *checkpointsCollectedLabel;
    Label *birdsBlastedLabel;

    Sprite* audioButton;
    Texture2D* audioButtonOnTex;
    Texture2D* audioButtonOffTex;
    Sprite* audioButtonSelected;
    
    Size winSize;
    
    Sprite* tearfix;
    Sprite* tearfix2;

    std::vector<Sprite*>stars;
    Sprite* sun;
    Sprite* moon;
    std::vector<Sprite*>lowerGroundPieces; //closest to observer
    std::vector<Sprite*>midGroundPieces; //field by road
    std::vector<Sprite*>topGroundPieces; //equivalent of road
    std::vector<Sprite*>aboveGroundPieces; //behind the player
    std::vector<Sprite*>groundFlowers;//Flowers on topGround and above ground
    
    //To remove subpixel artifacts
    std::vector<Sprite*>alowerGroundPieces; //closest to observer
    std::vector<Sprite*>amidGroundPieces; //field by road
    std::vector<Sprite*>atopGroundPieces; //equivalent of road
    
    int currentA;
    float backgroundWidth;
    float limitAtWhichToRemove;
    Color3B backgroundColor;
    
    SceneArtHolder artHolders[2];

    int currentArtHolder;
    
    LayerColor *backgroundLayer;
    Sprite *instructionSprite;
    bool isPaused;
    bool isTesting;
    bool isGameStarted;
    bool isGameStopped;
    ViewPlayer *player;
    std::unordered_map<int,Sprite*>obstructions;
    std::unordered_map<int,Sprite*>flowerJets;
    std::unordered_map<int,Sprite*>badFoods;
    std::unordered_map<int,Sprite*>bosses;
    std::unordered_map<int,Sprite*>oneUps;
    std::unordered_map<int,Sprite*>skipLevels;
    std::unordered_map<int,Sprite*>particles;

    std::vector<TouchEvent>receivedTouches;
    int score;
    int bestScore;
    int bestDistance;
    int jetCounter;
    Label *scoreLabel;
    Size visibleSize;
    Vec2 origin;
    Menu *pauseButton; // For pausing the game
    Menu *pauseMenu; // shown while game is paused
    
    
    bool onTouchBegan(Touch *touch, Event *event);
    void updateFlowerJets();
    void updateBadFoods();

    void updateObstructions();
    void updatePlayer();
    void updateScoreLabel();
    void updateBirdsBlasted();
    void updateCheckpointsCollectedLabel();
    void updateBosses();
    void updateOneUps();
    void updateSkipLevels();
    void updateParticles();

    void checkForContacts();
    
    void showInstructions();
    void hideInstruction();
    template <typename T>
    std::string numberTostring(T value);
    void backCallback(Ref* pSender);
    
    void addPauseButton();
    void removePauseButton();
    void receivePauseGame(Ref* pSender);
    void receiveResumeGame(Ref* pSender);

    void createPauseButton();
    void createPauseMenu();
    
    
    void playCallback(cocos2d::Ref* pSender);
    void continueCallBack(Ref* pSender);
    void rankingCallback(Ref* pSender);
    void rateCallback(Ref* pSender);
    void createGroundSpritesForScene();
    void updateGroundSpritesForScene(float distanceToMove);

    Sprite* addFallenPlayer(bool isFinished);
    float getBiggestHeight(Sprite* sprite);
    Sprite* createWithHolder(std::string labelImageName,Sprite* toPutBelow,std::string textValue);
    MenuItemSprite* createMenuItemSprite(std::string imageName, std::string selectedImageName,const ccMenuCallback& callback,float xPosition,float belowY);
    void addStatsAndMenu(bool isReachedTheEnd);
    
    void updateBackgroundPosition(float distanceToMove);
    void updateLevelSpriteFrames();
    
    std::string randomCloudFrameName();
    
    void fadeOutAndMakeInvisible(Sprite* sprite,float transitionTime,bool reorder,SPRITE_Z_ORDER newZOrder);
    void fadeOutAndRemove(Sprite* sprite,float transitionTime);

    void makeVisibleAndFadeIn(Sprite* sprite,float transitionTime);
    void updateSunMoon(Sprite* body,unsigned char r,unsigned char g,unsigned char b,std::string frameName,float transitionTime);

    void fadeOutAndRemoveChildren(Sprite* sprite,float transitionTime);
    void makeSunMoonSet(Sprite *body,float transitionTime);
    
    void spawnClouds();
    void makeStarsSet(float transitionTime);
    
    void tintWithTime(Sprite* sprite,float time,Color3B color);
    void groundPieces(bool isForArtifacts);
    void tintGroundSprites(bool isForArtifacts);
    void oneTime(float delta);
    void toggleSFXCallback(Ref* pSender);
    void colorsCallBack(Ref* pSender);
    void addBirdToEmptyPixel(Sprite* emptypixel);

public:
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    GameCenterHandlerC *gch;
#endif
    
    std::vector<Sprite*>ground;
    float totalTime;
    ModelWorld *model;
    
    void pauseGame();
    void resumeGame();
    
    ~SceneSinglePlayer()
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        delete gch;
#endif
        delete model;
    }
    
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::Scene* createScene();
    
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    
    // implement the "static create()" method manually
    CREATE_FUNC(SceneSinglePlayer);
};

NS_RIY_END
#endif /* defined(__muchly__SceneSinglePlayer__) */
