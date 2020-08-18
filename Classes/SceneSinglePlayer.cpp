//
//  SceneSinglePlayer.cpp
//  muchly
//
//  Created by Rahul Iyer on 14/06/14.
//
//

#include "SceneMenuMain.h"
#include "SceneSinglePlayer.h"
#include "SceneSkipTransition.h"
#include "SceneEndGame.h"
#include "JNIBridge.h"

USING_NS_CC;
USING_NS_RIY;

NS_RIY_BEGIN
Scene* SceneSinglePlayer::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();
    
    // 'layer' is an autorelease object
    auto layer = SceneSinglePlayer::create();
    
    // add layer as a child to scene
    scene->addChild(layer);
    
    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool SceneSinglePlayer::init()
{
    
    //////////////////////////////
    // 1. super init first
    if ( !Layer::init()) {
        return false;
    }
    
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    gch = new GameCenterHandlerC();
#endif
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniBridge::hideAds();
#endif
    
    visibleSize = Director::getInstance()->getVisibleSize();
    origin = Director::getInstance()->getVisibleOrigin();
    currentArtHolder = 0;
    winSize = Director::getInstance()->getWinSize();
    
    sun = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel);
    moon = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel);
    Vec2 defaultSunMoonLocation =   Vec2(winSize.width/2, winSize.height * 7.0f/8.0f);
    this->addChild(sun, SUN_SPRITE_Z_ORDER);
    this->addChild(moon, MOON_SPRITE_Z_ORDER);
    sun->setPosition(defaultSunMoonLocation);
    moon->setPosition(defaultSunMoonLocation);
    
    
    isPaused = false;
    jetCounter = 0;
    isGameStarted = false;
    isGameStopped = false;
    isTesting = false;
    
    score = 0;
    bestScore = UserDefault::getInstance()->getIntegerForKey(Constants::UserDefaults::bestScore.c_str());
    bestDistance = UserDefault::getInstance()->getIntegerForKey(Constants::UserDefaults::bestDistance.c_str());
    
    createPauseButton();
    
    std::string s = numberTostring(score);
    scoreLabel = Label::createWithTTF(s, "fonts/Marker Felt.ttf", 24); //scoreLabel Size
    checkpointSpriteLabelInGame = Sprite::createWithSpriteFrameName(Constants::SpriteNames::oneUp);
    checkpointSpriteLabelInGame->setPosition(pausePosition.x, visibleSize.height - pauseSpriteHeight - checkpointSpriteLabelInGame->getContentSize().height*2);
    checkPos = checkpointSpriteLabelInGame->getPosition();
    checkpointsCollectedLabel = Label::createWithTTF("x 0", "fonts/Marker Felt.ttf", 16);
    
    this->addChild(scoreLabel,SCORE_LABEL_Z_ORDER);
    this->addChild(checkpointSpriteLabelInGame,SCORE_LABEL_Z_ORDER);
    this->addChild(checkpointsCollectedLabel, SCORE_LABEL_Z_ORDER);
    
    auto singleTouchListener = EventListenerTouchOneByOne::create();
    singleTouchListener->setSwallowTouches(true);
    singleTouchListener->onTouchBegan = CC_CALLBACK_2(SceneSinglePlayer::onTouchBegan, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(singleTouchListener, this);
    
    totalTime = 0.0;
    
    this->model = new ModelWorld(false);
    this->player = new ViewPlayer(this);
    
    showInstructions();
    updateScoreLabel();
    updateCheckpointsCollectedLabel();
    
    if (model->justWarped) {
        if (!isGameStarted) {
            isGameStarted = true;
            hideInstruction();
        }
    }
    
    createGroundSpritesForScene();
    currentA = model->getASegment();
    model->winSize = winSize;
    score = model->getAbsoluteIndex();
    scoreLabel->setColor(Color3B(model->currentArt.agR, model->currentArt.agG, model->currentArt.agB));
    
    if (model->justWarped) {
        oneTime(0);
    }else{
        this->scheduleOnce(schedule_selector(SceneSinglePlayer::oneTime), 1.0);
    }
    tearfix->setColor(Color3B(model->currentArt.bgR, model->currentArt.bgG, model->currentArt.bgB));
    tearfix2->setColor(Color3B(model->currentArt.bgR, model->currentArt.bgG, model->currentArt.bgB));
    
    tearfix->setVisible(false);
    tearfix2->setVisible(false);
    updatePlayer();
    SoundManager::getInstance()->preloadSoundEffectName(Constants::SoundEffects::collisionSFX);

    birdsBlastedSpriteLabelInGame = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel);
    this->addChild(birdsBlastedSpriteLabelInGame, SCORE_LABEL_Z_ORDER);

    addBirdToEmptyPixel(birdsBlastedSpriteLabelInGame);
    
    birdsBlastedSpriteLabelInGame->setPosition(pausePosition.x, checkPos.y - checkpointSpriteLabelInGame->getContentSize().height);
    birdsBlastedLabel = Label::createWithTTF("x 0", "fonts/Marker Felt.ttf", 16);
    this->addChild(birdsBlastedLabel, SCORE_LABEL_Z_ORDER);
    updateBirdsBlasted();

    return true;
}

void SceneSinglePlayer::oneTime(float delta){
    tearfix->setVisible(true);
    tearfix2->setVisible(true);
    this->scheduleUpdate();
}

void SceneSinglePlayer::updateBackgroundPosition(float distanceToMove){
    
    bool shift = false;
    int shiftIndex = -1;
    for (int j = 0; j<2; j++){
        for (int i = 0; i < artHolders[j].backgrounds.size(); i++){
            
            Vec2 position = artHolders[j].backgrounds[i]->getPosition();
            position = Vec2(position.x + distanceToMove, position.y);
            
            if (position.x < 0 - limitAtWhichToRemove) {
                shift = true;
                shiftIndex = i;
            }
            
            artHolders[j].backgrounds[i]->setPosition(position);
        }
        
        if (shift) {
            Sprite* bgUnit = artHolders[j].backgrounds[shiftIndex];
            int otherIndex = -1;
            if (shiftIndex == 0) {
                otherIndex = 1;
            }else{
                otherIndex = 0;
            }
            Vec2 otherPos = artHolders[j].backgrounds[otherIndex]->getPosition();
            bgUnit->setPosition(otherPos.x + backgroundWidth, otherPos.y);
            spawnClouds();
        }
    }
    
    tearfix->setPosition(artHolders[currentArtHolder].backgrounds[0]->getPositionX() + artHolders[currentArtHolder].backgrounds[0]->getContentSize().width/2, artHolders[currentArtHolder].backgrounds[0]->getPositionY());
    tearfix2->setPosition(artHolders[currentArtHolder].backgrounds[0]->getPositionX() - artHolders[currentArtHolder].backgrounds[0]->getContentSize().width/2, artHolders[currentArtHolder].backgrounds[0]->getPositionY());
    
}

void SceneSinglePlayer::spawnClouds(){
    
    int nc = model->currentArt.noC;
    Sprite* emp = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
    emp->setCascadeOpacityEnabled(true);
    emp->setCascadeColorEnabled(true);
    this->addChild(emp,CLOUD_SPRITE_Z_ORDER);
    
    float coffSet = 0;
    
    float jik = 0.0;
    for (int l=0; l<nc; l++) {
        Sprite* cld = Sprite::createWithSpriteFrameName(randomCloudFrameName() );
        emp->addChild(cld);
        
        if (l==0) {
            emp->setPosition(Vec2(winSize.width+cld->getContentSize().width/2, winSize.height  - 40));
            jik = cld->getContentSize().width;
        }
        
        float sc = 0.5f + (arc4random()%50)/100.0f;
        int flipX = arc4random()%1;
        if (flipX) {
            cld->setFlippedX(true);
        }
        
        cld->setColor(Color3B(model->currentArt.clR, model->currentArt.clG, model->currentArt.clB));
        cld->setScale(sc);
        int opc = arc4random()%model->currentArt.clO;
        cld->setOpacity(opc);
        
        
        float np = coffSet + cld->getContentSize().width;
        cld->setPosition(np, 0);
        coffSet += cld->getContentSize().width;
    }
    
    float gv = 0 - this->model->groundVelocity.x * PTM_RATIO;
    float time = (winSize.width + coffSet)/gv;
    auto move = MoveTo::create(time, Vec2(0 - coffSet - jik, winSize.height - 40));
    auto fadeOut = FadeOut::create(model->baseDuration - model->totalTime);
    auto remove = CallFunc::create([emp](){
        emp->removeFromParent();
    });
    auto seq = Sequence::create(move,remove, NULL);
    emp->runAction(fadeOut);
    emp->runAction(seq);
}

