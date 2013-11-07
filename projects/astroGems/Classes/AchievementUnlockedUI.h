//
//  AchievementUnlockedUI.h
//  astroGems
//
//  Created by Alex Gievsky on 07.11.13.
//
//

#ifndef __astroGems__AchievementUnlockedUI__
#define __astroGems__AchievementUnlockedUI__

#include "StackableLayer.h"
#include "cocos2d.h"

USING_NS_CC;

class GameScene;

class AchievementUnlockedUI: public Layer, public StackableLayer {
public:
    virtual ~AchievementUnlockedUI();
    AchievementUnlockedUI();
    
    virtual bool init();
    CREATE_FUNC(AchievementUnlockedUI);
    
    // stackable layer stuff
    virtual void popUp(StackableLayer *baseLayer);
	virtual void popOut();
	virtual void putOn(StackableLayer *overlay);
	virtual void takeOff();
	virtual void disableTouches();
	virtual void enableTouches();
    
    void onEnter();
    
    void setGameLayer(GameScene *layer);
private:
    GameScene *gameLayer;
    
    Sprite *shine;
    Sprite *achievement;
    Sprite *achievementFrame;
    LayerColor *whiteShadow;
    Sprite *labelMount;
    LabelBMFont *achievementLabel;
};


#endif /* defined(__astroGems__AchievementUnlockedUI__) */
