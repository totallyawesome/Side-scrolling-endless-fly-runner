#include "SceneMenuMain.h"
#include "AppConstants.h"
#include "AppMacros.h"
#include "SceneSinglePlayer.h"
#include "SceneCredits.h"
#include "SceneInstructions.h"
#include "JNIBridge.h"

USING_NS_RIY;
NS_RIY_BEGIN
Scene* SceneMenuMain::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = SceneMenuMain::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SceneMenuMain::init()
{
    //////////////////////////////
    // 1. super init first
    //Make sure that this is the same as the scene single player color
    if ( !LayerColor::initWithColor(Color4B(153, 170, 187, 255) )) {
        return false;
    }
    isStoreLoaded = false;
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    this->createMainMenu();
    createTrees(true);
    createTrees(false);

    this->player = new ViewPlayer(this);
    player->sprite->setPosition(gameNameLabel->getPosition().x, gameNameLabel->getPosition().y - gameNameLabel->getContentSize().height/2 - player->sprite->getContentSize().height * 2.0);
    player->sprite->removeFromParentAndCleanup(false);

    mainMenuLayer->addChild(player->sprite,PLAYER_SPRITE_Z_ORDER);

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    gch = new GameCenterHandlerC();
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    if(!JniBridge::isAdShowing){
        JniBridge::showAds();
    }
#endif
    this->scheduleUpdate();
    return true;
}

void SceneMenuMain::groundPieces(ArtChunk chunk){
    Sprite *tg = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundlower);
    Sprite *ug = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundupper );
    Sprite *top = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundTop );
    Size lowerSize = tg->getContentSize();
    Size upperSize = ug->getContentSize();
    Size topSize = top->getContentSize();

    float wid = 400;
    float swid = tg->getContentSize().width;
    int number = (wid / swid) + 1;

    float xOffset = 0.0;
    for (int j=0; j<number; j++) {
        Sprite *groundSprite = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundlower);
        Sprite *upper = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundupper );
        Sprite *top = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundTop );

        Vec2 lowerPos = Vec2(0 - 2*groundSprite->getContentSize().width/2 + xOffset,groundSprite->getContentSize().height/2);

        groundSprite->setPosition(lowerPos);
        upper->setPosition(lowerPos.x, lowerSize.height + upper->getContentSize().height/2);
        top->setPosition(lowerPos.x, lowerSize.height + upperSize.height + top->getContentSize().height/2);

        Sprite *above = Sprite::createWithSpriteFrameName(Constants::SpriteNames::aboveground );
        above->setPosition(lowerPos.x, lowerSize.height + upperSize.height + topSize.height + above->getContentSize().height/2);

        mainMenuLayer->addChild(above,ABOVE_GROUND_SPRITE_Z_ORDER);
        aboveGroundPieces.push_back(above);


        xOffset += lowerSize.width;

        int nofGFl = 1 + arc4random()%3; // Number fo ground flowers
        Size size = top->getContentSize();
        for (int i = 0; i<nofGFl; i++) {
            Sprite* grf = Sprite::createWithSpriteFrameName(Constants::SpriteNames::flowerjet);
            float randX = arc4random()%((int)size.width);
            float randY = arc4random()%((int)size.height);
            grf->setPosition(randX, randY);
            top->addChild(grf);
            grf->setScale(0.5);
        }

        mainMenuLayer->addChild(top,GROUND_SPRITE_Z_ORDER);
        mainMenuLayer->addChild(upper,GROUND_SPRITE_Z_ORDER);
        mainMenuLayer->addChild(groundSprite,GROUND_SPRITE_Z_ORDER);

        lowerGroundPieces.push_back(groundSprite);
        midGroundPieces.push_back(upper);
        topGroundPieces.push_back(top);

        above->setColor(Color3B(chunk.agR, chunk.agG, chunk.agB));
        groundSprite->setColor(Color3B(chunk.lgR, chunk.lgG, chunk.lgB));
        upper->setColor(Color3B(chunk.mgR, chunk.mgG, chunk.mgB));
        top->setColor(Color3B(chunk.tgR, chunk.tgG, chunk.tgB));
    }
}

void SceneMenuMain::tintWithTime(Sprite* sprite,float time,Color3B color){
    TintTo* tint = TintTo::create(time, color);
    tint->setTag(TINT_ACTION_TAG);
    sprite->stopActionByTag(TINT_ACTION_TAG);
    sprite->runAction(tint);
}