void SceneSinglePlayer::createGroundSpritesForScene(){
    
    backgroundColor = Color3B(113, 197.0, 207);
    
    groundPieces(true);
    groundPieces(false);
    
    for (int k=0; k<2; k++) {
        for (int i=0; i<2; i++) {
            Sprite* t = Sprite::createWithSpriteFrameName(Constants::SpriteNames::background);
            t->setPosition(Vec2(t->getContentSize().width/2 + i*t->getContentSize().width, t->getContentSize().height/2));
            t->setCascadeOpacityEnabled(true);
            artHolders[k].backgrounds.push_back(t);
            backgroundWidth = t->getContentSize().width;
            this->addChild(t,BACKGROUND_SPRITE_Z_ORDER);
        }
    }
    
    tearfix = Sprite::createWithSpriteFrameName(Constants::SpriteNames::tearfix);
    tearfix2 = Sprite::createWithSpriteFrameName(Constants::SpriteNames::tearfix);
    
    this->addChild(tearfix, TEAR_FIX_SPRITE_Z_ORDER);
    this->addChild(tearfix2, TEAR_FIX_SPRITE_Z_ORDER);
    
    for (int i = 0; i<2; i++) {
        artHolders[0].backgrounds[i]->setSpriteFrame(model->currentArt.backgroundFrame );
        artHolders[0].backgrounds[i]->setFlippedY(model->currentArt.flpY);
        artHolders[0].backgrounds[i]->setColor(Color3B(model->currentArt.bgR, model->currentArt.bgG, model->currentArt.bgB));
        
        artHolders[1].backgrounds[i]->setOpacity(0);
        artHolders[1].backgrounds[i]->setVisible(false);
        this->reorderChild(artHolders[1].backgrounds[i], HIDDEN_BACKGROUND_SPRITE_Z_ORDER);
    }
}

void SceneSinglePlayer::groundPieces(bool isForArtifacts){
    Sprite *tg = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundlower);
    Sprite *ug = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundupper );
    Sprite *top = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundTop );
    Size lowerSize = tg->getContentSize();
    Size upperSize = ug->getContentSize();
    Size topSize = top->getContentSize();
    
    float wid = 360;
    float swid = tg->getContentSize().width;
    int number = (wid / swid) + 1;
    
    limitAtWhichToRemove = 360/2;
    float xOffset = 0.0;
    for (int j=0; j<number; j++) {
        Sprite *groundSprite = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundlower);
        groundSprite->setColor(Color3B(model->currentArt.lgR, model->currentArt.lgG, model->currentArt.lgB));
        Sprite *upper = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundupper );
        upper->setColor(Color3B(model->currentArt.mgR, model->currentArt.mgG, model->currentArt.mgB));;
        Sprite *top = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundTop );
        top->setColor(Color3B(model->currentArt.tgR, model->currentArt.tgG, model->currentArt.tgB));
        
        Vec2 lowerPos = Vec2(groundSprite->getContentSize().width/2+xOffset,groundSprite->getContentSize().height/2);
        
        
        groundSprite->setPosition(lowerPos);
        upper->setPosition(lowerPos.x, lowerSize.height + upper->getContentSize().height/2);
        top->setPosition(lowerPos.x, lowerSize.height + upperSize.height + top->getContentSize().height/2);
        
        if (!isForArtifacts) {
            Sprite *above = Sprite::createWithSpriteFrameName(Constants::SpriteNames::aboveground );
            above->setColor(Color3B(model->currentArt.agR, model->currentArt.agG, model->currentArt.agB));
            above->setPosition(lowerPos.x, lowerSize.height + upperSize.height + topSize.height + above->getContentSize().height/2);
            
            this->addChild(above,ABOVE_GROUND_SPRITE_Z_ORDER);
            aboveGroundPieces.push_back(above);
        }
        
        
        xOffset += lowerSize.width;
        
        int nofGFl = 1 + arc4random()%3; // Number fo ground flowers
        Size size = top->getContentSize();
        for (int i = 0; i<nofGFl; i++) {
            Sprite* grf = Sprite::createWithSpriteFrameName(Constants::SpriteNames::flowerjet);
            grf->setColor(Color3B(model->currentArt.gfR, model->currentArt.gfG, model->currentArt.gfB));
            float randX = arc4random()%((int)size.width);
            float randY = arc4random()%((int)size.height);
            grf->setPosition(randX, randY);
            top->addChild(grf);
            grf->setScale(0.5);
        }
        
        if (isForArtifacts) {
            this->addChild(top,AGROUND_SPRITE_Z_ORDER);
            this->addChild(upper,AGROUND_SPRITE_Z_ORDER);
            this->addChild(groundSprite,AGROUND_SPRITE_Z_ORDER);
            
            alowerGroundPieces.push_back(groundSprite);
            amidGroundPieces.push_back(upper);
            atopGroundPieces.push_back(top);
            
        }else{
            this->addChild(top,GROUND_SPRITE_Z_ORDER);
            this->addChild(upper,GROUND_SPRITE_Z_ORDER);
            this->addChild(groundSprite,GROUND_SPRITE_Z_ORDER);
            
            lowerGroundPieces.push_back(groundSprite);
            midGroundPieces.push_back(upper);
            topGroundPieces.push_back(top);
        }
    }
}

std::string SceneSinglePlayer::randomCloudFrameName(){
    int choice = arc4random()%9;
    std::string cld;
    switch (choice) {
        case 0:{
            cld = Constants::Clouds::cloud1;
            break;
        }
        case 1:{
            cld = Constants::Clouds::cloud2;
            break;
        }
        case 2:{
            cld = Constants::Clouds::cloud3;
            break;
        }
        case 3:{
            cld = Constants::Clouds::cloud4;
            break;
        }
        case 4:{
            cld = Constants::Clouds::cloud5;
            break;
        }
        case 6:{
            cld = Constants::Clouds::cloud7;
            break;
        }
        case 7:{
            cld = Constants::Clouds::cloud8;
            break;
        }
        case 8:{
            cld = Constants::Clouds::cloud9;
            break;
        }
        default:
            cld = Constants::Clouds::cloud1;
            break;
    }
    
    return cld;
}

void SceneSinglePlayer::updateGroundSpritesForScene(float distanceToMove){
    //Just like obstruction, we move the ground units. The ground units don't collide with anything, and like obstructions keep moving at a particular velocity.
    
    Sprite *tg = Sprite::createWithSpriteFrameName(Constants::SpriteNames::groundlower);
    float wid = 360;
    float swid = tg->getContentSize().width;
    int number = (wid / swid) + 1;
    float tw = number * swid;
    float swic = swid/2;
    for (int i = 0; i<lowerGroundPieces.size(); i++) {
        Sprite* ls = lowerGroundPieces[i];
        Sprite* ms = midGroundPieces[i];
        Sprite* ts = topGroundPieces[i];
        Sprite* ab = aboveGroundPieces[i];
        
        
        Vec2 lpos = ls->getPosition();
        Vec2 mpos = ms->getPosition();
        Vec2 tpos = ts->getPosition();
        Vec2 apos = ab->getPosition();
        
        float newX = lpos.x + distanceToMove;
        
        if (newX < 0 - swic) {
            newX = newX + tw;
            
            ls->setSpriteFrame(model->currentArt.lowerGroundFrame );
            ms->setSpriteFrame(model->currentArt.midGroundFrame );
            ts->setSpriteFrame(model->currentArt.topGroundFrame );
            ab->setSpriteFrame(model->currentArt.aboveGroundFrame );
        }
        
        ls->setPosition(Vec2(newX, lpos.y));
        ms->setPosition(Vec2(newX, mpos.y));
        ts->setPosition(Vec2(newX, tpos.y));
        ab->setPosition(Vec2(newX, apos.y));
    }
}

