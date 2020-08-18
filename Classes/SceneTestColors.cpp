//
//  SceneTestColors.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "SceneTestColors.h"

USING_NS_CC;
USING_NS_RIY;

NS_RIY_BEGIN

Scene* SceneTestColors::createScene(){
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = SceneTestColors::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

bool SceneTestColors::init(){

    //////////////////////////////
    // 1. super init first
    if ( !LayerColor::initWithColor(Color4B(0, 0, 0, 255) )) {
        return false;
    }
    currentColorIndex = -1;
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    background = Sprite::createWithSpriteFrameName(Constants::SpriteNames::background);
    this->addChild(background, BACKGROUND_SPRITE_Z_ORDER);
    background->setPosition(Vec2(background->getContentSize().width/2, background->getContentSize().height/2));

    groundPieces();
    Sprite* gen = Sprite::createWithSpriteFrameName(Constants::MenuImageNames::play);
    Size genDim = gen->getContentSize();
    MenuItemSprite* next = createMenuItemSprite(Constants::MenuImageNames::play, Constants::MenuImageNames::playselected, CC_CALLBACK_1(SceneTestColors::next, this), visibleSize.width - genDim.width/2 , visibleSize.height - genDim.height/2);
    MenuItemSprite* prev = createMenuItemSprite(Constants::MenuImageNames::play, Constants::MenuImageNames::playselected, CC_CALLBACK_1(SceneTestColors::prev, this),genDim.width/2 , visibleSize.height - genDim.height/2);

    MenuItemSprite* selectNext = createMenuItemSprite(Constants::MenuImageNames::play, Constants::MenuImageNames::playselected, CC_CALLBACK_1(SceneTestColors::selectNext, this),visibleSize.width - genDim.width/2 , visibleSize.height - genDim.height*2);

    MenuItemSprite* logAll = createMenuItemSprite(Constants::MenuImageNames::play, Constants::MenuImageNames::playselected, CC_CALLBACK_1(SceneTestColors::logAll, this),genDim.width/2 , visibleSize.height - genDim.height*2);

    prev->setRotation(180);
    Vector<MenuItem*> menuItems;
    menuItems.pushBack(next);
    menuItems.pushBack(selectNext);
    menuItems.pushBack(logAll);
    menuItems.pushBack(prev);

    menu =Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, MENU_Z_ORDER);


    sun = Sprite::createWithSpriteFrameName(Constants::Space::sun);
    this->addChild(sun, SUN_SPRITE_Z_ORDER);
    sun->setPosition(Vec2(prev->getPositionX() + prev->getContentSize().width + sun->getContentSize().width/2, visibleSize.height - sun->getContentSize().height));

    moon = Sprite::createWithSpriteFrameName(Constants::Space::moon);
    this->addChild(moon, MOON_SPRITE_Z_ORDER);
    moon->setPosition(Vec2(sun->getPositionX() + sun->getContentSize().width + moon->getContentSize().width, visibleSize.height - moon->getContentSize().height));

    std::string s = numberTostring(currentColorIndex);
    scoreLabel = Label::createWithTTF(s, "fonts/Marker Felt.ttf", 24);
    scoreLabel->setPosition(Point(origin.x + visibleSize.width/2 ,origin.y + visibleSize.height - scoreLabel->getContentSize().height));
    this->addChild(scoreLabel,SCORE_LABEL_Z_ORDER);
    this->player = new ViewPlayer(this);
    player->sprite->setPosition(Vec2(visibleSize.width*0.25, visibleSize.height*0.25));
    //Transitions the background with the same trees
    //Update the trees

    //Transitions backgrouns and trees

    //Should be able to view
    //Score:
    //Trees:
    //Leaves:
    //Birds:
    //Boss
    //Save
    //Skip
    //Particles

    //Make sure you can see the rgb values of the colors so you can know which to skip.

    //Make sure to skip colorsets which don't have enough colors.


    //Tap on left half ->see transition on current
    //Tap on right half -> shift to left.

    allSets = splitString(ModelArt::getInstance()->dayStr, '|');
    CCLOG("Colorsets:%lu",allSets.size());
    this->scheduleUpdate();
    return true;
}

void SceneTestColors::next(Ref* pSender){
    createSceneFromColorSet(++currentColorIndex);
}

