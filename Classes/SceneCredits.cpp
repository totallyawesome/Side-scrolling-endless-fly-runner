//
//  SceneCredits.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "SceneCredits.h"
#include "SceneMenuMain.h"
#include "AppConstants.h"
#include "AppMacros.h"

USING_NS_RIY;
NS_RIY_BEGIN
Scene* SceneCredits::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = SceneCredits::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SceneCredits::init()
{
    //////////////////////////////
    // 1. super init first
    //Make sure that this is the same as the scene single player color
    if ( !LayerColor::initWithColor(Color4B(153, 170, 187, 255) )) {
        return false;
    }

    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();

    this->createMainMenu();
    createTrees(true);
    createTrees(false);

    return true;
}

void SceneCredits::groundPieces(ArtChunk chunk){
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

void SceneCredits::tintWithTime(Sprite* sprite,float time,Color3B color){
    TintTo* tint = TintTo::create(time, color);
    tint->setTag(TINT_ACTION_TAG);
    sprite->stopActionByTag(TINT_ACTION_TAG);
    sprite->runAction(tint);
}

void SceneCredits::createTrees(bool onTop){
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

MenuItemSprite* SceneCredits::createMenuItemSprite(std::string normalFrameName,std::string selectedFrameName,const ccMenuCallback& callback,float xPosition, float yPosition){
    auto normalSprite = Sprite::createWithSpriteFrameName(normalFrameName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(selectedFrameName);
    selectedSprite->setColor(Color3B(128, 128, 128));
    auto menuItemSprite = MenuItemSprite::create(normalSprite, selectedSprite, callback);
    menuItemSprite->setPosition(xPosition, yPosition);

    return menuItemSprite;
}

void SceneCredits::createMainMenu(){
    mainMenuLayer = Node::create();
    this->addChild(mainMenuLayer,MENU_Z_ORDER);

    std::string creditsLine = "Farterson\n Copyright 2015 Rahul Iyer\nSome art licensed via Vector Stock.\n\n No part of this production may be used without explicit written permission of the copyright holders.\n\nCredits\n Art,Sound,Code by Rahul Iyer. \n\nThanks to:\n My family, Yan, Kabir, Sunaina";

    gameNameLabel = Label::createWithTTF(creditsLine, "fonts/Marker Felt.ttf", 14,Size(visibleSize.width*3/4, visibleSize.height*3/4),TextHAlignment::CENTER,
                                         TextVAlignment::TOP);
    gameNameLabel->setPosition(visibleSize.width/2, visibleSize.height/2);
    mainMenuLayer->addChild(gameNameLabel,MENU_Z_ORDER);

    Sprite *backSprite = Sprite::createWithSpriteFrameName(Constants::MenuImageNames::back);
    auto back = createMenuItemSprite(Constants::MenuImageNames::back, Constants::MenuImageNames::backselected, CC_CALLBACK_1(SceneCredits::backCallback, this), backSprite->getContentSize().width*1.1, visibleSize.height - backSprite->getContentSize().height*1.1);


    Vector<MenuItem*> menuItems;
    menuItems.pushBack(back);

    menu =Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    mainMenuLayer->addChild(menu, MENU_Z_ORDER);
}

void SceneCredits::backCallback(Ref* pSender){
    Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneMenuMain::createScene()));
}

NS_RIY_END
