//
//  SoundManager.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "SoundManager.h"

NS_RIY_BEGIN
using namespace cocos2d;

static SoundManager *_sharedSoundManager = NULL;

SoundManager* SoundManager::getInstance()
{
    if (! _sharedSoundManager)
    {
        _sharedSoundManager = new SoundManager();
        _sharedSoundManager->init();
    }

    return _sharedSoundManager;
}

bool SoundManager::init(void)
{
    muteAudio = false;
    return true;
}

void SoundManager::toggleAudio(){
    muteAudio = !muteAudio;
    if (muteAudio) {
        CocosDenshion::SimpleAudioEngine::getInstance()->stopAllEffects();
    }
}

bool SoundManager::isMuted(){
    return muteAudio;
}

void SoundManager::playSoundEffectName(std::string soundEffectName){

    if (!muteAudio) {
        std::string completeName = getFullName(soundEffectName);

        if (completeName != soundEffectName) {
            CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(completeName.c_str());
        }
    }
}

void SoundManager::playMusic(std::string musicName){
    if (!muteAudio) {
        if (!CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying()) {

            std::string completeName = getFullName(musicName);

//            CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(completeName.c_str(), false);


//            CocosDenshion::SimpleAudioEngine::getInstance()->playBackgroundMusic(completeName.c_str());
        }

    }
}

void SoundManager::stopMusic(){
//    CocosDenshion::SimpleAudioEngine::getInstance()->stopBackgroundMusic();
}

void SoundManager::pauseAudio(){
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseAllEffects();
    CocosDenshion::SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

void SoundManager::resumeAudio(){
    if (!muteAudio) {
        CocosDenshion::SimpleAudioEngine::getInstance()->resumeAllEffects();
        CocosDenshion::SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    }
}

bool SoundManager::isMusicPlaying(){
    return CocosDenshion::SimpleAudioEngine::getInstance()->isBackgroundMusicPlaying();
}

void SoundManager::preloadSoundEffectName(std::string soundEffectName){

    std::string completeName = getFullName(soundEffectName);

    if (completeName != soundEffectName) {
        CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(completeName.c_str());
    }
}

void SoundManager::unloadSoundEffectName(std::string soundEffectName){

    std::string completeName = getFullName(soundEffectName);

    if (completeName != soundEffectName) {
        CocosDenshion::SimpleAudioEngine::getInstance()->unloadEffect(completeName.c_str());
    }
}



std::string SoundManager::getFullName(std::string soundEffectName){
    std::string completeName;
#if CC_TARGET_PLATFORM == CC_PLATFORM_IOS
    completeName = soundEffectName + Constants::FileSuffixes::caf;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_MAC
    completeName = soundEffectName + Constants::FileSuffixes::caf;
#elif CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    completeName = soundEffectName + Constants::FileSuffixes::ogg;
#endif
    return completeName;
}

NS_RIY_END