void SceneMenuMain::createTrees(bool onTop){
    ArtChunk currentArt = ModelArt::getInstance()->artChunkForASegmentIndex(1);
    groundPieces(currentArt);

    Sprite* sun = Sprite::createWithSpriteFrameName(Constants::Space::sun);
    sun->setColor(Color3B(currentArt.suR, currentArt.suG, currentArt.suB));
    sun->setPosition(visibleSize.width - sun->getContentSize().width, visibleSize.height - sun->getContentSize().height);
    mainMenuLayer->addChild(sun, SUN_SPRITE_Z_ORDER);

    background = Sprite::createWithSpriteFrameName(Constants::SpriteNames::background);
    background->setColor(Color3B(currentArt.bgR, currentArt.bgG, currentArt.bgB));
    background->setPosition(background->getContentSize().width/2, background->getContentSize().height/2);
    mainMenuLayer->addChild(background, BACKGROUND_SPRITE_Z_ORDER);
    int totalTrees = 14;
    for (int i=0; i<totalTrees; i++) {
        float xOffset = visibleSize.width / (totalTrees-2);
        Sprite* empty = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel);
        Sprite *tree = Sprite::createWithSpriteFrameName(currentArt.obstructionFrame + "t");
        empty->addChild(tree);

        int randFlip = i%2;
        if (randFlip) {
            tree->setFlippedX(true);
        }

        tree->setColor(Color3B(currentArt.trR,currentArt.trG,currentArt.trB));
        if(currentArt.hasLeaves){
            Sprite *leaves = Sprite::createWithSpriteFrameName(currentArt.obstructionFrame + "l");
            leaves->setColor(Color3B(currentArt.tlR, currentArt.tlG,currentArt.tlB));
            empty->addChild(leaves);

            if (randFlip) {
                leaves->setFlippedX(true);
            }

            if (onTop) {
                leaves->setFlippedY(true);
            }
        }

        float y = tree->getContentSize().height/4;
        float sinx = sin((180.0 + (180.0/ ((float)totalTrees) * i) )*((M_PI)/180.0));
        y = sinx * y + tree->getContentSize().height/4;

        if (onTop) {
            tree->setFlippedY(true);

            empty->setPosition(i*xOffset,  visibleSize.height - y);

        }else{
            empty->setPosition(i*xOffset,  y);
        }

        if (i%2 == 0) {
            mainMenuLayer->addChild(empty,OBSTRUCTION_SPRITE_Z_ORDER);
        }
    }
}

