//
//  SceneEndGame.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "SceneEndGame.h"
#include "SceneMenuMain.h"
#include "AppConstants.h"
#include "AppMacros.h"
#include "SceneSinglePlayer.h"

USING_NS_RIY;
NS_RIY_BEGIN
Scene* SceneEndGame::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = SceneEndGame::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SceneEndGame::init()
{
    //////////////////////////////
    // 1. super init first
    //Make sure that this is the same as the scene single player color
    if ( !LayerColor::initWithColor(Color4B(153, 170, 187, 255) )) {
        return false;
    }
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    loadCongMessages();
    this->createMainMenu();
    createTrees(true);
    createTrees(false);

    this->player = new ViewPlayer(this);
    player->sprite->setPosition(gameNameLabel->getPosition().x, gameNameLabel->getPosition().y - gameNameLabel->getContentSize().height/2 - player->sprite->getContentSize().height * 2.0);
    player->sprite->removeFromParentAndCleanup(false);

    mainMenuLayer->addChild(player->sprite,PLAYER_SPRITE_Z_ORDER);

    messageIndex = 0;
    gapBetweenMessageChanges = 1;
    timeSinceLastMessageChange = 0;
    this->scheduleUpdate();
    SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::menu);

    return true;
}

void SceneEndGame::loadCongMessages(){
    congMessages.push_back("बहुत बढ़िया! खेल खत्म!");
    congMessages.push_back("Congratulations! Game Over!");
    congMessages.push_back("おめでとう！ゲームオーバー！");
    congMessages.push_back("Félicitations! Jeu Terminé!");
    congMessages.push_back("Herzlichen Glückwunsch! Spiel Vorbei!");
    congMessages.push_back("恭喜！游戏结束！");
    congMessages.push_back("恭喜！遊戲結束！");
    congMessages.push_back("Поздравления! Игра Закончена!");
    congMessages.push_back("Selamat! Tamat!");
    congMessages.push_back("מזל טוב! סוף המשחק!");
    congMessages.push_back("تهانينا! انتهت اللعبة!");
    congMessages.push_back("축하합니다! 게임 끝!");
    congMessages.push_back("¡Enhorabuena! Juego Terminado!");
    congMessages.push_back("Complimenti! Game Over!");
    congMessages.push_back("Tillykke! Spil Over!");
    congMessages.push_back("Gefeliciteerd! Game Over!");
    congMessages.push_back("Gratulacje! Game Over!");
    congMessages.push_back("Onneksi olkoon! Game Over!");
    congMessages.push_back("Gratulerer! Game Over!");
    congMessages.push_back("Grattis! Game Over!");
    congMessages.push_back("ขอแสดงความยินดี จบเกม!");
    congMessages.push_back("Enhorabona! Joc Acabat!");
    congMessages.push_back("Gratulálunk! Játék Vége!");
    congMessages.push_back("Blahopřejeme! Konec Hry!");
    congMessages.push_back("Συγχαρητήρια! Τέλος Παιχνιδιού!");
    congMessages.push_back("Blahoželáme! Koniec Hry!");
    congMessages.push_back("Čestitamo! Igra Je Gotova!");
    congMessages.push_back("Xin chúc mừng! Kết Thúc!");
    congMessages.push_back("Felicitări! Sfârșitul Jocului!");
    congMessages.push_back("Tahniah! Permainan Tamat!");
    congMessages.push_back("Поздоровлення! Гра Закінчена!");

}

void SceneEndGame::groundPieces(ArtChunk chunk){
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

void SceneEndGame::tintWithTime(Sprite* sprite,float time,Color3B color){
    TintTo* tint = TintTo::create(time, color);
    tint->setTag(TINT_ACTION_TAG);
    sprite->stopActionByTag(TINT_ACTION_TAG);
    sprite->runAction(tint);
}

void SceneEndGame::createTrees(bool onTop){
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

MenuItemSprite* SceneEndGame::createMenuItemSprite(std::string normalFrameName,std::string selectedFrameName,const ccMenuCallback& callback,float xPosition, float yPosition){
    auto normalSprite = Sprite::createWithSpriteFrameName(normalFrameName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(selectedFrameName);
    selectedSprite->setColor(Color3B(128, 128, 128));
    auto menuItemSprite = MenuItemSprite::create(normalSprite, selectedSprite, callback);
    menuItemSprite->setPosition(xPosition, yPosition);

    return menuItemSprite;
}

void SceneEndGame::createMainMenu(){
    mainMenuLayer = Node::create();
    this->addChild(mainMenuLayer,MENU_Z_ORDER);

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    gameNameLabel = Label::createWithSystemFont(congMessages[0], "Arial", 18);
#elif CC_TARGET_PLATFROM == CC_PLATFORM_ANDROID
    gameNameLabel = Label::createWithTTF(congMessages[1], "fonts/Marker Felt.ttf", 18);
#else
    gameNameLabel = Label::createWithTTF(congMessages[1], "fonts/Marker Felt.ttf", 18);
#endif

    gameNameLabel->setPosition(visibleSize.width/2, visibleSize.height * 6.0/8.0);
    mainMenuLayer->addChild(gameNameLabel,MENU_Z_ORDER);

    auto play = createMenuItemSprite(Constants::MenuImageNames::play, Constants::MenuImageNames::playselected, CC_CALLBACK_1(SceneEndGame::playCallback, this), visibleSize.width/2, visibleSize.height/2);


    Vector<MenuItem*> menuItems;
    menuItems.pushBack(play);

    menu =Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    mainMenuLayer->addChild(menu, MENU_Z_ORDER);
}

void SceneEndGame::update(float dt)
{
    updatePlayer();

#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    timeSinceLastMessageChange+=dt;
    if (timeSinceLastMessageChange > gapBetweenMessageChanges) {
        timeSinceLastMessageChange = 0;

        int nm = messageIndex%(congMessages.size());
        messageIndex++;
        if (messageIndex > congMessages.size()) {
            messageIndex = messageIndex%congMessages.size();
        }
        std::string message = congMessages[nm];
        gameNameLabel->setString(message);
    }
#endif
}

void SceneEndGame::updatePlayer(){

}

void SceneEndGame::playCallback(Ref* pSender){
    GameStatus::savePoints.clear();
    Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneSinglePlayer::createScene()));

    //    Director::getInstance()->replaceScene(TransitionShrinkGrow::create(1.0, SceneSinglePlayer::createScene()));
}

NS_RIY_END
