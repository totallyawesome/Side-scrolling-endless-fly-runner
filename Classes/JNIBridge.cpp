//
//  JNIBridge.cpp
//  Side Scroller
//
//  Created by Rahul Iyer
//
//

#include "JNIBridge.h"
#define CLASS_NAME "org/cocos2dx/cpp/AppActivity"
#define SHOW_ADS_METHOD_NAME "showAd"
#define SHOW_ADS_PARAM_CODE "()V"
#define HIDE_ADS_METHOD_NAME "hideAd"
#define HIDE_ADS_PARAM_CODE "()V"
USING_NS_CC;

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include "jni/JniHelper.h"
#endif

bool JniBridge::isAdShowing = false;

void JniBridge::showAds(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("Show ads in cpp called");
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method, CLASS_NAME, SHOW_ADS_METHOD_NAME,SHOW_ADS_PARAM_CODE);
    method.env->CallStaticVoidMethod(method.classID, method.methodID);
    method.env->DeleteLocalRef(method.classID);
    isAdShowing = true;

#endif

}

void JniBridge::rate(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("Rate in cpp called");
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method, CLASS_NAME, "rate","()V");
    method.env->CallStaticVoidMethod(method.classID, method.methodID);
    method.env->DeleteLocalRef(method.classID);
    isAdShowing = true;

#endif

}

void JniBridge::hideAds(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("Hide ads in cpp called");
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method, CLASS_NAME, HIDE_ADS_METHOD_NAME,HIDE_ADS_PARAM_CODE);
    method.env->CallStaticVoidMethod(method.classID, method.methodID);
    method.env->DeleteLocalRef(method.classID);
    isAdShowing = false;
#endif
}

void JniBridge::submitScore(int score){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("Submit score cpp called");
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method, CLASS_NAME,"submitScore","(Ljava/lang/String;)V");
    std::string s = std::to_string(score);
    char const *message = s.c_str();
    jstring stringMessage = method.env->NewStringUTF(message);
    method.env->CallStaticVoidMethod(method.classID,method.methodID, stringMessage);
    method.env->DeleteLocalRef(stringMessage);
    method.env->DeleteLocalRef(method.classID);
#endif
}

void JniBridge::showLeaderboard(){
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
    CCLOG("Show leaderboard in cpp called");
    JniMethodInfo method;
    JniHelper::getStaticMethodInfo(method, CLASS_NAME, "showLeaderboard","()V");
    method.env->CallStaticVoidMethod(method.classID, method.methodID);
    method.env->DeleteLocalRef(method.classID);
#endif
}