void SceneTestColors::prev(Ref* pSender){
    if (currentColorIndex > 0) {
        createSceneFromColorSet(--currentColorIndex);
    }
}

void SceneTestColors::selectNext(Ref* pSender){

    std::string colorString = allSets[currentColorIndex];
    totalSelected = totalSelected + colorString + "|";
    std::string output = "Saved colorset:" + colorString;
    CCLOG("%s",output.c_str());

    createSceneFromColorSet(++currentColorIndex);
}

void SceneTestColors::logAll(Ref* pSender){

    CCLOG("All:\n%s",totalSelected.c_str());
}


void SceneTestColors::createSceneFromColorSet(int index){

    std::string colorString = allSets[index];
    float transitionTime = 0.5f;

    ModelColorSet colorSet = ModelArt::getInstance()->createSortedColorSet(colorString);
    if (colorSet.colors.size() == 0) {
        CCLOG("No more colors..");
        logAll(nullptr);
        return;
    }

    ArtChunk chunk = ModelArt::getInstance()->artChunkFromColorSet(colorSet, index);

    background->stopAllActions();
    TintTo* tintBg = TintTo::create(transitionTime,chunk.bgR , chunk.bgG, chunk.bgB);
    background->runAction(tintBg);

    sun->stopAllActions();
    if (chunk.hSun) {
        sun->setVisible(true);
        TintTo* tintSun = TintTo::create(0,chunk.suR , chunk.suG, chunk.suB);
        FadeIn* fadeIn = FadeIn::create(transitionTime);
        sun->runAction(tintSun);
        sun->runAction(fadeIn);
    }else{
        sun->setVisible(false);
        sun->setOpacity(0);
    }

    moon->stopAllActions();
    if (chunk.hMoon) {
        moon->setVisible(true);
        TintTo* tintMoon = TintTo::create(transitionTime,chunk.moR , chunk.moG, chunk.moB);
        FadeIn* fadeIn = FadeIn::create(transitionTime);
        moon->runAction(tintMoon);
        moon->runAction(fadeIn);
    }else{
        moon->setVisible(false);
        moon->setOpacity(0);
    }

    scoreLabel->stopAllActions();
    TintTo* tintScore = TintTo::create(transitionTime,chunk.agR , chunk.agG, chunk.agB);
    scoreLabel->runAction(tintScore);
    std::string s = numberTostring(index);
    scoreLabel->setString(s);
    scoreLabel->setPosition(Point(origin.x + visibleSize.width/2 ,origin.y + visibleSize.height - scoreLabel->getContentSize().height));

    //Transition the background,stars,moon,sun,groundsprites

    for (int i = 0; i<lowerGroundPieces.size(); i++) {

        Sprite* ls;
        Sprite* ms;
        Sprite* ts;
        Sprite* ab;
        float timeLeft = 2;


        ls = lowerGroundPieces[i];
        ms = midGroundPieces[i];
        ts = topGroundPieces[i];
        ab = aboveGroundPieces[i];

        tintWithTime(ab, timeLeft, Color3B(chunk.agR, chunk.agG, chunk.agB));
        tintWithTime(ls, timeLeft, Color3B(chunk.lgR, chunk.lgG, chunk.lgB));
        tintWithTime(ms, timeLeft, Color3B(chunk.mgR, chunk.mgG, chunk.mgB));
        tintWithTime(ts, timeLeft, Color3B(chunk.tgR, chunk.tgG, chunk.tgB));

        for (Node* child : ts->getChildren())
        {
            if (dynamic_cast<Sprite*>(child)!=NULL) {
                Sprite* sprite = (Sprite*) child;
                tintWithTime(sprite, timeLeft, Color3B(chunk.gfR, chunk.gfG, chunk.gfB));
            }
        }

    }

    //particles,flowerjets


    //Move the trees, birds, boss out / in
    //move existing bird out.
    //set position
    if (bird!=nullptr) {
        bird->stopAllActions();
        auto moveOff = MoveTo::create(transitionTime,Vec2(0 - visibleSize.width/10,bird->getPositionY()));
        Sprite* s = bird;
        auto remove = CallFunc::create([s](){
            s->removeFromParent();
        });
        auto seq = Sequence::createWithTwoActions(moveOff, remove);

        s->runAction(seq);
    }

    Vec2 startPosition = Vec2(visibleSize.width*1.5,visibleSize.height/2);
    Vec2 modelPosition = Vec2(visibleSize.width/2,visibleSize.height/2);;

    Sprite *emptypixel = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
    Sprite *rearwing = Sprite::createWithSpriteFrameName(chunk.bwF );
    Sprite *beak = Sprite::createWithSpriteFrameName(Constants::Bird::beak );
    Sprite *body = Sprite::createWithSpriteFrameName(Constants::Bird::body );
    Sprite *eye = Sprite::createWithSpriteFrameName(Constants::Bird::eye );
    Sprite *eyeball = Sprite::createWithSpriteFrameName(Constants::Bird::eyeball );
    Sprite *frontwing = Sprite::createWithSpriteFrameName(Constants::Bird::wing1 );

    emptypixel->addChild(rearwing);
    emptypixel->addChild(beak);
    emptypixel->addChild(body);
    emptypixel->addChild(eye);
    emptypixel->addChild(eyeball);
    emptypixel->addChild(frontwing);

    rearwing->setColor(Color3B(chunk.bwR, chunk.bwG, chunk.bwB));
    frontwing->setColor(Color3B(chunk.bwR, chunk.bwG, chunk.bwB));
    beak->setColor(Color3B(chunk.bkR, chunk.bkG, chunk.bkB));
    body->setColor(Color3B(chunk.bbR, chunk.bbG, chunk.bbB));
    eye->setColor(Color3B(chunk.byR, chunk.byG, chunk.byB));
    eyeball->setColor(Color3B(chunk.beR, chunk.beG, chunk.beB));

    beak->setPosition(Vec2(0 - body->getContentSize().width/2, 0));

    rearwing->setRotation(270);
    rearwing->setOpacity(200);

    rearwing->setPosition(Vec2(0, 0 - body->getContentSize().height/4));
    frontwing->setPosition(Vec2(0, 0 -body->getContentSize().height/4));

    auto rotateRearWingDown = RotateTo::create(0.5, 360);
    auto rotateRearWingUp = RotateTo::create(0.5, 270);
    auto rearSeq = Sequence::createWithTwoActions(rotateRearWingDown, rotateRearWingUp);
    auto repeatRear = RepeatForever::create(rearSeq);
    rearwing->runAction(repeatRear);

    auto rotateForeWingUp = RotateTo::create(0.5, 270);
    auto rotateForeWingDown = RotateTo::create(0.5, 360);
    auto foreSeq = Sequence::createWithTwoActions(rotateForeWingUp, rotateForeWingDown);
    auto repeatFront = RepeatForever::create(foreSeq);
    frontwing->runAction(repeatFront);

    Vec2 position = startPosition;
    emptypixel->setPosition(position);
    auto moveIn = MoveTo::create(transitionTime,Vec2(modelPosition.x - body->getContentSize().width*2, modelPosition.y));
    this->addChild(emptypixel, BAD_FOOD_SPRITE_Z_ORDER);
    bird = emptypixel;
    bird->runAction(moveIn);

    if (tree!=nullptr) {
        tree->stopAllActions();
        auto moveOff = MoveTo::create(transitionTime,Vec2(0 - visibleSize.width/10,tree->getPositionY()));
        Sprite* s = tree;
        auto remove = CallFunc::create([s](){
            s->removeFromParent();
        });
        auto seq = Sequence::createWithTwoActions(moveOff, remove);

        s->runAction(seq);
    }

    Sprite* trunk = Sprite::createWithSpriteFrameName(chunk.obstructionFrame + "t");
    tree = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel);
    tree->setCascadeOpacityEnabled(true);
    tree->addChild(trunk);
    trunk->setColor(Color3B(chunk.trR, chunk.trG, chunk.trB));

    if(chunk.hasLeaves){
        Sprite *leaves = Sprite::createWithSpriteFrameName(chunk.obstructionFrame + "l");
        tree->addChild(leaves);
        leaves->setColor(Color3B(chunk.tlR, chunk.tlG, chunk.tlB));
    }

    position = Vec2(startPosition);
    tree->setPosition(position);
    this->addChild(tree, OBSTRUCTION_SPRITE_Z_ORDER
                   );
    auto moveTreeIn = MoveTo::create(transitionTime,modelPosition);
    tree->runAction(moveTreeIn);

    if (boss!=nullptr) {
        boss->stopAllActions();
        auto moveOff = MoveTo::create(transitionTime,Vec2(0 - visibleSize.width/10,tree->getPositionY()));
        Sprite* s = boss;
        auto remove = CallFunc::create([s](){
            s->removeFromParent();
        });
        auto seq = Sequence::createWithTwoActions(moveOff, remove);

        s->runAction(seq);
    }

    float timePeriod = 0.5;
    float angle = 45;
    float angleInc = 45;
    Sprite *emptypixelTop,*emptypixelBottom;
    Sprite *badFoodTop,*badFoodBottom;
    Sprite *badTeethTop,*badTeethBottom;
    Sprite *stem,*stemLeaves,*pot;

    badTeethTop = Sprite::createWithSpriteFrameName(Constants::SpriteNames::bossTeeth );
    badTeethTop->setFlippedY(true);
    badTeethBottom = Sprite::createWithSpriteFrameName(Constants::SpriteNames::bossTeeth );
    emptypixel = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
    emptypixelTop = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
    emptypixelBottom = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );

    std::string treeIndex = chunk.obstructionFrame;

    stem = Sprite::createWithSpriteFrameName("s" + treeIndex + "t");
    emptypixel->addChild(stem);

    if (chunk.hasLeaves) {
        stemLeaves = Sprite::createWithSpriteFrameName("s" + treeIndex + "l");
        stemLeaves->setColor(Color3B(chunk.tlR,chunk.tlG,chunk.tlB));
        stemLeaves->setAnchorPoint(Vec2(0.5,1.0));
        emptypixel->addChild(stemLeaves);
    }


    stem->setColor(Color3B(chunk.trR,chunk.trG,chunk.trB));

    pot = Sprite::createWithSpriteFrameName(Constants::Boss::pot);
    pot->setColor(Color3B(chunk.lgR, chunk.lgG, chunk.lgB));

    Vec2 collarPos = Vec2(stem->getContentSize().width/2, stem->getContentSize().height/2);
    Vec2 collarPosf = Vec2(stem->getContentSize().width/2, stem->getContentSize().height/4);

    std::string rearBranch = "br1" + treeIndex;
    std::string foreBranch = "br2" + treeIndex;

    for (int i = 0; i<4; i++) {
        int startAngle;
        int endAngle;
        Vec2 shoulderPos;
        std::string branchName;


        switch (i) {
            case 0:
            case 2:
            {//right
                startAngle = angle;
                endAngle = angle + angleInc;
                break;
            }
            case 1:
            case 3:
            {//left
                startAngle = angle + 270;
                endAngle = startAngle - angleInc;
                break;
            }
            default:
                break;
        }

        switch (i) {
            case 0:
            case 1:
            {//rear
                shoulderPos = collarPos;
                branchName = rearBranch;
                break;
            }
            case 2:
            case 3:
            {//fore
                shoulderPos = collarPosf;
                branchName = foreBranch;
                break;
            }
            default:
                break;
        }

        Sprite* arm = Sprite::createWithSpriteFrameName("s" + treeIndex + "t");
        arm->setColor(Color3B(chunk.trR, chunk.trG, chunk.trB));
        if (chunk.hasLeaves) {
            Sprite* leaves = Sprite::createWithSpriteFrameName("s" + treeIndex + "l");
            leaves->setColor(Color3B(chunk.tlR, chunk.tlG, chunk.tlB));

            leaves->setRotation(startAngle);
            leaves->setAnchorPoint(Vec2(0.5, 0.0));
            leaves->setPosition(shoulderPos);
            RotateTo *drh = RotateTo::create(timePeriod,endAngle);
            RotateTo *urh = RotateTo::create(timePeriod,startAngle);
            Sequence *mrh = Sequence::createWithTwoActions(drh , urh);
            RepeatForever *mrhf = RepeatForever::create(mrh);
            leaves->setScale(0.5);
            leaves->runAction(mrhf);
            stem->addChild(leaves);
        }

        arm->setRotation(startAngle);
        arm->setAnchorPoint(Vec2(0.5, 0.0));
        arm->setPosition(shoulderPos);
        RotateTo *drh = RotateTo::create(timePeriod,endAngle);
        RotateTo *urh = RotateTo::create(timePeriod,startAngle);
        Sequence *mrh = Sequence::createWithTwoActions(drh , urh);
        RepeatForever *mrhf = RepeatForever::create(mrh);
        arm->setScale(0.5);
        arm->runAction(mrhf);
        stem->addChild(arm);
    }



    emptypixel->addChild(pot);
    emptypixel->addChild(emptypixelTop);
    emptypixel->addChild(emptypixelBottom);

    badFoodTop = Sprite::createWithSpriteFrameName(Constants::SpriteNames::pizzaBoss + Constants::SpriteNames::top);
    badFoodBottom = Sprite::createWithSpriteFrameName(Constants::SpriteNames::pizzaBoss + Constants::SpriteNames::bottom);

    badFoodTop->setColor(Color3B(chunk.bgR,chunk.bgG,chunk.bgB));
    badFoodBottom->setColor(Color3B(chunk.bgR,chunk.bgG,chunk.bgB));
    badFoodTop->setAnchorPoint(Vec2(0.5, 0));
    badFoodBottom->setAnchorPoint(Vec2(0.5,1));
    badTeethTop->setAnchorPoint(Vec2(1, 1));
    badTeethBottom->setAnchorPoint(Vec2(1, 0));

    badFoodTop->setOpacity(0);
    badFoodBottom->setOpacity(0);

    int num = 5;
    for (int i = 0; i< num; i++){
        Sprite *uppert = Sprite::createWithSpriteFrameName("s" + treeIndex + "t");
        uppert->setPosition(badFoodTop->getContentSize().width/2, 0);
        uppert->setRotation(0 - 90 + (180/num)*i);
        uppert->setColor(Color3B(chunk.trR, chunk.trG, chunk.trB));
        badFoodTop->addChild(uppert);
        uppert->setScale(0.5);
        uppert->setAnchorPoint(Vec2(0.5, 0.0));


        if (chunk.hasLeaves) {
            Sprite *upperl = Sprite::createWithSpriteFrameName("s" + treeIndex + "l");
            upperl->setPosition(badFoodTop->getContentSize().width/2, 0);
            upperl->setRotation(0 - 90 + (180/num)*i);
            upperl->setColor(Color3B(chunk.tlR, chunk.tlG, chunk.tlB));
            upperl->setScale(0.5);
            upperl->setAnchorPoint(Vec2(0.5, 0.0));
            badFoodTop->addChild(upperl);
        }
    }

    for (int i = 0; i< num; i++){
        Sprite *uppert = Sprite::createWithSpriteFrameName("s" + treeIndex + "t");
        uppert->setPosition(badFoodTop->getContentSize().width/2, badFoodTop->getContentSize().height);
        uppert->setRotation(90 + (180/num)*i);
        uppert->setColor(Color3B(chunk.trR, chunk.trG, chunk.trB));
        uppert->setScale(0.5);
        uppert->setAnchorPoint(Vec2(0.5, 0.0));
        badFoodBottom->addChild(uppert);


        if (chunk.hasLeaves) {
            Sprite *upperl = Sprite::createWithSpriteFrameName("s" + treeIndex + "l");
            upperl->setPosition(badFoodTop->getContentSize().width/2, badFoodTop->getContentSize().height);
            upperl->setRotation(90 + (180/num)*i);
            upperl->setColor(Color3B(chunk.tlR, chunk.tlG, chunk.tlB));
            upperl->setScale(0.5);
            upperl->setAnchorPoint(Vec2(0.5, 0.0));
            badFoodBottom->addChild(upperl);
        }
    }

    stem->setPosition(Vec2(0,0 - badFoodBottom->getContentSize().height/2));
    pot->setPosition(stem->getPosition());
    stem->setAnchorPoint(Vec2(0.5,1.0));
    pot->setAnchorPoint(stem->getAnchorPoint());

    eyeball = Sprite::createWithSpriteFrameName(Constants::Bird::eye);
    eyeball->setPosition(Vec2(badFoodTop->getContentSize().width/4, badFoodTop->getContentSize().height/4));
    badFoodTop->addChild(eyeball);
    eyeball->setColor(Color3B(chunk.byR, chunk.byG, chunk.byB));

    RotateTo *openUpperJaw = RotateTo::create(timePeriod,angle);
    RotateTo *closeUpperJaw = RotateTo::create(timePeriod,0);
    Sequence *seq = Sequence::createWithTwoActions(openUpperJaw, closeUpperJaw);
    RepeatForever *repeatForever = RepeatForever::create(seq);
    emptypixelTop->runAction(repeatForever);

    RotateTo *openLowerJaw = RotateTo::create(timePeriod,360 - angle);
    RotateTo *closeLowerJaw = RotateTo::create(timePeriod,0);
    Sequence *seq2 = Sequence::createWithTwoActions(openLowerJaw, closeLowerJaw);
    RepeatForever *repeatForever2 = RepeatForever::create(seq2);
    emptypixelBottom->runAction(repeatForever2);

    emptypixelTop->addChild(badTeethTop);
    emptypixelBottom->addChild(badTeethBottom);
    emptypixelTop->addChild(badFoodTop);
    emptypixelBottom->addChild(badFoodBottom);

    emptypixel->setPosition(position);
    this->addChild(emptypixel, BOSS_SPRITE_Z_ORDER);
    boss = emptypixel;

    auto moveBossIn = MoveTo::create(transitionTime,Vec2(modelPosition.x + trunk->getContentSize().width, modelPosition.y));
    boss->runAction(moveBossIn);
}

