//
//  FastBeginScene.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "FastBeginScene.h"
#include "SceneMenuMain.h"
#include "SceneTestColors.h"
#include "SceneEndGame.h"
#include "SceneCredits.h"
#include "SoundManager.h"

USING_NS_RIY;
NS_RIY_BEGIN

Scene* FastBeginScene::createScene()
{
    // 'scene' is an autorelease object
    auto scene = Scene::create();

    // 'layer' is an autorelease object
    auto layer = FastBeginScene::create();

    // add layer as a child to scene
    scene->addChild(layer);

    // return the scene
    return scene;
}

// on "init" you need to initialize your instance
bool FastBeginScene::init(){
    //////////////////////////////
    // 1. super init first
    //This is the same as the main menu background color
    if ( !LayerColor::initWithColor(Color4B(153, 170, 187, 255) )) {
        return false;
    }
    isFading = false;
    totalTime = 0;
    loadedSprites = false;
    this->scheduleUpdate();
    return true;
}

void FastBeginScene::update(float dt){
    if (!loadedSprites) {

        loadedSprites = true;
        SpriteFrameCache* cache = SpriteFrameCache::getInstance();
        std::string plist = en(FileUtils::getInstance()->getStringFromFile("colors.far"));
        std::string png = en(FileUtils::getInstance()->getStringFromFile("levels.far"));

        int len = 0;
        unsigned char *buffer;
        len = base64Decode((unsigned char*)plist.c_str(), (unsigned int)plist.length(), &buffer);
        std::string dplist(reinterpret_cast<char*>(buffer));

        len = base64Decode((unsigned char*)png.c_str(), (unsigned int)png.length(), &buffer);
        std::string dpng(reinterpret_cast<char*>(buffer));

        Image* img = new Image();
        bool ok = img->initWithImageData(buffer,len);

        if (!ok) {
            CCLOG("What me worry?");
        }

        Texture2D* tex = nullptr;
        tex = Director::getInstance()->getTextureCache()->addImage(img, "balls");

        cache->addSpriteFramesWithFileContent(dplist, tex);

    }else{
        totalTime+=dt;
        if (totalTime > 2) {
            if (!isFading) {
                isFading = true;
                SoundManager::getInstance()->playSoundEffectName(Constants::SoundEffects::menu);
                Director::getInstance()->replaceScene(TransitionCrossFade::create(1.0, SceneMenuMain::createScene()));
            }
        }
    }
}

std::string FastBeginScene::en(std::string s) {
    std::string o = "";
    for (int i = 0; i< s.length(); i+=2){

        if(i+1<s.length()){
            o += s[i+1];
        }

        o += s[i];
    }

    return o;
}

NS_RIY_END
