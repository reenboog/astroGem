
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "FieldWatcherDelegate.h"
#include "StackableLayer.h"

USING_NS_CC;

class GemField;
class GameUI;
class AchievementsUI;
class LevelUpUI;

class GameScene: public cocos2d::Layer, public FieldWatcherDelegate, public StackableLayer {
public:
    enum GameSceneState {
        GSS_Normal,
        GSS_LevelUp,
        GSS_AchievementUnlocked
    } state;
    
    virtual ~GameScene();
    GameScene();
    // cocos2d stuff
    virtual bool init();
    static cocos2d::Scene * scene();
    CREATE_FUNC(GameScene);
    
    void reset();

    // touches
	virtual void ccTouchEnded(Touch *touch, Event *event);
	virtual bool ccTouchBegan(Touch *touch, Event *event);
	virtual void ccTouchMoved(Touch *touch, Event *event);
    
    // touch extras
    Point convertLocationToFieldCoordinates(Point point);
    Point convertFieldCoordinatesToWorldLocation(const Point &point);

    // field watcher delegate methods
    virtual void onGemDestroyed(GemColour colour, int x, int y, int score);
	virtual void onGemsMatched(int length, GemColour colour, int startX, int startY, int endX, int endY, int score);
	virtual void onGemsToBeShuffled();
	virtual void onGemsStartedSwapping();
	virtual void onGemsFinishedMoving();
	virtual void onMoveMade(bool legal);
    virtual void onStartedResolvinMatches(const MatchList &founMatches);

    virtual void onRainbowGemDestroyed(int x, int y);
    
    // stackabl layer virtuals
    virtual void popUp(StackableLayer *baseLayer);
	virtual void popOut();
	virtual void putOn(StackableLayer *overlay);
	virtual void takeOff();
	virtual void disableTouches();
	virtual void enableTouches();
    
    //void setTimeLeft(float time);
    
    void onEnter();
    
    // ui
    void onMakeFunBtnPressed();
    void onPauseBtnPressed();
    
    // some setters/getters
    void setUI(GameUI *ui);
    void setAchievementsUI(AchievementsUI *ui);
    void setLevelUpUI(LevelUpUI *ui);
    
    void setScore(int score);
    void applyScoreMultiplierProgress(float progress);
    void setScoreMultiplierProgress(float progress);
    void setCurrentLevel(int level);
        
    void applyCoins(int coins, int fromX, int fromY);
    
    void applyNextBackground();
    
    // update logic
    void update(float dt);
private:
    void setState(GameSceneState state);
private:
    GemField *field;
    Sprite *back;
    
    GameUI *ui;
    AchievementsUI *achievementsUI;
    LevelUpUI *levelUpUI;
    
    bool swipeEnded;
	bool canTouch;

    //bool gameOver;
    //float timeLeft;
    
    // score multiplier stuff
    struct ScoreMultiplier {
        enum ScoreMultiplierState {
            SMS_Normal,
            SMS_Active,
            SMS_FadingOut
        
        } state;
        
        ScoreMultiplier(): multiplier(0), currentProgress(0), activityTimer(0), state(SMS_Normal) {}
        
        void fadeOut() {
            activityTimer = 0.0f;
            state = SMS_FadingOut;
            multiplier = 1.0f;
        }
        
        void rewind() {
            activityTimer = 0;
            state = SMS_Normal;
            multiplier = 1.0f;
        }
        
        void apply() {
            multiplier = kScoreExtraMultiplier;
            state = SMS_Active;
            activityTimer = kScoreMultiplierActiveTime;
        }
        
        float multiplier;
        float currentProgress;
        float activityTimer;
    } scoreMultiplier;

    int currentScore;
    int currentRainbowGems;
    int currentBackgroundIndex;
    
    Point firstTouchLocation;
};

#endif