void SceneTestColors::tintWithTime(Sprite* sprite,float time,Color3B color){
    TintTo* tint = TintTo::create(time, color);
    tint->setTag(TINT_ACTION_TAG);
    sprite->stopActionByTag(TINT_ACTION_TAG);
    sprite->runAction(tint);
}

void SceneTestColors::groundPieces(){
    Sprite *tg = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundlower);
    Sprite *ug = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundupper );
    Sprite *top = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundTop );
    Size lowerSize = tg->getContentSize();
    Size upperSize = ug->getContentSize();
    Size topSize = top->getContentSize();

    float wid = 360;
    float swid = tg->getContentSize().width;
    int number = (wid / swid) + 1;

    float xOffset = 0.0;
    for (int j=0; j<number; j++) {
        Sprite *groundSprite = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundlower);
        Sprite *upper = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundupper );
        Sprite *top = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundTop );

        Vec2 lowerPos = Vec2(groundSprite->getContentSize().width/2+xOffset,groundSprite->getContentSize().height/2);

        groundSprite->setPosition(lowerPos);
        upper->setPosition(lowerPos.x, lowerSize.height + upper->getContentSize().height/2);
        top->setPosition(lowerPos.x, lowerSize.height + upperSize.height + top->getContentSize().height/2);

        Sprite *above = Sprite::createWithSpriteFrameName(Constants::SpriteNames::aboveground );
        above->setPosition(lowerPos.x, lowerSize.height + upperSize.height + topSize.height + above->getContentSize().height/2);

        this->addChild(above,ABOVE_GROUND_SPRITE_Z_ORDER);
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

        this->addChild(top,GROUND_SPRITE_Z_ORDER);
        this->addChild(upper,GROUND_SPRITE_Z_ORDER);
        this->addChild(groundSprite,GROUND_SPRITE_Z_ORDER);

        lowerGroundPieces.push_back(groundSprite);
        midGroundPieces.push_back(upper);
        topGroundPieces.push_back(top);
    }
}


template <typename T>
std::string SceneTestColors::numberTostring(T value)
{
    //create an output string stream
    std::ostringstream os ;

    //throw the value into the string stream
    os << value ;

    //convert the string stream into a string and return
    return os.str() ;
}

void SceneTestColors::update(float dt){

}

MenuItemSprite* SceneTestColors::createMenuItemSprite(std::string normalFrameName,std::string selectedFrameName,const ccMenuCallback& callback,float xPosition, float yPosition){
    auto normalSprite = Sprite::createWithSpriteFrameName(normalFrameName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(selectedFrameName);
    selectedSprite->setColor(Color3B(128, 128, 128));
    auto menuItemSprite = MenuItemSprite::create(normalSprite, selectedSprite, callback);
    menuItemSprite->setPosition(xPosition, yPosition);

    return menuItemSprite;
}

std::vector<std::string> SceneTestColors::splitString(std::string input, char delim){
    std::stringstream test(input);
    std::string segment;
    std::vector<std::string> seglist;
    while(std::getline(test, segment, delim))
    {
        if (segment != "") {
            seglist.push_back(segment);
        }
    }
    return seglist;
}

NS_RIY_END
