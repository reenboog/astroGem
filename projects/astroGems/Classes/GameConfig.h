//
//  GameConfig.h
//  1pGigTickets
//
//  Created by Alex Gievsky on 13.09.13.
//
//

#ifndef ___pGigTickets__GameConfig__
#define ___pGigTickets__GameConfig__

#include "cocos2d.h"
#include "Constants.h"

#define kAppVersion         1
#define kGameTimer          60

#define kBaseIconValue      50
#define kFourInRowIconValue 500
#define kHypercubeIconValue 1000

#define kInitialRainbowGemsRequiredAchievement 2


class GameConfig {
private:
    GameConfig();
public:
    ~GameConfig();
    
    void load();
    void save();
    
    static GameConfig * sharedInstance();
private:
    static GameConfig *__sharedInstance;
public:
    int version;
    
    float gameTimer;
    
    float rainbowGemsRequiredForAchievement;
    
    float baseIconValue;
    float fourInRowIconValue;
    float hypercubeIconValue;
    
    int currentCoins;
    int currentLevel;
    int currentAchievementIndex;
    
    AchievementPool achievements;
    
    // required to align ui
    cocos2d::Point gridPos;
    cocos2d::Point levelLabelGUIPos;
};

#endif /* defined(___pGigTickets__GameConfig__) */
