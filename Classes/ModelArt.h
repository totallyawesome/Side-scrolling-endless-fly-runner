//
//  ModelArt.h
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#ifndef __SiderScroller__ModelArt__
#define __SiderScroller__ModelArt__

#include <stdio.h>
#include "AppConstants.h"
#include "GameMacros.h"
#include "ModelColorSet.h"

NS_RIY_BEGIN

struct ArtChunk {
    std::string lowerGroundFrame;
    std::string midGroundFrame;
    std::string topGroundFrame;
    std::string aboveGroundFrame;

    unsigned char lgR;
    unsigned char lgG;
    unsigned char lgB;

    unsigned char mgR;
    unsigned char mgG;
    unsigned char mgB;

    unsigned char tgR;
    unsigned char tgG;
    unsigned char tgB;

    unsigned char agR;
    unsigned char agG;
    unsigned char agB;

    std::string backgroundFrame;
    unsigned char bgR;
    unsigned char bgG;
    unsigned char bgB;
    bool flpY;

    std::string obstructionFrame;
    bool hasLeaves;
    unsigned char trR; //Trunk color
    unsigned char trG;
    unsigned char trB;
    unsigned char tlR; //Leaf color
    unsigned char tlG;
    unsigned char tlB;

    //Particle
    int noP; // Particles to spawn at a time
    float pSD; //particle spawn delay
    int xPL; // xPush Limit
    int yPL; // yPush Limit
    float xSK; // xOffset for changing slant of weather
    unsigned char paR;
    unsigned char paG;
    unsigned char paB;

    //Clouds
    int noC; // Number of clouds. Each cloud will be made up of a random number of subclouds from 3 - 12
    float msC; //Maximum scale of the subclouds. Scale will be between 0.5 and msC
    float gbC; // Gap between clouds as a percentage of screen size.
    unsigned char clR;
    unsigned char clG;
    unsigned char clB;
    unsigned char clO; // make this random

    //Bird
    std::string bwF; //birdy wing frame
    unsigned char bbR; //Birdy body color
    unsigned char bbG;
    unsigned char bbB;
    unsigned char bkR; //Birdy beak color
    unsigned char bkG;
    unsigned char bkB;
    unsigned char bwR; //Birdy wing color
    unsigned char bwG;
    unsigned char bwB;
    unsigned char byR; //Birdy eye color
    unsigned char byG;
    unsigned char byB;
    unsigned char beR; //Birdy eyeball color
    unsigned char beG;
    unsigned char beB;

    //Sun
    bool hSun; // Has Sun ?
    unsigned char suR;
    unsigned char suG;
    unsigned char suB;

    //Moon
    bool hMoon; // Has Moon ?
    unsigned char moR;
    unsigned char moG;
    unsigned char moB;

    //Stars
    bool hStars; // Has Stars ?
    unsigned char stR;
    unsigned char stG;
    unsigned char stB;

    //Ground flowers
    unsigned char gfR;
    unsigned char gfG;
    unsigned char gfB;
};

class ModelArt {

private:
    std::vector<ArtChunk> art;
    std::vector<ArtChunk> bwArt;

    std::vector<ModelColorSet> dayColorsets;
    std::vector<ModelColorSet> bwColorsets;

    std::vector<std::string>treenames;

    std::vector<std::string>wingnames;

    int bossIndex;

    int modifyPri(int pri,int modifier,int numberoftimestomodify);
    std::string treeForIndex(int index);
    std::string wingForIndex(int index);

    void loadTreeNames();
    void loadWingNames();
    float calculateLumOfColor(unsigned char r,unsigned char g,unsigned char b);
public:
    bool isBW;
    std::string bwString;
    std::string dayStr;

    void init();
    void createColorSets(std::vector<ModelColorSet>& colorSets,std::string colstr);
    ModelColorSet createColorSet(std::string colstr);
    ModelColorSet createSortedColorSet(std::string colstr);
    ArtChunk artChunkFromColorSet(ModelColorSet set,int i);
    ArtChunk artChunkForASegmentIndex(int aSegmentIndex);

    std::vector<std::string> splitString(std::string input,char delim);

    static ModelArt* getInstance(void);

    ModelArt(){
        init();
    };

};

NS_RIY_END

#endif /* defined(__SiderScroller__ModelArt__) */