void SceneSinglePlayer::tintWithTime(Sprite* sprite,float time,Color3B color){
    TintTo* tint = TintTo::create(time, color);
    tint->setTag(TINT_ACTION_TAG);
    sprite->stopActionByTag(TINT_ACTION_TAG);
    sprite->runAction(tint);
}

template <typename T>
std::string SceneSinglePlayer::numberTostring(T value)
{
    //create an output string stream
    std::ostringstream os ;
    
    //throw the value into the string stream
    os << value ;
    
    //convert the string stream into a string and return
    return os.str() ;
}

void SceneSinglePlayer::showInstructions(){
    scoreLabel->setVisible(false);
    
    instructionSprite = Sprite::createWithSpriteFrameName(Constants::SpriteNames::tapInstruction1);
    Animation *instructionAnimation;
    Vector<SpriteFrame*> instructionAnimFrames;
    SpriteFrameCache* cache = SpriteFrameCache::getInstance();
    
    SpriteFrame* frame = cache->getSpriteFrameByName( Constants::SpriteNames::tapInstruction1 );
    instructionAnimFrames.pushBack(frame);
    frame = cache->getSpriteFrameByName( Constants::SpriteNames::tapInstruction2 );
    instructionAnimFrames.pushBack(frame);
    
    instructionAnimation = Animation::createWithSpriteFrames(instructionAnimFrames,0.8);
    instructionSprite->runAction(RepeatForever::create(Animate::create(instructionAnimation)));
    this->addChild(instructionSprite, INSTRUCTION_SPRITE_Z_ORDER);
    
    instructionSprite->setPosition(visibleSize.width/2 - instructionSprite->getContentSize().width/3, visibleSize.height * 4.0/8.0);
}

void SceneSinglePlayer::hideInstruction(){
    scoreLabel->setVisible(true);
    instructionSprite->setVisible(false);
}

