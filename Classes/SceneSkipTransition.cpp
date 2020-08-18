//
//  SceneSkipTransition.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "SceneSkipTransition.h"
#include "SceneSinglePlayer.h"
#include "ModelArt.h"

USING_NS_CC;
USING_NS_RIY;

NS_RIY_BEGIN

Scene* SceneSkipTransition::createScene(){
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = SceneSkipTransition::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool SceneSkipTransition::init(){

    //////////////////////////////
    // 1. super init first
    if ( !LayerColor::initWithColor(Color4B(0, 0, 0, 255) )) {
        return false;
    }


    isPaused = false;
    winSize = Director::getInstance()->getWinSize();
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    totalTime = 0;
    timeLimit = 0;
    timePerSegment = 3;
    currentSegment = 0;
    lastTreeSpawnTime = 0;
    treesPerSegment = 6;

    char str[100] = {0};
    Animation *walkingAnimation;
    Vector<SpriteFrame*> walkingAnimFrames;
    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    for(int i = 1; i < 5; i++)
    {
        sprintf(str, (Constants::SpriteNames::playerWalkCycleText ).c_str(), i);
        SpriteFrame* frame = cache->getSpriteFrameByName( str );
        walkingAnimFrames.pushBack(frame);
    }

    for(int i = 4; i > 0; i--)
    {
        sprintf(str, (Constants::SpriteNames::playerWalkCycleText ).c_str(), i);
        SpriteFrame* frame = cache->getSpriteFrameByName( str );
        walkingAnimFrames.pushBack(frame);
    }

    walkingAnimation = Animation::createWithSpriteFrames(walkingAnimFrames,0.1);
    player = Sprite::createWithSpriteFrameName(Constants::SpriteNames::playerInitialFrame );
    this->player->runAction(RepeatForever::create(Animate::create(walkingAnimation)));
    this->addChild(player,PLAYER_SPRITE_Z_ORDER);

    float playerX = GameStatus::savePoints.back().playerPosX;
    float playerY = GameStatus::savePoints.back().playerPosY;

    int start = 0;
    int maxSegmentsToSimulate = 2;
    if (GameStatus::savePoints.back().aSegmentIndex+1 - GameStatus::savePoints.back().originASegmentIndex > maxSegmentsToSimulate) {
        start = GameStatus::savePoints.back().aSegmentIndex+1 - maxSegmentsToSimulate;
    }else{
        start = GameStatus::savePoints.back().originASegmentIndex;
    }

    for (int i=start; i<GameStatus::savePoints.back().aSegmentIndex+1; i++) {
        ArtChunk oA = ModelArt::getInstance()->artChunkForASegmentIndex(i);
        chunks.push_back(oA);
        timeLimit+= timePerSegment;
    }

    player->setPosition(playerX*PTM_RATIO, playerY*PTM_RATIO);

    this->runAction(TintTo::create(0,chunks[currentSegment].bgR, chunks[currentSegment].bgG, chunks[currentSegment].bgB));

    ParticleSystem* jet = ParticleSystemQuad::create(Constants::pex::jetflame);
    jet->setPosition(player->getPosition());
    this->addChild(jet,PLAYER_SPRITE_Z_ORDER);

    ParticleSystem* fastTravel = ParticleSystemQuad::create(Constants::pex::fasttravel);
    this->addChild(fastTravel,PLAYER_SPRITE_Z_ORDER);

    delayBetweenTreeSpawns = (timePerSegment / ((float)treesPerSegment));

    std::string s = numberTostring(GameStatus::savePoints.back().originAbsoluteIndex) + " m --> " + numberTostring(GameStatus::savePoints.back().absoluteIndex) + " m";
    scoreLabel = Label::createWithTTF(s, "fonts/Marker Felt.ttf", 24);
    this->addChild(scoreLabel,SCORE_LABEL_Z_ORDER);
    scoreLabel->setPosition(Point(origin.x + visibleSize.width/2 ,origin.y + visibleSize.height - scoreLabel->getContentSize().height));

    createPauseButton();
    this->scheduleUpdate();
    return true;
}

void SceneSkipTransition::spawnTree(){
    ArtChunk chunk = chunks[currentSegment];
    Sprite* tree = Sprite::createWithSpriteFrameName(chunk.obstructionFrame + "t");
    tree->setColor(Color3B(chunk.trR, chunk.trG, chunk.trB));
    tree->setPosition(Vec2(winSize.width + tree->getContentSize().height, winSize.height/2));

    auto move = MoveTo::create(1, Vec2(0 - tree->getContentSize().height, winSize.height/2));
    auto rotate = RotateBy::create(2, 0 - 360);
    tree->runAction(RepeatForever::create(rotate));
    tree->runAction(move);
    this->addChild(tree,OBSTRUCTION_SPRITE_Z_ORDER);
}

void SceneSkipTransition::update(float dt){

    if (currentSegment < chunks.size()) {
        if(totalTime - lastTreeSpawnTime > delayBetweenTreeSpawns){
            spawnTree();
            lastTreeSpawnTime = totalTime;
        }
    }else{
        Director::getInstance()->replaceScene(SceneSinglePlayer::createScene());
    }


    totalTime+=dt;
    if (totalTime > timePerSegment) {
        totalTime = 0;
        lastTreeSpawnTime = 0;
        currentSegment++;
        float transitionTime = timePerSegment/2;
        this->runAction(TintTo::create(transitionTime, chunks[currentSegment].bgR, chunks[currentSegment].bgG, chunks[currentSegment].bgB));
    }
}

void SceneSkipTransition::removePauseButton(){
    pauseButton->setVisible(false);
}

void SceneSkipTransition::pauseGame(){

    if (!isPaused) {
        isPaused = true;
        removePauseButton();

        //pause game
        this->unscheduleUpdate();
        Vector<Node*> children = this->getChildren();
        for(Node* child : children)
        {
            child -> pause();
        }

        //show pause menu;
        createPauseMenu();


        // if you use SimpleAudioEngine, it must be pause
        // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    }
}

void SceneSkipTransition::createPauseButton(){
    std::string playImageFileName = Constants::SpriteNames::pause;
    std::string playSelectedImageFileName = Constants::SpriteNames::pause;
    auto normalSprite = Sprite::createWithSpriteFrameName(playImageFileName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(playSelectedImageFileName);

    auto pause = MenuItemSprite::create(normalSprite, selectedSprite, CC_CALLBACK_1(SceneSkipTransition::receivePauseGame, this));
    pause->setPosition(selectedSprite->getContentSize().width, visibleSize.height - selectedSprite->getContentSize().height);

    Vector<MenuItem*> menuItems;
    menuItems.pushBack(pause);

    pauseButton = Menu::createWithArray(menuItems);
    pauseButton->setPosition(Vec2::ZERO);
    this->addChild(pauseButton, MENU_Z_ORDER);
}

void SceneSkipTransition::receivePauseGame(Ref* pSender){
    pauseGame();
}

void SceneSkipTransition::createPauseMenu(){
    std::string pauseImageFileName = Constants::SpriteNames::bigpause;
    std::string pauseSelectedImageFileName = Constants::SpriteNames::bigpause;
    auto normalSprite = Sprite::createWithSpriteFrameName(pauseImageFileName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(pauseSelectedImageFileName);

    auto pause = MenuItemSprite::create(normalSprite, selectedSprite, CC_CALLBACK_1(SceneSkipTransition::receiveResumeGame, this));
    pause->setPosition(visibleSize.width/2 , visibleSize.height/2);

    Vector<MenuItem*> menuItems;
    menuItems.pushBack(pause);

    pauseMenu = Menu::createWithArray(menuItems);
    pauseMenu->setPosition(Vec2::ZERO);
    this->addChild(pauseMenu, MENU_Z_ORDER);
}

void SceneSkipTransition::receiveResumeGame(Ref* pSender){

    resumeGame();
}

void SceneSkipTransition::addPauseButton(){
    pauseButton->setVisible(true);
}

void SceneSkipTransition::resumeGame(){

    if (isPaused) {
        isPaused = false;
        //Remove pause menu
        pauseMenu->removeFromParent();

        // if you use SimpleAudioEngine, it must resume here
        // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();

        //resume game
        this->scheduleUpdate();
        Vector<Node*> children = this->getChildren();
        for(Node* child : children)
        {
            child -> resume();
        }

        addPauseButton();
    }
}

template <typename T>
std::string SceneSkipTransition::numberTostring(T value)
{
    //create an output string stream
    std::ostringstream os ;

    //throw the value into the string stream
    os << value ;

    //convert the string stream into a string and return
    return os.str() ;
}

NS_RIY_END
