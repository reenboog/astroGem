
#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "FieldWatcherDelegate.h"

USING_NS_CC;

class GemField;
class GameUI;

class GameScene: public cocos2d::Layer, public FieldWatcherDelegate {
public:
    ~GameScene();
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
    
    //void setTimeLeft(float time);
    
    void onMakeFunBtnPressed();
    
    // some setters/getters
    void setUI(GameUI *ui);
    void setScore(int score);
    void applyScoreMultiplierProgress(float progress);
    void setScoreMultiplierProgress(float progress);
        
    void applyCoins(int coins, int fromX, int fromY);
    
    // update logic
    void update(float dt);
private:
    GemField *field;
    Sprite *back;
    
    GameUI *ui;
    
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
    
    Point firstTouchLocation;
};

#endif