void SceneSinglePlayer::createPauseButton(){
    std::string playImageFileName = Constants::SpriteNames::pause;
    std::string playSelectedImageFileName = Constants::SpriteNames::pause;
    auto normalSprite = Sprite::createWithSpriteFrameName(playImageFileName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(playSelectedImageFileName);
    pauseSpriteWidth = normalSprite->getContentSize().width;
    pauseSpriteHeight = normalSprite->getContentSize().height;
    
    auto pause = MenuItemSprite::create(normalSprite, selectedSprite, CC_CALLBACK_1(SceneSinglePlayer::receivePauseGame, this));
    pause->setPosition(selectedSprite->getContentSize().width, visibleSize.height - selectedSprite->getContentSize().height);
    pausePosition = pause->getPosition();
    
    Vector<MenuItem*> menuItems;
    menuItems.pushBack(pause);
    
    pauseButton = Menu::createWithArray(menuItems);
    pauseButton->setPosition(Vec2::ZERO);
    this->addChild(pauseButton, MENU_Z_ORDER);
}

void SceneSinglePlayer::createPauseMenu(){
    std::string pauseImageFileName = Constants::SpriteNames::bigpause;
    std::string pauseSelectedImageFileName = Constants::SpriteNames::bigpause;
    auto normalSprite = Sprite::createWithSpriteFrameName(pauseImageFileName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(pauseSelectedImageFileName);
    
    auto pause = MenuItemSprite::create(normalSprite, selectedSprite, CC_CALLBACK_1(SceneSinglePlayer::receiveResumeGame, this));
    pause->setPosition(visibleSize.width/2 , visibleSize.height/2);
    
    Vector<MenuItem*> menuItems;
    menuItems.pushBack(pause);
    
    pauseMenu = Menu::createWithArray(menuItems);
    pauseMenu->setPosition(Vec2::ZERO);
    this->addChild(pauseMenu, MENU_Z_ORDER);
}

void SceneSinglePlayer::addPauseButton(){
    pauseButton->setVisible(true);
}

void SceneSinglePlayer::removePauseButton(){
    pauseButton->setVisible(false);
}

void SceneSinglePlayer::receivePauseGame(Ref* pSender){
    pauseGame();
}

void SceneSinglePlayer::pauseGame(){
    
    if (!isPaused && !isGameStopped) {
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

void SceneSinglePlayer::resumeGame(){
    
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

void SceneSinglePlayer::receiveResumeGame(Ref* pSender){
    
    resumeGame();
    //    CCLOG("resume button pressed");
}

void SceneSinglePlayer::update(float dt)
{
    if(!isGameStopped){
        float groundVelocity = this->model->groundVelocity.x;
        float distanceToMove = PTM_RATIO*groundVelocity*dt;
        
        this->updateGroundSpritesForScene(distanceToMove);
        updateBackgroundPosition(distanceToMove);
        
        if (totalTime > 1) {
            if (player->sprite->getPositionX() < 0) {
                addStatsAndMenu(false);
            }
        }
        
    }else{
        player->sprite->stopAllActions();
        model->player->body->SetFixedRotation(false);
    }
    
    updatePlayer();
    
    //When a collision with an obstacle occurs, the only thing that should be updated is the player. The obstructions, eatables and ground should stop moving.
    
    if (isGameStarted) {
        totalTime+=dt;
        updateObstructions();
        this->model->tick(dt,receivedTouches,false);
        checkForContacts();
        updateFlowerJets();
        updateBadFoods();
        updateBosses();
        updateOneUps();
        updateSkipLevels();
        updateParticles();
        if(score!=model->getAbsoluteIndex()){
            score = model->getAbsoluteIndex();
            updateScoreLabel();
        }
        
        int modelA = model->getASegment();
        if (currentA != modelA) {
            currentA = modelA;
            updateLevelSpriteFrames();
        }
        
    }
}

void SceneSinglePlayer::fadeOutAndMakeInvisible(Sprite* sprite,float transitionTime,bool reorder,SPRITE_Z_ORDER newZOrder){
    auto fadeOut = FadeOut::create(transitionTime);
    auto makeInvisible = CallFunc::create([sprite,reorder,newZOrder](){
        sprite->setVisible(false);
        if (reorder) {
            sprite->getParent()->reorderChild(sprite, newZOrder);
        }
    });
    auto seq = Sequence::createWithTwoActions(fadeOut, makeInvisible);
    
    sprite->runAction(seq);
}

void SceneSinglePlayer::fadeOutAndRemove(Sprite* sprite,float transitionTime){
    auto fadeOut = FadeOut::create(transitionTime);
    auto remove = CallFunc::create([sprite](){
        sprite->removeFromParent();
    });
    auto seq = Sequence::createWithTwoActions(fadeOut, remove);
    
    sprite->runAction(seq);
}

void SceneSinglePlayer::makeVisibleAndFadeIn(Sprite *sprite, float transitionTime){
    
    sprite->setOpacity(0);
    sprite->setVisible(true);
    auto fadeIn = FadeIn::create(transitionTime);
    sprite->runAction(fadeIn);
}

void SceneSinglePlayer::updateSunMoon(Sprite* body,unsigned char r,unsigned char g,unsigned char b,std::string frameName,float transitionTime){
    
    Sprite* ns = Sprite::createWithSpriteFrameName(frameName);
    ns->setOpacity(0);
    
    float randomX = ((arc4random()%100)/100.0f)*winSize.width;
    
    float randomY = ((arc4random()%100)/100.0f)*ns->getContentSize().width*3; // Distance from top of screen
    
    Vec2 nl = Vec2(randomX, winSize.height - randomY);
    float newSCale = 1 + (randomY/winSize.height);
    auto scaleTo = ScaleTo::create(transitionTime, newSCale);
    
    float ranRot = arc4random()%360;
    
    //Move it to the location of the new sun slowly
    //Rotate it to some new value
    auto move = MoveTo::create(transitionTime, nl);
    auto rotate = RotateBy::create(transitionTime, ranRot);
    body->stopAllActions();
    body->runAction(move);
    body->runAction(rotate);
    
    //Remove the old sun.
    Vector<Node*> children = body->getChildren();
    for(Node* child : children)
    {
        Sprite* sp = dynamic_cast<Sprite*>(child);
        if (sp!=NULL) {
            fadeOutAndRemove(sp, transitionTime);
            sp->getParent()->reorderChild(sp, 5);
        }
    }
    
    //Add the new sprite and fadein.
    auto fadeIn = FadeIn::create(transitionTime);
    ns->runAction(fadeIn);
    ns->runAction(scaleTo);
    ns->setColor(Color3B(r, g, b));
    body->addChild(ns, 4);
    body->setColor(Color3B(r, g, b));
}

void SceneSinglePlayer::makeSunMoonSet(Sprite *body,float transitionTime){
    //Move offscreen above top right.
    Sprite *referenceSize = Sprite::createWithSpriteFrameName(Constants::Space::sun);
    Vector<Node*> children = body->getChildren();
    for(Node* child : children)
    {
        Sprite* sp = dynamic_cast<Sprite*>(child);
        if (sp!=NULL) {
            Vec2 nl = Vec2(0 - referenceSize->getContentSize().width, winSize.height + referenceSize->getContentSize().height);
            float randRot = arc4random()%359;
            auto move = MoveTo::create(transitionTime, nl);
            auto scaleBack = ScaleTo::create(transitionTime,1.0);
            auto rotate = RotateTo::create(transitionTime, randRot);
            body->runAction(rotate);
            body->runAction(move);
            body->runAction(scaleBack);
            fadeOutAndRemoveChildren(body,transitionTime);
        }
    }
}

void SceneSinglePlayer::makeStarsSet(float transitionTime){
    int starIndex = arc4random()%17 + 3;
    char str[100] = {0};
    sprintf(str, (Constants::Space::star).c_str(), starIndex);
    Sprite* referenceSize = Sprite::createWithSpriteFrameName(str);
    
    for(Sprite* star : stars)
    {
        star->stopAllActions();
        Vec2 nl = Vec2(0 - referenceSize->getContentSize().width, winSize.height + referenceSize->getContentSize().height);
        float randRot = arc4random()%359;
        auto move = MoveTo::create(transitionTime, nl);
        auto scaleBack = ScaleTo::create(transitionTime,1.0);
        auto rotate = RotateTo::create(transitionTime, randRot);
        star->runAction(rotate);
        star->runAction(move);
        star->runAction(scaleBack);
        fadeOutAndRemove(star,transitionTime);
    }
    stars.clear();
}


void SceneSinglePlayer::fadeOutAndRemoveChildren(Sprite* sprite,float transitionTime){
    Vector<Node*> children = sprite->getChildren();
    for(Node* child : children)
    {
        Sprite* sp = dynamic_cast<Sprite*>(child);
        if (sp!=NULL) {
            fadeOutAndRemove(sp, transitionTime);
        }
        sp->getParent()->reorderChild(sp, 5);
    }
}

void SceneSinglePlayer::updateLevelSpriteFrames(){
    SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::level);

    std::string tim = model->currentDateTime();
    
    float transitionTime = 2.0f;
    Sprite *referenceSize = Sprite::createWithSpriteFrameName(Constants::Space::sun);
    scoreLabel->setColor(Color3B(model->currentArt.agR, model->currentArt.agG, model->currentArt.agB));
    birdsBlastedLabel->setColor(Color3B(model->currentArt.agR, model->currentArt.agG, model->currentArt.agB));
    checkpointsCollectedLabel->setColor(Color3B(model->currentArt.agR, model->currentArt.agG, model->currentArt.agB));
    birdsBlastedSpriteLabelInGame->removeAllChildrenWithCleanup(false);
    addBirdToEmptyPixel(birdsBlastedSpriteLabelInGame);
    
    if (model->currentArt.hSun) {
        sun->setPosition(Vec2(winSize.width+referenceSize->getContentSize().width, winSize.height+referenceSize->getContentSize().height));
        updateSunMoon(sun, model->currentArt.suR, model->currentArt.suG, model->currentArt.suB, Constants::Space::sun, transitionTime);
    }else{
        //Fade out and make invisible
        makeSunMoonSet(sun, transitionTime);
    }
    
    if(model->currentArt.hMoon){
        moon->setPosition(Vec2(winSize.width+referenceSize->getContentSize().width, winSize.height+referenceSize->getContentSize().height));
        
        updateSunMoon(moon, model->currentArt.moR, model->currentArt.moG, model->currentArt.moB, Constants::Space::moon, transitionTime);
    }else{
        makeSunMoonSet(moon, transitionTime);
    }
    
    if (model->currentArt.hStars) {
        //Remove the old stars by fading them out slowly
        makeStarsSet(transitionTime);
        
        
        Vec2 startPosition = Vec2(winSize.width + referenceSize->getContentSize().width, winSize.height + referenceSize->getContentSize().height);
        
        int noS = 2 + arc4random()%5;
        float xOffset = winSize.width/noS;
        float newX = (arc4random()%((int)winSize.width/3));
        
        for (int i=0; i<noS; i++) {
            
            int starIndex = arc4random()%17 + 3;
            char str[100] = {0};
            sprintf(str, (Constants::Space::star).c_str(), starIndex);
            
            Sprite* star = Sprite::createWithSpriteFrameName(str);
            Size starSize = star->getContentSize();
            star->setColor(Color3B(model->currentArt.stR, model->currentArt.stG, model->currentArt.stB));
            float newY = arc4random()%(80) + starSize.height;
            
            Vec2 newPosition = Vec2(newX + i*xOffset,winSize.height - newY);
            star->setPosition(startPosition);
            
            float flickerTime = 0.05;
            unsigned char flickerOpacity = 220;
            
            float newSCale = (arc4random()%200)/100.0f + 1;
            auto scaleTo = ScaleTo::create(transitionTime, newSCale);
            auto move = MoveTo::create(transitionTime, newPosition);
            auto flickerForever = RepeatForever::create(Sequence::createWithTwoActions(FadeIn::create(flickerTime), FadeTo::create(flickerTime, flickerOpacity)));
            
            this->addChild(star, STAR_SPRITE_Z_ORDER);
            stars.push_back(star);
            star->runAction(scaleTo);
            star->runAction(move);
            star->runAction(flickerForever);
        }
        
        //Create the new star but set opacity to 0
        //Set their positions random
        //Slowly fade them in.
    }else{
        //Fade out and remove;
        makeStarsSet(transitionTime);
    }
    
    
    
    //Fade out the current art holder to 0, and sets alls its stuff as visible = false
    for (int i = 0; i< artHolders[currentArtHolder].backgrounds.size(); i++) {
        
        fadeOutAndMakeInvisible(artHolders[currentArtHolder].backgrounds[i], transitionTime, true, HIDDEN_BACKGROUND_SPRITE_Z_ORDER);
    }
    
    // Set the new art holder to current
    if (!currentArtHolder) {
        currentArtHolder = 1;
    }else{
        currentArtHolder = 0;
    }
    
    // Fade it in, in the same time as we fade out the old.
    
    for (int i = 0; i< artHolders[currentArtHolder].backgrounds.size(); i++) {
        artHolders[currentArtHolder].backgrounds[i]->removeAllChildren();
        artHolders[currentArtHolder].backgrounds[i]->setVisible(true);
        artHolders[currentArtHolder].backgrounds[i]->setOpacity(255);
        
        //Set the art to the new sprite frame
        artHolders[currentArtHolder].backgrounds[i]->setSpriteFrame(model->currentArt.backgroundFrame );
        
        //Flip Y if necessary
        artHolders[currentArtHolder].backgrounds[i]->setFlippedY(model->currentArt.flpY);
        
        artHolders[currentArtHolder].backgrounds[i]->setColor(Color3B(model->currentArt.bgR, model->currentArt.bgG, model->currentArt.bgB));
        
        // Fade it in, in the same time as we fade out the old.
        auto delay = DelayTime::create(transitionTime);
        Sprite *foo = artHolders[currentArtHolder].backgrounds[i];
        auto reorder = CallFunc::create([foo](){
            foo->getParent()->reorderChild(foo, BACKGROUND_SPRITE_Z_ORDER);
        });
        auto seq = Sequence::createWithTwoActions(delay, reorder);
        artHolders[currentArtHolder].backgrounds[i]->runAction(seq);
    }
    tearfix->setColor(Color3B(model->currentArt.bgR, model->currentArt.bgG, model->currentArt.bgB));
    tearfix2->setColor(Color3B(model->currentArt.bgR, model->currentArt.bgG, model->currentArt.bgB));
    
    //Update ground sprites:
    tintGroundSprites(false); // Not for artifacts
    tintGroundSprites(true); // For artifacts
    
}

void SceneSinglePlayer::tintGroundSprites(bool isForArtifacts){
    for (int i = 0; i<lowerGroundPieces.size(); i++) {
        
        Sprite* ls;
        Sprite* ms;
        Sprite* ts;
        Sprite* ab;
        float timeLeft = 2;
        
        if (isForArtifacts) {
            ls = alowerGroundPieces[i];
            ms = amidGroundPieces[i];
            ts = atopGroundPieces[i];
        }else{
            ls = lowerGroundPieces[i];
            ms = midGroundPieces[i];
            ts = topGroundPieces[i];
            ab = aboveGroundPieces[i];
            
            tintWithTime(ab, timeLeft, Color3B(model->currentArt.agR, model->currentArt.agG, model->currentArt.agB));
        }
        
        
        tintWithTime(ls, timeLeft, Color3B(model->currentArt.lgR, model->currentArt.lgG, model->currentArt.lgB));
        tintWithTime(ms, timeLeft, Color3B(model->currentArt.mgR, model->currentArt.mgG, model->currentArt.mgB));
        tintWithTime(ts, timeLeft, Color3B(model->currentArt.tgR, model->currentArt.tgG, model->currentArt.tgB));
        
        for (Node* child : ts->getChildren())
        {
            if (dynamic_cast<Sprite*>(child)!=NULL) {
                Sprite* sprite = (Sprite*) child;
                tintWithTime(sprite, timeLeft, Color3B(model->currentArt.gfR, model->currentArt.gfG, model->currentArt.gfB));
            }
        }
        
    }
}

void SceneSinglePlayer::checkForContacts(){
    
    bool isFini = model->isLastLevelReached();
    
    if (isFini || ((model->contactListener.contacts.size() > 0)&&(!isTesting))) {
        if (!isGameStopped) {
            addStatsAndMenu(isFini);
            
            auto action = Sequence::create(CallFunc::create([&]() {
                this->setColor(Color3B(255, 0, 0));
            }),DelayTime::create(0.05),CallFunc::create([&]() {
                this->setColor(Color3B(255, 255, 255));
            }),DelayTime::create(0.05),CallFunc::create([&]() {
                this->setColor(backgroundColor);
            }),NULL);
            
            for (Sprite* bg : artHolders[currentArtHolder].backgrounds){
                bg->runAction(Repeat::create(action, 2));
            }
        }
    }
    
    model->contactListener.contacts.clear();
}

//ON GAME OVER GAMEOVER
void SceneSinglePlayer::addStatsAndMenu(bool isReachedTheEnd){
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniBridge::showAds();
#endif
    
    scoreLabel->setVisible(false);
    removePauseButton();
    isGameStopped = true;
    this->model->stopGame();
    
    int currentDistance = model->getAbsoluteIndex();
    if (currentDistance > bestDistance) {
        bestDistance = currentDistance;
        UserDefault::getInstance()->setIntegerForKey(Constants::UserDefaults::bestDistance.c_str(), bestDistance);
        
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
        gch->reportScore(bestDistance, Constants::LeaderboardIds::distanceLeaderboardId);
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        JniBridge::submitScore(bestDistance);
#endif
    }
    
    float timeForMenuToAppear = 0.5f;
    Sprite* top;
    Sprite* grayback = Sprite::createWithSpriteFrameName(Constants::SpriteNames::background);
    this->addChild(grayback, MENU_BACK_Z_ORDER);
    grayback->setColor(Color3B(128, 128, 128));
    grayback->setOpacity(0);
    grayback->setScale((grayback->getContentSize().width)/(winSize.width*1.6), (grayback->getContentSize().height)/(winSize.height*2.0));
    grayback->setPosition(winSize.width/2, winSize.height/2);
    FadeTo* grayAppear = FadeTo::create(timeForMenuToAppear, 128);
    grayback->runAction(grayAppear);
    
    
    
    top = Sprite::createWithSpriteFrameName(Constants::SpriteNames::trophy );
    top->setVisible(false);
    this->addChild(top, MENU_Z_ORDER);
    
    if (!isReachedTheEnd) {
        //Player crashed into something, so we play crash sound.
        SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::collisionSFX);
    }else{
        
        //TODO Use a different transition
        Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneEndGame::createScene()));
        return;
    }
    
    top->setPosition(visibleSize.width/2, grayback->getPositionY() + grayback->getContentSize().height/2*grayback->getScaleY()*0.7);
    
    
    int sc = 0;
    std::string s = numberTostring(sc);
    
    Sprite* cdisth = createWithHolder(Constants::SpriteNames::distanceLabel, top, ": " + numberTostring(model->getAbsoluteIndex()) + " m");
    Sprite* cbestdisth = createWithHolder(Constants::SpriteNames::trophy, cdisth,": " + numberTostring(bestDistance) + "/2928 m");
    cdisth->setOpacity(0);
    cbestdisth->setOpacity(0);
    FadeTo* c1appear = FadeIn::create(timeForMenuToAppear);
    cdisth->runAction(c1appear);
    FadeTo* c2appear = FadeIn::create(timeForMenuToAppear);
    cbestdisth->runAction(c2appear);
    
    MenuItemSprite* continueGame = createMenuItemSprite(Constants::MenuImageNames::continueImageName, Constants::MenuImageNames::continueSelectedImageName, CC_CALLBACK_1(SceneSinglePlayer::continueCallBack, this), visibleSize.width/4, cbestdisth->getPosition().y - cbestdisth->getContentSize().height);
    
    MenuItemSprite* play = createMenuItemSprite(Constants::MenuImageNames::play, Constants::MenuImageNames::playselected, CC_CALLBACK_1(SceneSinglePlayer::playCallback, this), visibleSize.width/4, cbestdisth->getPosition().y*0.9);
    
    MenuItemSprite* rate = createMenuItemSprite(Constants::MenuImageNames::rate, Constants::MenuImageNames::rateselected, CC_CALLBACK_1(SceneSinglePlayer::rateCallback, this), visibleSize.width/2, cbestdisth->getPosition().y*0.9);
    
    MenuItemSprite* ranking = createMenuItemSprite(Constants::MenuImageNames::rankings, Constants::MenuImageNames::rankingsselected, CC_CALLBACK_1(SceneSinglePlayer::rankingCallback, this), visibleSize.width*3/4, cbestdisth->getPosition().y*0.9);
    
    Sprite *backSprite = Sprite::createWithSpriteFrameName(Constants::MenuImageNames::back);
    MenuItemSprite *back = createMenuItemSprite(Constants::MenuImageNames::back, Constants::MenuImageNames::backselected, CC_CALLBACK_1(SceneSinglePlayer::backCallback, this), backSprite->getContentSize().width*1.1, visibleSize.height);
    
    Vector<MenuItem*> menuItems;
    
    if (!GameStatus::savePoints.empty() && !isReachedTheEnd) {
        menuItems.pushBack(continueGame);
        continueGame->setPosition(play->getPosition());
    }else{
        menuItems.pushBack(play);
    }
    
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
    auto musiconoff = MenuItemSprite::create(audioButton, audioButtonSelected, CC_CALLBACK_1(SceneSinglePlayer::toggleSFXCallback, this));
    musiconoff->setPosition(visibleSize.width/2, 80 + colorFrame->getContentSize().height/2);
    
    menuItems.pushBack(rate);
    menuItems.pushBack(ranking);
    menuItems.pushBack(back);
    menuItems.pushBack(musiconoff);

    if (ModelArt::getInstance()->isBW) {
        auto colors = createMenuItemSprite(Constants::MenuImageNames::colors, Constants::MenuImageNames::colorselected, CC_CALLBACK_1(SceneSinglePlayer::colorsCallBack, this), colorFrame->getContentSize().width*1.1, 80 + colorFrame->getContentSize().height*1.1);
        
        menuItems.pushBack(colors);
    }else{
        auto colors = createMenuItemSprite(Constants::MenuImageNames::bw, Constants::MenuImageNames::bwselected, CC_CALLBACK_1(SceneSinglePlayer::colorsCallBack, this), colorFrame->getContentSize().width*1.1, 80 + colorFrame->getContentSize().height*1.1);
        
        menuItems.pushBack(colors);
    }
    
    auto menu =Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, MENU_Z_ORDER);
    
    menu->setCascadeOpacityEnabled(true);
    menu->setOpacity(0);
    FadeIn* menuAppear = FadeIn::create(timeForMenuToAppear);
    menu->runAction(menuAppear);
}

