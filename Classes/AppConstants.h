//
//  AppConstants.h
//  Side Scroller
//
//  Created by Rahul Iyer
//

#ifndef muchly_AppConstants_h
#define muchly_AppConstants_h

#include "AppMacros.h"
#include <string>
#include "ModelSavePoint.h"
using namespace std;

NS_RIY_BEGIN

typedef enum{
    PLAYER_INITIAL_STATE = 0,
}PLAYER_STATE;

namespace Constants{

    namespace Textures{
        static const std::string gameSprites = "utf16";
        static const std::string menuSprites = "menusprites";
        static const std::string backgroundSprites = "backgroundprites";
    }

    namespace FileSuffixes{
        static const std::string png = ".png";
        static const std::string plist = ".plist";
        static const std::string wav = ".wav";
        static const std::string ogg = ".ogg";
        static const std::string caf = ".caf";
        static const std::string mp3 = ".mp3";

    }

    namespace SoundEffects{
        static const std::string flap = "flap";
        static const std::string menu = "menu";
        static const std::string birdexplode = "birdexplode";
        static const std::string checkpoint = "checkpoint";
        static const std::string defeatboss = "defeatboss";
        static const std::string level = "level";
        static const std::string collisionSFX = "fartersoncollision";
        static const std::string scoreSFX = "score";
        static const std::string munch = "munch";
    }

    namespace MenuImageNames{
        static const std::string back = "back";
        static const std::string backselected = "backselected";
        static const std::string play = "play";
        static const std::string playselected = "playselected";
        static const std::string rate = "rate";
        static const std::string rateselected = "rateselected";
        static const std::string rankings = "rankings";
        static const std::string rankingsselected = "rankingsselected";
        static const std::string iap = "iap";
        static const std::string iapselected = "iapselected";
        static const std::string removeads = "removeads";
        static const std::string removeadsselected = "removeadsselected";
        static const std::string restorepurchases = "restorepurchases";
        static const std::string restorepurchasesselected = "restorepurchasesselected";
        static const std::string continueImageName = "continue";
        static const std::string continueSelectedImageName = "continueselected";
        static const std::string copyright = "copyright";
        static const std::string copyrightselected = "copyrightselected";
        static const std::string colors = "colors";
        static const std::string colorselected = "colorselected";
        static const std::string bw = "bws";
        static const std::string bwselected = "bwselected";
        static const std::string musicon = "musicon.png";
        static const std::string musicoff = "musicoff.png";
    }

    namespace UserDefaults{
        static const std::string bestScore = "b";
        static const std::string bestDistance = "d";
    }

    namespace LeaderboardIds{
//        static const std::string defaultLeaderboardId = "topscore"; //Removing topscore for now.
        static const std::string defaultLeaderboardId = "distance";
        static const std::string distanceLeaderboardId = "distance";
    }

    namespace MenuButtonTitles{
        static const std::string removeAds = "Remove Ads";
        static const std::string restorePurchases = "Restore purchases";
    }

    namespace SpriteNames{
        static const std::string emptyPixel = "emptypixel";
        static const std::string flowerjet = "flowerjet";
        static const std::string badTeeth = "badteeth";
        static const std::string bossTeeth = "bossteeth";
        static const std::string bossstem = "bossstem";
        static const std::string bossforearm = "branch2";
        static const std::string bossreararm = "branch1";
        static const std::string oneUp = "1up";
        static const std::string playerInitialFrame = "walk1";
        static const std::string playerWalkCycleText = "walk%d";
        static const std::string score = "score";
        static const std::string trophy = "trophy";
        static const std::string distanceLabel = "distance";
        static const std::string bestDistanceLabel  = "bestdistance";
        static const std::string pause = "pause";
        static const std::string bigpause = "bigpausebutton";
        static const std::string musicalbean = "musicalbean";
        static const std::string tapInstruction1 = "tapinstruction1";
        static const std::string tapInstruction2 = "tapinstruction2";
        static const std::string onion = "onion";

        static const std::string ground = "ground";
        static const std::string groundupper = "grupper1";
        static const std::string groundlower = "grlower1";
        static const std::string groundTop = "grtop";
        static const std::string aboveground = "aboveground";


        static const std::string badPizza = "badpizza";
        static const std::string top = "top";
        static const std::string bottom = "bottom";
        static const std::string pizzaBoss = "pizzaboss";
        static const std::string background = "white";
        static const std::string backgroundgradient = "whiteupdark";

        static const std::string particle = "particle";

        static const std::string tearfix = "tearfix";

    }

    namespace Strokes{
        static const std::string spiral = "spiral";
        static const std::string spot = "spot";
        static const std::string verticalstroke = "verticalstroke";
    }

    namespace Boss {
        static const std::string stem = "stem";
        static const std::string pot = "pot";
    }

    namespace Clouds{
        static const std::string cloud1 = "cloud1";
        static const std::string cloud2 = "cloud2";
        static const std::string cloud3 = "cloud3";
        static const std::string cloud4 = "cloud4";
        static const std::string cloud5 = "cloud5";
        static const std::string cloud6 = "cloud6";
        static const std::string cloud7 = "cloud7";
        static const std::string cloud8 = "cloud8";
        static const std::string cloud9 = "cloud9";
    }

    namespace Space{
        static const std::string sun = "sun";
        static const std::string moon = "moon";
        static const std::string star = "star%d"; // Between 3 and 20
    }

    namespace Bird {
        static const std::string body = "birdbody";
        static const std::string beak = "birdbeak";
        static const std::string eye = "birdeye";
        static const std::string eyeball = "birdeyeball";

        static const std::string wing1 = "wing1";
        static const std::string wing2 = "wing2";
        static const std::string wing3 = "wing3";
        static const std::string wing4 = "wing4";
        static const std::string wing5 = "wing5";
        static const std::string wing6 = "wing6";
        static const std::string wing7 = "wing7";
        static const std::string wing8 = "wing8";
        static const std::string wing9 = "wing9";
        static const std::string wing10 = "wing10";
        static const std::string wing11 = "wing11";
        static const std::string wing12 = "wing12";
        static const std::string wing13 = "wing13";
        static const std::string wing14 = "wing14";
        static const std::string wing15 = "wing15";
        static const std::string wing16 = "wing16";
        static const std::string wing17 = "wing17";
        static const std::string wing18 = "wing18";
        static const std::string wing19 = "wing19";
        static const std::string wing20 = "wing20";
        static const std::string wing21 = "wing21";
    }

    namespace Music{
        static const std::string changes = "changes";
        static const std::string sunset = "sunset";
    }

    namespace Trees{
        static const std::string tree1 = "tree1";
        static const std::string tree2 = "tree2";
    }

    namespace Shapes{
        static const std::string badFood = "birdbody";
        static const std::string boss = "pizzabosstop";
        static const std::string bossstem = "bossstem";
        static const std::string oneUp = "1up";
        static const std::string skip = "musicalbean";
    }

    //Particle effects. Files should be saved in res so cocos2d-x can find them.
    namespace pex{
        static const std::string badFoodExplosion = "onexplosionparticleeffect.plist";
        static const std::string flowerjetcollision = "onflowerjetcollision.plist";
        static const std::string jetflame = "jetflame.plist";
        static const std::string fasttravel = "fasttravel.plist";
    }
};

NS_RIY_END
#endif
