//
//  GameUI.h
//  astroGems
//
//  Created by Alex Gievsky on 09.10.13.
//
//

#ifndef __astroGems__GameUI__
#define __astroGems__GameUI__

#include "cocos2d.h"

USING_NS_CC;

class GameScene;

class GameUI: public Layer {
public:
    ~GameUI();
    GameUI();
    // cocos2d stuff
    virtual bool init();
    CREATE_FUNC(GameUI);
    
    void reset();
    // callbacks
    void onPauseBtnPressed(Object *sender);
    
    void setGameLayer(GameScene *layer);
    
    void setScore(int value);
    void setLevel(int lv);
    void setCoins(int coins);
    //void setTimeLeft(float seconds);
    //void setScoreMultiplier(float multiplier);
    void setScoreMultiplierProgress(float progress);

    void setRainbowGemsProgress(float progress);
private:
    //ProgressTimer *timeProgress;
    ProgressTimer *rainbowGemsProgress;
    ProgressTimer *multiplierProgress;
    LabelBMFont *scoreLabel;
    LabelBMFont *levelLabel;
    LabelBMFont *coinsLabel;
    //LabelBMFont *timeLabel;
    MenuItem *pauseBtn;

    GameScene *gameLayer;
};

#endif /* defined(__astroGems__GameUI__) */
