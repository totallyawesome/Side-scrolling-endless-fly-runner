#include <vector>
#include <string>
#include "AppDelegate.h"
#include "AppMacros.h"
#include "SceneMenuMain.h"
#include "SceneSinglePlayer.h"
#include "SceneSkipTransition.h"
#include "FastBeginScene.h"

USING_NS_CC;
USING_NS_RIY;
AppDelegate::AppDelegate() {
    
}

AppDelegate::~AppDelegate()
{
}

//if you want a different context,just modify the value of glContextAttrs
//it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    //set OpenGL context attributions,now can only set six attributions:
    //red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};
    
    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; //flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching() {
    
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLViewImpl::create("My Game");
        director->setOpenGLView(glview);
    }
    
    glview->setDesignResolutionSize(designResolutionSize.width, designResolutionSize.height, ResolutionPolicy::FIXED_HEIGHT);
    
    std::vector<std::string> searchPath;
    searchPath.push_back("res");
    Size frameSize = glview->getFrameSize();
    
    if(frameSize.height < stringset.size.height){
        searchPath.push_back(charset.directory);
        director->setContentScaleFactor(charset.size.height/designResolutionSize.height);
    }else{
        searchPath.push_back(stringset.directory);
        director->setContentScaleFactor(stringset.size.height/designResolutionSize.height);
    }

    
    // set searching path
    FileUtils *fileUtils = FileUtils::getInstance();
    fileUtils->setSearchPaths(searchPath);
    
    // turn on display FPS
    director->setDisplayStats(false);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    register_all_packages();
    
    // create a scene. it's an autorelease object
    auto scene = FastBeginScene::createScene();
    
    // run
    director->runWithScene(scene);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    Scene *scene = Director::getInstance()->getRunningScene();
    Vector<Node*> children = scene->getChildren();
    for(Node* child : children)
    {
        //For each pausable game scene, add code to handle interruption here.
        SceneSinglePlayer* sps = dynamic_cast<SceneSinglePlayer*>(child);
        if (sps!=NULL) {
            sps->pauseGame();
            break;
        }else{
            SceneSkipTransition* skp = dynamic_cast<SceneSkipTransition*>(child);
            if (skp!=NULL) {
                skp->pauseGame();
                break;
            }
        }
    }
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
}