MenuItemSprite* SceneMenuMain::createMenuItemSprite(std::string normalFrameName,std::string selectedFrameName,const ccMenuCallback& callback,float xPosition, float yPosition){
    auto normalSprite = Sprite::createWithSpriteFrameName(normalFrameName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(selectedFrameName);
    selectedSprite->setColor(Color3B(128, 128, 128));
    auto menuItemSprite = MenuItemSprite::create(normalSprite, selectedSprite, callback);
    menuItemSprite->setPosition(xPosition, yPosition);

    return menuItemSprite;
}

void SceneMenuMain::createMainMenu(){
    mainMenuLayer = Node::create();
    this->addChild(mainMenuLayer,MENU_Z_ORDER);

    gameNameLabel = Label::createWithTTF("Sidescroller", "fonts/Marker Felt.ttf", 24);
    gameNameLabel->setPosition(visibleSize.width/2, visibleSize.height * 6.0/8.0);
    mainMenuLayer->addChild(gameNameLabel,MENU_Z_ORDER);

    auto play = createMenuItemSprite(Constants::MenuImageNames::play, Constants::MenuImageNames::playselected, CC_CALLBACK_1(SceneMenuMain::playCallback, this), visibleSize.width/4, visibleSize.height/2);

    auto rate = createMenuItemSprite(Constants::MenuImageNames::rate, Constants::MenuImageNames::rateselected, CC_CALLBACK_1(SceneMenuMain::rateCallback, this), visibleSize.width/2, visibleSize.height/2);

    auto ranking = createMenuItemSprite(Constants::MenuImageNames::rankings, Constants::MenuImageNames::rankingsselected, CC_CALLBACK_1(SceneMenuMain::rankingCallback, this), visibleSize.width*3/4, visibleSize.height/2);

    auto questionMark = Label::createWithTTF("(?)", "fonts/Marker Felt.ttf", 32);
    auto helpButton = MenuItemLabel::create(questionMark, CC_CALLBACK_1(SceneMenuMain::showHelpCallback, this));
    helpButton->setPosition(Vec2(questionMark->getContentSize().width*1.1, visibleSize.height - questionMark->getContentSize().height*1.1));

    Sprite* copyFrame = Sprite::createWithSpriteFrameName(Constants::MenuImageNames::copyright);
    auto credits = createMenuItemSprite(Constants::MenuImageNames::copyright, Constants::MenuImageNames::copyrightselected, CC_CALLBACK_1(SceneMenuMain::creditsCallBack, this), visibleSize.width - copyFrame->getContentSize().width*1.1, 80 + copyFrame->getContentSize().height/2);

    Sprite* colorFrame = Sprite::createWithSpriteFrameName(Constants::MenuImageNames::colors);

    audioButtonOnTex = Director::getInstance()->getTextureCache()->addImage(Constants::MenuImageNames::musicon);
    audioButtonOffTex = Director::getInstance()->getTextureCache()->addImage(Constants::MenuImageNames::musicoff);

    if (SoundManager::getInstance()->isMuted()) {
        audioButton = Sprite::create(Constants::MenuImageNames::musicoff);
        audioButtonSelected = Sprite::create(Constants::MenuImageNames::musicon);
    }else{
        audioButton = Sprite::create(Constants::MenuImageNames::musicon);
        audioButtonSelected = Sprite::create(Constants::MenuImageNames::musicoff);
    }

    audioButtonSelected->setColor(Color3B(128, 128, 128));
    auto musiconoff = MenuItemSprite::create(audioButton, audioButtonSelected, CC_CALLBACK_1(SceneMenuMain::toggleSFXCallback, this));
    musiconoff->setPosition(visibleSize.width/2, visibleSize.height/4);

    Vector<MenuItem*> menuItems;
    menuItems.pushBack(play);
    menuItems.pushBack(rate);
    menuItems.pushBack(ranking);
    menuItems.pushBack(credits);
    menuItems.pushBack(musiconoff);
    menuItems.pushBack(helpButton);

    if (ModelArt::getInstance()->isBW) {
        auto colors = createMenuItemSprite(Constants::MenuImageNames::colors, Constants::MenuImageNames::colorselected, CC_CALLBACK_1(SceneMenuMain::colorsCallBack, this), colorFrame->getContentSize().width*1.1, 80 + copyFrame->getContentSize().height/2);

        menuItems.pushBack(colors);
    }else{
        auto colors = createMenuItemSprite(Constants::MenuImageNames::bw, Constants::MenuImageNames::bwselected, CC_CALLBACK_1(SceneMenuMain::colorsCallBack, this), colorFrame->getContentSize().width*1.1, 80 + copyFrame->getContentSize().height/2);

        menuItems.pushBack(colors);
    }

    menu =Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    mainMenuLayer->addChild(menu, MENU_Z_ORDER);
}

void SceneMenuMain::update(float dt)
{
    updatePlayer();

}

void SceneMenuMain::updatePlayer(){

}

void SceneMenuMain::rankingCallback(Ref* pSender){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    gch->showLeaderboard(Constants::LeaderboardIds::defaultLeaderboardId);
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniBridge::showLeaderboard();
#endif
}

void SceneMenuMain::rateCallback(Ref* pSender)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    gch->rateApp();
#endif

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniBridge::rate();
#endif

}

void SceneMenuMain::creditsCallBack(Ref* pSender)
{
    Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneCredits::createScene()));
}

void SceneMenuMain::colorsCallBack(Ref* pSender)
{
    ModelArt::getInstance()->isBW = !ModelArt::getInstance()->isBW;
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, SceneMenuMain::createScene()));
}

void SceneMenuMain::playCallback(Ref* pSender){
    GameStatus::savePoints.clear();
    GameStatus::birdsBlasted = 0;
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneSinglePlayer::createScene()));
}

void SceneMenuMain::showHelpCallback(Ref* pSender){
    Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneInstructions::createScene()));
}

void SceneMenuMain::toggleSFXCallback(Ref* pSender){
    SoundManager::getInstance()->toggleAudio();

    if (SoundManager::getInstance()->isMuted()) {
        audioButton->setTexture(audioButtonOffTex);
        audioButtonSelected->setTexture(audioButtonOnTex);
    }else{
        audioButton->setTexture(audioButtonOnTex);
        audioButtonSelected->setTexture(audioButtonOffTex);
        SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::checkpoint);
    }
}

NS_RIY_END
