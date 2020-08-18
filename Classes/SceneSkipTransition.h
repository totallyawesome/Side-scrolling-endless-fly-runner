//
//  SceneSkipTransition.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__SceneSkipTransition__
#define __SiderScroller__SceneSkipTransition__

#include "AppConstants.h"
#include "AppMacros.h"
#include "cocos2d.h"
#include "ModelArt.h"

using namespace cocos2d;
NS_RIY_BEGIN
class SceneSkipTransition : public cocos2d::LayerColor{
protected:
    void update(float dt);
private:
    bool isPaused = false;

    Size visibleSize;
    Vec2 origin;
    Size winSize;
    Label* scoreLabel;
    std::vector<Sprite*>backgrounds;

    float totalTime;
    float timeLimit;
    float timePerSegment;

    float lastTreeSpawnTime;
    float delayBetweenTreeSpawns;
    int treesPerSegment;

    int currentSegment;
    Sprite* player;
    std::vector<ArtChunk> chunks;
    void createTree();

    void spawnTree();

    Menu *pauseButton; // For pausing the game
    Menu *pauseMenu; // shown while game is paused


    template <typename T>
    std::string numberTostring(T value);
    void removePauseButton();
    void createPauseMenu();
    void receiveResumeGame(Ref* pSender);
    void addPauseButton();
    void createPauseButton();
    void receivePauseGame(Ref* pSender);

public:

    ~SceneSkipTransition()
    {

    }

    static cocos2d::Scene* createScene();
    void pauseGame();
    void resumeGame();

    virtual bool init();

    // implement the "static create()" method manually
    CREATE_FUNC(SceneSkipTransition);

};


NS_RIY_END


#endif /* defined(__SiderScroller__SceneSkipTransition__) */
