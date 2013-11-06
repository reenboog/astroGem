//
//  GameConfig.cpp
//  1pGigTickets
//
//  Created by Alex Gievsky on 13.09.13.
//
//

#include "GameConfig.h"
#include "Localized.h"

USING_NS_CC;

#define kCurrentAchievementIndexKey "currentAchievementIndex"
#define kCurrentLevelKey "currentLevel"
#define kCurrentCoinsKey "currentCoins"

GameConfig * GameConfig::__sharedInstance = nullptr;

GameConfig::~GameConfig() {
    
}

GameConfig::GameConfig() {
    
}

GameConfig * GameConfig::sharedInstance() {
    if(__sharedInstance == nullptr) {
        __sharedInstance = new GameConfig();
    }
    
    return __sharedInstance;
}

void GameConfig::load() {
    version = kAppVersion;
    
    gameTimer = kGameTimer;
    
    baseIconValue = kBaseIconValue;
    fourInRowIconValue = kFourInRowIconValue;
    hypercubeIconValue = kHypercubeIconValue;
    
    rainbowGemsRequiredForAchievement = kInitialRainbowGemsRequiredAchievement;
    
    currentAchievementIndex = 0;
    currentCoins = 0;
    currentLevel = 0;

    int itemIndex = 0;
    
    bool continueLoop = true;
    do {
        char buff[100];
        sprintf(buff, "%s%i", kAchievementPrefix, itemIndex);
        std::string itemDescription = Localized::getString(buff);
        
        if(itemDescription.compare(kLocalizedStringNotFound) != 0) {
            achievements.push_back({itemIndex, itemDescription});
            itemIndex++;
        } else  {
            continueLoop = false;
        }
        
    } while(continueLoop);
    
    currentAchievementIndex = UserDefault::getInstance()->getIntegerForKey(kCurrentAchievementIndexKey, 0);
    currentLevel = UserDefault::getInstance()->getIntegerForKey(kCurrentLevelKey, 0);
    currentCoins = UserDefault::getInstance()->getIntegerForKey(kCurrentCoinsKey, 0);
    
}

void GameConfig::save() {
    // use user defaults here
    UserDefault::getInstance()->setIntegerForKey(kCurrentAchievementIndexKey, currentAchievementIndex);
    UserDefault::getInstance()->setIntegerForKey(kCurrentLevelKey, currentLevel);
    UserDefault::getInstance()->setIntegerForKey(kCurrentCoinsKey, currentCoins);
    
}