//
//  ViewPlayer.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "ViewPlayer.h"

NS_RIY_BEGIN
void ViewPlayer::init(Layer *scene){
    std::string firstFrameName = Constants::SpriteNames::playerInitialFrame ;

    _isFlying = true;
    sprite = Sprite::createWithSpriteFrameName(firstFrameName);
    scene->addChild(sprite, PLAYER_SPRITE_Z_ORDER);

    this->startWalking();
}

void ViewPlayer::startWalking(){
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
    this->sprite->runAction(RepeatForever::create(Animate::create(walkingAnimation)));
}


NS_RIY_END
