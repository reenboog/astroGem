
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "FieldWatcherDelegate.h"

USING_NS_CC;

class GemField;

class GameScene: public cocos2d::Layer, public FieldWatcherDelegate {
public:
    ~GameScene();
    GameScene();
    // cocos2d stuff
    virtual bool init();
    static cocos2d::Scene * scene();
    CREATE_FUNC(GameScene);

    // touches
	virtual void ccTouchEnded(Touch *touch, Event *event);
	virtual bool ccTouchBegan(Touch *touch, Event *event);
	virtual void ccTouchMoved(Touch *touch, Event *event);
    
    // touch extras
    Point convertLocationToFieldCoordinates(Point point);

    // field watcher delegate methods
    virtual void onGemDestroyed(GemColour colour);
	virtual void onGemsMatched(int length, GemColour colour);
	virtual void onGemsToBeShuffled();
	virtual void onGemsStartedSwapping();
	virtual void onGemsFinishedMoving();
	virtual void onMoveMade(bool legal);
    
    // update logic
    void update(float dt);
private:
    GemField *field;
    Sprite *back;
    
    bool swipeEnded;
	bool canTouch;
    
    Point firstTouchLocation;
};

#endif
