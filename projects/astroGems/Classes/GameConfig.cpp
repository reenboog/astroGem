//
//  GameConfig.cpp
//  1pGigTickets
//
//  Created by Alex Gievsky on 13.09.13.
//
//

#include "GameConfig.h"

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
    
    gameTimer = kgameTimer;
    quizTimer = kQuizTimer;
    qMultiplier = kQMultiplier;
    
    baseIconValue = kBaseIconValue;
    fourInRowIconValue = kFourInRowIconValue;
    noteIconValue = kNoteIconValue;
}

void GameConfig::save() {
    // use user defaults here
    
}