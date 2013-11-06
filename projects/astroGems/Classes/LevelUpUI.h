//
//  LevelUpUI.h
//  astroGems
//
//  Created by Alex Gievsky on 04.11.13.
//
//

#ifndef __astroGems__LevelUpUI__
#define __astroGems__LevelUpUI__

#include "StackableLayer.h"
#include "cocos2d.h"

USING_NS_CC;

class GameScene;

class LevelUpUI: public Layer, public StackableLayer {
public:
    virtual ~LevelUpUI();
    LevelUpUI();
    
    virtual bool init();
    CREATE_FUNC(LevelUpUI);

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
    Sprite *star;
    LayerColor *whiteShadow;
    Sprite *labelMount;
    LabelBMFont *lvLabel;
    LabelBMFont *levelUpLabel;
};

#endif /* defined(__astroGems__LevelUpUI__) */