Sprite* SceneSinglePlayer::addFallenPlayer(bool isFinished){
    auto singlePixel = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
    singlePixel->setCascadeOpacityEnabled(true);
    auto fallenPlayer = Sprite::createWithSpriteFrameName(Constants::SpriteNames::playerInitialFrame );
    fallenPlayer->setRotation(270);
    auto flowerstar = Sprite::createWithSpriteFrameName(Constants::SpriteNames::flowerjet );
    flowerstar->setPosition(Vec2(0 - fallenPlayer->getContentSize().height, fallenPlayer->getContentSize().width));
    flowerstar->setColor(Color3B(model->currentArt.tlR, model->currentArt.tlR, model->currentArt.tlR));
    auto moveTo = MoveTo::create(0.5, Vec2(fallenPlayer->getContentSize().height, fallenPlayer->getContentSize().width));
    auto moveBack = MoveTo::create(0.5, Vec2(0 - fallenPlayer->getContentSize().height, fallenPlayer->getContentSize().width));
    auto moveSeq = Sequence::createWithTwoActions(moveTo, moveBack);
    flowerstar->runAction(RepeatForever::create(moveSeq));
    singlePixel->addChild(fallenPlayer);
    singlePixel->addChild(flowerstar);
    singlePixel->setPosition(visibleSize.width/2, visibleSize.height * 8.0/9.0);
    singlePixel->setOpacity(0);
    FadeIn* fi = FadeIn::create(0.5);
    singlePixel->runAction(fi);
    this->addChild(singlePixel,SCORE_LABEL_Z_ORDER);
    
    return singlePixel;
}

