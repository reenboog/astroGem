
#include "AppDelegate.h"
#include "GameScene.h"
#include "GameConfig.h"

#include "Localized.h"

USING_NS_CC;

using namespace std;

AppDelegate::AppDelegate() {

}

AppDelegate::~AppDelegate() {
    GameConfig::sharedInstance()->save();
    
    delete GameConfig::sharedInstance();
    Localized::purge();
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    Director * director = Director::getInstance();
    EGLView * eglView = EGLView::getInstance();

    director->setOpenGLView(eglView);
	
    // turn on display FPS
    director->setDisplayStats(false);

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    // load localized strings
    Localized::load();
    
    // load game config
    GameConfig::sharedInstance()->load();

    // create a scene. it's an autorelease object
    Scene *scene = GameScene::scene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    GameConfig::sharedInstance()->save();

    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();

    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
}