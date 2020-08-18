//
//  SoundManager.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__SoundManager__
#define __SiderScroller__SoundManager__

#include "AppConstants.h"
#include "AppMacros.h"
#include "cocos2d.h"
#include "SimpleAudioEngine.h"
#include "GameMacros.h"


NS_RIY_BEGIN
using namespace cocos2d;
class SoundManager {

private:

    std::string getFullName(std::string soundEffectName);
    std::string lastPlayedMusic;
    int lastPlayedId;
    bool muteAudio;

public:

    void playSoundEffectName(std::string soundEffectName);
    void playMusic(std::string musicName);
    void stopMusic();
    void pauseAudio();
    void resumeAudio();

    void preloadSoundEffectName(std::string soundEffectName);
    void unloadSoundEffectName(std::string soundEffectName);

    void toggleAudio();
    bool isMuted();
    bool isMusicPlaying();

    static SoundManager* getInstance(void);


    SoundManager(){
    };


    virtual ~SoundManager()
    {
    };

    bool init(void);
};

NS_RIY_END
#endif /* defined(__SiderScroller__SoundManager__) */