MenuItemSprite* SceneSinglePlayer::createMenuItemSprite(std::string imageName, std::string selectedImageName,const ccMenuCallback& callback,float xPosition,float belowY){
    std::string imageFileName = imageName;
    std::string selectedImageFileName = selectedImageName;
    
    auto normalSprite = Sprite::createWithSpriteFrameName(imageFileName);
    auto selectedSprite = Sprite::createWithSpriteFrameName(selectedImageFileName);
    auto menuItemSprite = MenuItemSprite::create(normalSprite, selectedSprite, callback);
    
    menuItemSprite->setPosition(xPosition, belowY - normalSprite->getContentSize().height * 1.2);
    menuItemSprite->setOpacity(0);
    FadeIn* menuAppear = FadeIn::create(0.5);
    menuItemSprite->runAction(menuAppear);
    return menuItemSprite;
}


Sprite* SceneSinglePlayer::createWithHolder(std::string labelImageName,Sprite* toPutBelow,std::string textValue){
    Sprite* scoreHolder = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
    scoreHolder->setCascadeOpacityEnabled(true);
    float biggestHeight = 0;
    float myHeight = 0;
    
    auto currentScoreLabel = Sprite::createWithSpriteFrameName(labelImageName );
    currentScoreLabel->setPosition(winSize.width/2 - currentScoreLabel->getContentSize().width, 0);
    scoreHolder->addChild(currentScoreLabel);
    
    Label *currentScoreValue = Label::createWithTTF("", "fonts/Marker Felt.ttf", 20);
    currentScoreValue->setString(textValue);
    currentScoreValue->setPosition(winSize.width/2 + currentScoreValue->getContentSize().width/2, currentScoreLabel->getPosition().y);
    
    scoreHolder->addChild(currentScoreValue);
    
    myHeight = getBiggestHeight(scoreHolder);
    biggestHeight = getBiggestHeight(toPutBelow);
    
    scoreHolder->setPosition(0,toPutBelow->getPosition().y - (myHeight/2 + biggestHeight/2)*1.1);
    this->addChild(scoreHolder,SCORE_LABEL_Z_ORDER);
    
    return scoreHolder;
}

float SceneSinglePlayer::getBiggestHeight(Sprite* sprite){
    float biggestHeight = 0;
    
    for (Node* child : sprite->getChildren())
    {
        if (dynamic_cast<Sprite*>(child)!=NULL) {
            Sprite* sprite = (Sprite*) child;
            float spriteHeight = sprite->getContentSize().height;
            if (spriteHeight > biggestHeight) {
                biggestHeight = spriteHeight;
            }
        }
    }
    
    return biggestHeight;
}


void SceneSinglePlayer::backCallback(Ref* pSender){
    SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::menu);
    Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneMenuMain::createScene()));
}

void SceneSinglePlayer::updateScoreLabel(){
    
    std::string s = numberTostring(model->getAbsoluteIndex()) + " m";
    scoreLabel->setString(s);
    scoreLabel->setPosition(Point(origin.x + visibleSize.width/2 ,origin.y + visibleSize.height - scoreLabel->getContentSize().height));
}

void SceneSinglePlayer::updateCheckpointsCollectedLabel(){
    
    std::string s = "x " + numberTostring(GameStatus::savePoints.size());
    checkpointsCollectedLabel->setString(s);
    checkpointsCollectedLabel->setPosition(Vec2(checkPos.x + checkpointSpriteLabelInGame->getContentSize().width + checkpointsCollectedLabel->getContentSize().width/2 , checkPos.y));
}

void SceneSinglePlayer::updateBirdsBlasted(){
    std::string s = "x " + numberTostring(GameStatus::birdsBlasted);
    birdsBlastedLabel->setString(s);
    birdsBlastedLabel->setPosition(Vec2(checkPos.x + checkpointSpriteLabelInGame->getContentSize().width + birdsBlastedLabel->getContentSize().width/2 , birdsBlastedSpriteLabelInGame->getPositionY()));
    
    updateCheckpointsCollectedLabel();
}

