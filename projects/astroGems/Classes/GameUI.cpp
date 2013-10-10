//
//  GameUI.cpp
//  astroGems
//
//  Created by Alex Gievsky on 09.10.13.
//
//

#include "GameUI.h"
#include "GameScene.h"
#include "GameConfig.h"

GameUI::~GameUI() {
    
}

GameUI::GameUI(): Layer() {
    timeProgress = nullptr;
    multiplierProgress = nullptr;
    timeLabel = nullptr;
    pauseBtn = nullptr;
    scoreLabel = nullptr;
    
    gameLayer = nullptr;
}

#pragma mark - init

bool GameUI::init() {
    if(!Layer::init()) {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point screenCenter = {visibleSize.width / 2.0, visibleSize.height / 2.0};
    Point gridPos = GameConfig::sharedInstance()->gridPos;
    
    Sprite *grid = Sprite::create("field/grid.png");
    Size gridSize = grid->getContentSize();
    
    // multiplier progress
    Sprite *multiplierProgressMount = Sprite::create("multiplierProgressMount.png");
    multiplierProgressMount->setAnchorPoint({0.5, 0});
    multiplierProgressMount->setPosition({visibleSize.width / 2.0, gridPos.y + gridSize.height / 2.0});
    
    this->addChild(multiplierProgressMount);
    
    multiplierProgress = ProgressTimer::create(Sprite::create("multiplierProgress.png"));
    multiplierProgress->setType(ProgressTimer::Type::BAR);
    multiplierProgress->setAnchorPoint({0, 0.5});
    multiplierProgress->setPosition({0, multiplierProgressMount->getContentSize().height / 2.0});
    multiplierProgress->setMidpoint({0, 0});
    multiplierProgress->setBarChangeRate({1, 0});
    multiplierProgress->setPercentage(30);
    
    multiplierProgressMount->addChild(multiplierProgress);
    
    // time progress
    Sprite *timeProgressMount = Sprite::create("timeProgressMount.png");
    timeProgressMount->setAnchorPoint({0.5, 1});
    timeProgressMount->setPosition({visibleSize.width / 2.0, gridPos.y - gridSize.height / 2.0});
    
    this->addChild(timeProgressMount);
    
    timeProgress = ProgressTimer::create(Sprite::create("multiplierProgress.png"));
    timeProgress->setType(ProgressTimer::Type::BAR);
    timeProgress->setAnchorPoint({0, 0.5});
    timeProgress->setPosition({0, timeProgressMount->getContentSize().height / 2.0});
    timeProgress->setMidpoint({0, 0});
    timeProgress->setBarChangeRate({1, 0});
    timeProgress->setPercentage(30);
    
    timeProgressMount->addChild(timeProgress);
    
    // time label
    timeLabel = LabelBMFont::create("0", "time.fnt");
    timeLabel->setPosition({timeProgress->getContentSize().width / 2.0, timeProgress->getContentSize().height / 2.0});
    timeProgress->addChild(timeLabel);
    
    pauseBtn = MenuItemImage::create("pauseBtn.png", "pauseBtnOn.png", "pauseBtn.png", CC_CALLBACK_1(GameUI::onPauseBtnPressed, this));
    //pauseBtn->setAnchorPoint({0.5, 0.7});
    pauseBtn->setPosition({0, 0});
    
    Menu *pauseMenu = Menu::create(pauseBtn, NULL);
    pauseMenu->setPosition({pauseBtn->getContentSize().width * 0.8, visibleSize.height - pauseBtn->getContentSize().height * 0.8});
    
    this->addChild(pauseMenu);
    
    return true;
}

void GameUI::reset() {
}

#pragma mark - callbacks

void GameUI::onPauseBtnPressed(Object *sender) {
    
}

#pragma mark - setters

void GameUI::setGameLayer(GameScene *layer) {
    this->gameLayer = layer;
}

void GameUI::setScore(int value) {
    String scoreStr = "";
    
    scoreStr.appendWithFormat("%i", value);
    
//    scoreLabel->setString(scoreStr.getCString());
}

void GameUI::setTimeLeft(float seconds) {
    // format time here
    String timeStr = "";
    
    int min = seconds / 60;
    int sec = (int)seconds % 60;
    
    if(min < 10) {
        timeStr.appendWithFormat("0%i:", min);
    } else {
        timeStr.appendWithFormat("%i:", min);
    }
    
    if(sec < 10) {
        timeStr.appendWithFormat("0%i", sec);
    } else {
        timeStr.appendWithFormat("%i", sec);
    }
    
    timeLabel->setString(timeStr.getCString());
    
    timeProgress->setPercentage(100 * seconds / GameConfig::sharedInstance()->gameTimer);
}

void GameUI::setScoreMultiplier(float multiplier) {
    
}

void GameUI::setScoreMultiplierProgress(float progress) {
    multiplierProgress->setPercentage(100 * progress / kScoreMultiplierMaxProgress);
}