void SceneSinglePlayer::updateOneUps(){
    //Add the new oneups
    for (auto newId : model->newOneUps){
        ModelOneUp *modelOneUp = model->oneUps[newId];
        b2Vec2 modelPosition = modelOneUp->body->GetPosition();
        
        //TODO Add some particle effect so it looks special
        Sprite *oneUp = Sprite::createWithSpriteFrameName(Constants::SpriteNames::oneUp );
        Vec2 position = Vec2(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
        oneUp->setPosition(position);
        this->addChild(oneUp, EATABLE_SPRITE_Z_ORDER);
        this->oneUps[newId] = oneUp;
    }
    
    
    //TODO Just after removing do some particle effect so that it is not jarring.
    //Remove the old eatables
    for (auto oneUpToRemoveId : model->removedOneUps){
        Sprite *oneUp = this->oneUps[oneUpToRemoveId];
        if (oneUp != NULL) {
            oneUp->removeFromParent();
            SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::checkpoint);
        }
        
        oneUps.erase(oneUpToRemoveId);
        updateCheckpointsCollectedLabel();
    }
    
    //Update all the oneupds, position, appearance etc.
    for (auto oneUpData : model->oneUps){
        int oneUpId = oneUpData.first;
        Sprite *oneUp = this->oneUps[oneUpId];
        ModelOneUp *modelOneUp = model->oneUps[oneUpId];
        b2Vec2 modelPosition = modelOneUp->body->GetPosition();
        oneUp->setPosition(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
    }
}

void SceneSinglePlayer::updateParticles(){
    //Add the new particles
    for (auto newId : model->newParticles){
        ModelParticle *modelParticle = model->particles[newId];
        b2Vec2 modelPosition = modelParticle->body->GetPosition();
        
        Sprite *particle = Sprite::createWithSpriteFrameName(Constants::SpriteNames::particle );
        particle->setColor(Color3B(model->currentArt.paR, model->currentArt.paG, model->currentArt.paB));
        
        Vec2 position = Vec2(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
        particle->setPosition(position);
        this->addChild(particle, WEATHER_Z_ORDER);
        this->particles[newId] = particle;
    }
    
    for (auto particleToRemove : model->removedParticles){
        Sprite *particle = this->particles[particleToRemove];
        if (particle != NULL) {
            particle->removeFromParent();
        }
        
        particles.erase(particleToRemove);
    }
    
    //Update all the obstructions, position, appearance etc.
    for (auto parData : model->particles){
        int partId = parData.first;
        Sprite *particle = this->particles[partId];
        ModelParticle *modelParticle = model->particles[partId];
        b2Vec2 modelPosition = modelParticle->body->GetPosition();
        particle->setPosition(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
    }
}

void SceneSinglePlayer::updateSkipLevels(){
    //Add the new oneups
    for (auto newId : model->newSkipLevels){
        ModelSkipLevel *modelSkip = model->skipLevels[newId];
        b2Vec2 modelPosition = modelSkip->body->GetPosition();
        
        Sprite *skip = Sprite::createWithSpriteFrameName(Constants::SpriteNames::musicalbean );
        Vec2 position = Vec2(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
        skip->setPosition(position);
        this->addChild(skip, EATABLE_SPRITE_Z_ORDER);
        this->skipLevels[newId] = skip;
    }
    
    //Remove the old eatables
    for (auto skipToRemoveId : model->removedSkipLevels){
        Sprite *skip = this->skipLevels[skipToRemoveId];
        if (skip != NULL) {
            skip->removeFromParent();
            SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::level);
        }
        
        if (model->timeToSkipLevels){
            Director::getInstance()->replaceScene(SceneSkipTransition::createScene());
        }
        skipLevels.erase(skipToRemoveId);
    }
    
    //Update all the obstructions, position, appearance etc.
    for (auto skipData : model->skipLevels){
        int skipId = skipData.first;
        Sprite *skip = this->skipLevels[skipId];
        ModelSkipLevel *modelSkip = model->skipLevels[skipId];
        b2Vec2 modelPosition = modelSkip->body->GetPosition();
        skip->setPosition(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
    }
}

//If you change this, make sure to update the main menu scene if necessary
void SceneSinglePlayer::updatePlayer(){
    b2Vec2 playerPositionPh = this->model->player->body->GetPosition();
    player->sprite->setPosition(playerPositionPh.x * PTM_RATIO,playerPositionPh.y*PTM_RATIO);
    player->sprite->setRotation(CC_RADIANS_TO_DEGREES(model->player->body->GetAngle()));
}

void SceneSinglePlayer::updateObstructions(){
    //Add the new obstructions
    for (auto newId : model->newObstructionIds){
        ModelObstruction *modelObstruction = model->obstructions[newId];
        b2Vec2 modelPosition = modelObstruction->body->GetPosition();
        
        Sprite *obstruction = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel);
        
        Sprite *tree = Sprite::createWithSpriteFrameName(model->currentArt.obstructionFrame + "t");
        
        obstruction->setCascadeOpacityEnabled(true);
        obstruction->addChild(tree);
        tree->setColor(Color3B(model->currentArt.trR, model->currentArt.trG, model->currentArt.trB));
        
        
        if(model->currentArt.hasLeaves){
            Sprite *leaves = Sprite::createWithSpriteFrameName(model->currentArt.obstructionFrame + "l");
            obstruction->addChild(leaves);
            leaves->setColor(Color3B(model->currentArt.tlR, model->currentArt.tlG, model->currentArt.tlB));
        }
        
        Vec2 position = Vec2(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
        obstruction->setPosition(position);
        this->addChild(obstruction, OBSTRUCTION_SPRITE_Z_ORDER
                       );
        this->obstructions[newId] = obstruction;
        if(modelObstruction->isInvisibleAndUncollidable){
            obstruction->setOpacity(0);
        }
        
        if(modelObstruction->isUp){
            obstruction->setRotation(180.0f);
        }
    }
    
    
    //Remove the old obstructions
    for (auto obstructionToRemoveId : model->removedObstructionIds){
        Sprite *obstruction = this->obstructions[obstructionToRemoveId];
        obstructions.erase(obstructionToRemoveId);
        obstruction->removeFromParent();
    }
    
    //Update all the obstructions, position, appearance etc.
    for (auto obstructionData : model->obstructions){
        int obstructionId = obstructionData.first;
        Sprite *obstruction = this->obstructions[obstructionId];
        ModelObstruction *modelObstruction = model->obstructions[obstructionId];
        b2Vec2 modelPosition = modelObstruction->body->GetPosition();
        obstruction->setPosition(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
    }
}

void SceneSinglePlayer::updateFlowerJets(){
    //Add the new flowerjets
    for (auto newId : model->newFlowerJets){
        ModelFlowerJet *modelFlowerJet = model->flowerJets[newId];
        b2Vec2 modelPosition = modelFlowerJet->body->GetPosition();
        
        Sprite *flowerJet = Sprite::createWithSpriteFrameName(Constants::SpriteNames::flowerjet );
        flowerJet->setScale(modelFlowerJet->scale, modelFlowerJet->scale);
        Vec2 position = Vec2(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
        flowerJet->setPosition(position);
        this->addChild(flowerJet, FLOWERJET_SPRITE_Z_ORDER);
        this->flowerJets[newId] = flowerJet;
        
        flowerJet->setColor(Color3B(model->currentArt.tlR, model->currentArt.tlG, model->currentArt.tlB));
    }
    
    //Remove the old flowerjets
    for (auto flowerJetToRemoveId : model->removedFlowerJets){
        Sprite *flowerJet = this->flowerJets[flowerJetToRemoveId];
        if (flowerJet != NULL) {
            Vec2 position = flowerJet->getPosition();
            if (position.y > 100) {
                ParticleSystem* bfe = ParticleSystemQuad::create(Constants::pex::badFoodExplosion);
                this->addChild(bfe,PARTICLE_EXPLOSION_Z_ORDER);
                bfe->setPosition(position);
                SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::birdexplode);
            }
            
            flowerJet->removeFromParent();
        }
        
        flowerJets.erase(flowerJetToRemoveId);
    }
    
    //Update all flowerjets, position, appearance etc.
    for (auto flowerJetData : model->flowerJets){
        int flowerJetId = flowerJetData.first;
        Sprite *flowerJet = this->flowerJets[flowerJetId];
        ModelFlowerJet *modelFlowerJet = model->flowerJets[flowerJetId];
        b2Vec2 modelPosition = modelFlowerJet->body->GetPosition();
        flowerJet->setPosition(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
    }
}

void SceneSinglePlayer::updateBosses(){
    //Add the new bosses
    for (auto newId : model->newBosses){
        
        ModelBoss *modelBoss = model->bosses[newId];
        b2Vec2 modelPosition = modelBoss->body->GetPosition();
        float timePeriod = 0.5;
        float angle = 45;
        float angleInc = 45;
        Sprite *emptypixel,*emptypixelTop,*emptypixelBottom;
        Sprite *badFoodTop,*badFoodBottom;
        Sprite *badTeethTop,*badTeethBottom;
        Sprite *stem,*stemLeaves,*pot;
        
        badTeethTop = Sprite::createWithSpriteFrameName(Constants::SpriteNames::bossTeeth );
        badTeethTop->setFlippedY(true);
        badTeethBottom = Sprite::createWithSpriteFrameName(Constants::SpriteNames::bossTeeth );
        emptypixel = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
        emptypixelTop = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
        emptypixelBottom = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
        
        std::string treeIndex = model->currentArt.obstructionFrame;
        
        stem = Sprite::createWithSpriteFrameName("s" + treeIndex + "t");
        emptypixel->addChild(stem);
        
        if (model->currentArt.hasLeaves) {
            stemLeaves = Sprite::createWithSpriteFrameName("s" + treeIndex + "l");
            stemLeaves->setColor(Color3B(model->currentArt.tlR,model->currentArt.tlG,model->currentArt.tlB));
            stemLeaves->setAnchorPoint(Vec2(0.5,1.0));
            emptypixel->addChild(stemLeaves);
        }
        
        
        stem->setColor(Color3B(model->currentArt.trR,model->currentArt.trG,model->currentArt.trB));
        
        pot = Sprite::createWithSpriteFrameName(Constants::Boss::pot);
        pot->setColor(Color3B(model->currentArt.lgR, model->currentArt.lgG, model->currentArt.lgB));
        
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
            arm->setColor(Color3B(model->currentArt.trR, model->currentArt.trG, model->currentArt.trB));
            if (model->currentArt.hasLeaves) {
                Sprite* leaves = Sprite::createWithSpriteFrameName("s" + treeIndex + "l");
                leaves->setColor(Color3B(model->currentArt.tlR, model->currentArt.tlG, model->currentArt.tlB));
                
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
        
        badFoodTop->setColor(Color3B(model->currentArt.bgR,model->currentArt.bgG,model->currentArt.bgB));
        badFoodBottom->setColor(Color3B(model->currentArt.bgR,model->currentArt.bgG,model->currentArt.bgB));
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
            uppert->setColor(Color3B(model->currentArt.trR, model->currentArt.trG, model->currentArt.trB));
            badFoodTop->addChild(uppert);
            uppert->setScale(0.5);
            uppert->setAnchorPoint(Vec2(0.5, 0.0));
            
            
            if (model->currentArt.hasLeaves) {
                Sprite *upperl = Sprite::createWithSpriteFrameName("s" + treeIndex + "l");
                upperl->setPosition(badFoodTop->getContentSize().width/2, 0);
                upperl->setRotation(0 - 90 + (180/num)*i);
                upperl->setColor(Color3B(model->currentArt.tlR, model->currentArt.tlG, model->currentArt.tlB));
                upperl->setScale(0.5);
                upperl->setAnchorPoint(Vec2(0.5, 0.0));
                badFoodTop->addChild(upperl);
            }
        }
        
        for (int i = 0; i< num; i++){
            Sprite *uppert = Sprite::createWithSpriteFrameName("s" + treeIndex + "t");
            uppert->setPosition(badFoodTop->getContentSize().width/2, badFoodTop->getContentSize().height);
            uppert->setRotation(90 + (180/num)*i);
            uppert->setColor(Color3B(model->currentArt.trR, model->currentArt.trG, model->currentArt.trB));
            uppert->setScale(0.5);
            uppert->setAnchorPoint(Vec2(0.5, 0.0));
            badFoodBottom->addChild(uppert);
            
            
            if (model->currentArt.hasLeaves) {
                Sprite *upperl = Sprite::createWithSpriteFrameName("s" + treeIndex + "l");
                upperl->setPosition(badFoodTop->getContentSize().width/2, badFoodTop->getContentSize().height);
                upperl->setRotation(90 + (180/num)*i);
                upperl->setColor(Color3B(model->currentArt.tlR, model->currentArt.tlG, model->currentArt.tlB));
                upperl->setScale(0.5);
                upperl->setAnchorPoint(Vec2(0.5, 0.0));
                badFoodBottom->addChild(upperl);
            }
        }
        
        stem->setPosition(Vec2(0,0 - badFoodBottom->getContentSize().height/2));
        pot->setPosition(stem->getPosition());
        stem->setAnchorPoint(Vec2(0.5,1.0));
        pot->setAnchorPoint(stem->getAnchorPoint());
        
        Sprite *eyeball = Sprite::createWithSpriteFrameName(Constants::Bird::eye);
        eyeball->setPosition(Vec2(badFoodTop->getContentSize().width/4, badFoodTop->getContentSize().height/4));
        badFoodTop->addChild(eyeball);
        eyeball->setColor(Color3B(model->currentArt.byR, model->currentArt.byG, model->currentArt.byB));
        
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
        
        Vec2 position = Vec2(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
        emptypixel->setPosition(position);
        this->addChild(emptypixel, BOSS_SPRITE_Z_ORDER);
        this->bosses[newId] = emptypixel;
    }
    
    for (auto bossToRemoveId : model->removedBosses){
        Sprite *boss = this->bosses[bossToRemoveId];
        if (boss != NULL) {
            boss->removeFromParent();
            SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::birdexplode);
            SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::defeatboss);
        }
        
        bosses.erase(bossToRemoveId);
    }
    
    //Update all the obstructions, position, appearance etc.
    for (auto bossData : model->bosses){
        int bossId = bossData.first;
        Sprite *boss = this->bosses[bossId];
        ModelBoss *modelBoss = model->bosses[bossId];
        //        BAD_FOOD_TYPE type = modelBadFood->type;
        b2Vec2 modelPosition = modelBoss->body->GetPosition();
        boss->setPosition(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
    }
}

void SceneSinglePlayer::updateBadFoods(){
    
    //Add the new badfoods
    for (auto newId : model->newBadFoodIds){
        ModelBadFood *modelBadFood = model->badFoodNodes[newId];
        b2Vec2 modelPosition = modelBadFood->body->GetPosition();
        Vec2 position = Vec2(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);

        Sprite *emptypixel = Sprite::createWithSpriteFrameName(Constants::SpriteNames::emptyPixel );
        addBirdToEmptyPixel(emptypixel);
        emptypixel->setPosition(position);
        
        this->addChild(emptypixel, BAD_FOOD_SPRITE_Z_ORDER);
        this->badFoods[newId] = emptypixel;
    }
    
    for (auto badFoodToRemoveId : model->removedBadFoodIds){
        Sprite *badFood = this->badFoods[badFoodToRemoveId];
        if (badFood != NULL) {
            badFood->removeFromParent();
            updateBirdsBlasted();
        }
        
        badFoods.erase(badFoodToRemoveId);
    }
    
    //Update all the obstructions, position, appearance etc.
    for (auto badFoodData : model->badFoodNodes){
        int badFoodId = badFoodData.first;
        Sprite *badFood = this->badFoods[badFoodId];
        ModelBadFood *modelBadFood = model->badFoodNodes[badFoodId];
        b2Vec2 modelPosition = modelBadFood->body->GetPosition();
        badFood->setPosition(modelPosition.x*PTM_RATIO, modelPosition.y*PTM_RATIO);
        
    }
}

void SceneSinglePlayer::addBirdToEmptyPixel(Sprite* emptypixel){
    Sprite *rearwing = Sprite::createWithSpriteFrameName(model->currentArt.bwF );
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
    
    rearwing->setColor(Color3B(model->currentArt.bwR, model->currentArt.bwG, model->currentArt.bwB));
    frontwing->setColor(Color3B(model->currentArt.bwR, model->currentArt.bwG, model->currentArt.bwB));
    beak->setColor(Color3B(model->currentArt.bkR, model->currentArt.bkG, model->currentArt.bkB));
    body->setColor(Color3B(model->currentArt.bbR, model->currentArt.bbG, model->currentArt.bbB));
    eye->setColor(Color3B(model->currentArt.byR, model->currentArt.byG, model->currentArt.byB));
    eyeball->setColor(Color3B(model->currentArt.beR, model->currentArt.beG, model->currentArt.beB));
    
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
}

bool SceneSinglePlayer::onTouchBegan(Touch *touch, Event *event){
    
    if (!isPaused) {
        if (!isGameStopped) {
            TouchEvent touchEvent;
            this->receivedTouches.push_back(touchEvent);
            SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::flap);
        }
        
        if (!isGameStarted) {
            isGameStarted = true;
            hideInstruction();
        }
    }
    
    return true;
}

void SceneSinglePlayer::rankingCallback(Ref* pSender){
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    gch->showLeaderboard(Constants::LeaderboardIds::defaultLeaderboardId);
#endif
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniBridge::showLeaderboard();
#endif
}

void SceneSinglePlayer::rateCallback(Ref* pSender)
{
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    gch->rateApp();
#endif
    
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    JniBridge::rate();
#endif
}

void SceneSinglePlayer::playCallback(Ref* pSender){
    GameStatus::savePoints.clear();
    GameStatus::birdsBlasted = 0;
    Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneSinglePlayer::createScene()));
}

void SceneSinglePlayer::continueCallBack(Ref* pSender){
    GameStatus::birdsBlasted = 0;
    Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneSkipTransition::createScene()));
}

void SceneSinglePlayer::colorsCallBack(Ref* pSender)
{
    ModelArt::getInstance()->isBW = !ModelArt::getInstance()->isBW;
    CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    
    if (!GameStatus::savePoints.empty() && !model->isLastLevelReached()) {
        Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneSkipTransition::createScene()));
    }else{
        Director::getInstance()->replaceScene(TransitionSlideInR::create(1.0, SceneSinglePlayer::createScene()));
    }
    
}

void SceneSinglePlayer::toggleSFXCallback(Ref* pSender){
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
