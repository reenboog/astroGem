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
    //timeProgress = nullptr;
    rainbowGemsProgress = nullptr;
    multiplierProgress = nullptr;
    //timeLabel = nullptr;
    pauseBtn = nullptr;
    scoreLabel = nullptr;
    levelLabel = nullptr;
    coinsLabel = nullptr;
    coinsMenu = nullptr;
    coinsBtn = nullptr;
    
    makeFunMenu = nullptr;
    makeFunBtn = nullptr;
    makeFunLabel = nullptr;

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
//    Sprite *timeProgressMount = Sprite::create("timeProgressMount.png");
//    timeProgressMount->setAnchorPoint({0.5, 1});
//    timeProgressMount->setPosition({visibleSize.width / 2.0, gridPos.y - gridSize.height / 2.0});
//    
//    this->addChild(timeProgressMount);
//    
//    timeProgress = ProgressTimer::create(Sprite::create("multiplierProgress.png"));
//    timeProgress->setType(ProgressTimer::Type::BAR);
//    timeProgress->setAnchorPoint({0, 0.5});
//    timeProgress->setPosition({0, timeProgressMount->getContentSize().height / 2.0});
//    timeProgress->setMidpoint({0, 0});
//    timeProgress->setBarChangeRate({1, 0});
//    timeProgress->setPercentage(30);
//    
//    timeProgressMount->addChild(timeProgress);
    
    Sprite *rainbowGemsProgressMount = Sprite::create("timeProgressMount.png");
    rainbowGemsProgressMount->setAnchorPoint({0.5, 1});
    rainbowGemsProgressMount->setPosition({visibleSize.width / 2.0, gridPos.y - gridSize.height / 2.0});
    
    this->addChild(rainbowGemsProgressMount);
    
    rainbowGemsProgress = ProgressTimer::create(Sprite::create("multiplierProgress.png"));
    rainbowGemsProgress->setType(ProgressTimer::Type::BAR);
    rainbowGemsProgress->setAnchorPoint({0, 0.5});
    rainbowGemsProgress->setPosition({0, rainbowGemsProgressMount->getContentSize().height / 2.0});
    rainbowGemsProgress->setMidpoint({0, 0});
    rainbowGemsProgress->setBarChangeRate({1, 0});
    rainbowGemsProgress->setPercentage(0);
    
    rainbowGemsProgressMount->addChild(rainbowGemsProgress);
    
    // time label
//    timeLabel = LabelBMFont::create("0", "time.fnt");
//    timeLabel->setPosition({timeProgress->getContentSize().width / 2.0, timeProgress->getContentSize().height / 2.0});
//    timeProgress->addChild(timeLabel);
    
    pauseBtn = MenuItemImage::create("pauseBtn.png", "pauseBtnOn.png", "pauseBtn.png", CC_CALLBACK_1(GameUI::onPauseBtnPressed, this));
    //pauseBtn->setAnchorPoint({0.5, 0.7});
    pauseBtn->setPosition({0, 0});
    
    Menu *pauseMenu = Menu::create(pauseBtn, NULL);
    pauseMenu->setPosition({pauseBtn->getContentSize().width * 0.8, visibleSize.height - pauseBtn->getContentSize().height * 0.8});
    
    this->addChild(pauseMenu);
    
    // level
    levelLabel = LabelBMFont::create("12", "time.fnt");
    levelLabel->setPosition({visibleSize.width * 0.98, gridPos.y + gridSize.height / 2.0 + multiplierProgressMount->getContentSize().height});
    levelLabel->setAnchorPoint({1.0f, 0.0f});
    
    this->addChild(levelLabel);
    
    // score
    
    scoreLabel = LabelBMFont::create("110", "time.fnt");
    scoreLabel->setPosition({visibleSize.width * 0.02, gridPos.y + gridSize.height / 2.0 + multiplierProgressMount->getContentSize().height});
    scoreLabel->setAnchorPoint({0.0f, 0.0f});
    
    this->addChild(scoreLabel);
    
    // coins menu
    coinsBtn = MenuItemImage::create("ui/coinsBtn.png", "ui/coinsBtnOn.png", CC_CALLBACK_0(GameUI::onCoinsBtnPressed, this));
    coinsBtn->setAnchorPoint({0.0f, 1.0f});
    //coinsBtn->setAnchorPoint({0.0f, 0.5f});
    coinsBtn->setPosition({0, 0});
    
    coinsMenu = Menu::create(coinsBtn, NULL);
    coinsMenu->setPosition({visibleSize.width * 0.02, gridPos.y - gridSize.height / 2 - rainbowGemsProgressMount->getContentSize().height});
    //coinsMenu->setPosition({visibleSize.width * 0.02, (visibleSize.height / 2 - (gridSize.height / 2 + rainbowGemsProgressMount->getContentSize().height)) * 0.5});
    
    this->addChild(coinsMenu);
    
    // coins label
    coinsLabel = LabelBMFont::create("999", "time.fnt");
    coinsLabel->setPosition({coinsBtn->getContentSize().width * 0.7, coinsBtn->getContentSize().height * 0.5});
    coinsLabel->setAnchorPoint({1.0f, 0.5f});
    coinsLabel->setScale(0.7);
    
    coinsBtn->addChild(coinsLabel);
    
    // make fun btn
    makeFunBtn = MenuItemImage::create("ui/makeFunBtn.png", "ui/makeFunBtnOn.png", CC_CALLBACK_0(GameUI::onMakeFunBtnPressed, this));
    makeFunBtn->setAnchorPoint({1.0f, 1.0f});
    makeFunBtn->setPosition({0, 0});
    
    makeFunMenu = Menu::create(makeFunBtn, NULL);
    makeFunMenu->setPosition({visibleSize.width * 0.98, gridPos.y - gridSize.height / 2 - rainbowGemsProgressMount->getContentSize().height});
    
    this->addChild(makeFunMenu);
    
    // makeFun label; add locals here
    makeFunLabel = LabelBMFont::create("Make fun", "time.fnt");
    makeFunLabel->setPosition({makeFunBtn->getContentSize().width * 0.5, makeFunBtn->getContentSize().height * 0.5});
    makeFunLabel->setScale(0.7);
    
    makeFunBtn->addChild(makeFunLabel);

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
    
    scoreLabel->setString(scoreStr.getCString());
}

void GameUI::setLevel(int value) {
    String levelStr = "";
    
    levelStr.appendWithFormat("%i", value);
    
    levelLabel->setString(levelStr.getCString());
}


void GameUI::setCoins(int coins, int fromX, int fromY) {
    
    
    Point pos = {fromX, fromY};
    Point endPos = coinsMenu->getPosition() + Point(coinsBtn->getContentSize().width * 0.2, -coinsBtn->getContentSize().height * 0.5);
    
    Sprite *coin = Sprite::createWithSpriteFrameName("coin.png");
    coin->setPosition(pos);
    
    Point pt1 = {fromX + 100, fromX + 100};
    Point pt2 = (pt1 + endPos) * 0.5;
    
    ccBezierConfig config = {endPos, pt1, pt2};
    
    MotionStreak *streak = MotionStreak::create(0.2, 2, 25, {255, 255, 255}, "gems/lightningSquare.png");
    //streak->setColor(lightningColor);
    streak->setPosition(pos);
    
    streak->runAction(EaseSineInOut::create(BezierTo::create(0.7, config)));
    
    this->addChild(streak);
    this->addChild(coin);
    
    coin->runAction(Sequence::create(Spawn::create(EaseSineInOut::create(BezierTo::create(0.7, config)),
                                                   Sequence::create(DelayTime::create(0.5),
                                                                    FadeOut::create(0.2),
                                                                    NULL),
                                                   NULL),
                                     CallFunc::create([=]() {
                                            this->setCoins(coins);
                                            coin->removeFromParent();
                                            streak->removeFromParent();
                                     }),
                                     NULL));
}

void GameUI::setCoins(int coins) {
    String coinsStr = "";
    coinsStr.appendWithFormat("%i", coins);
    coinsLabel->setString(coinsStr.getCString());
}

//void GameUI::setTimeLeft(float seconds) {
//    // format time here
//    String timeStr = "";
//    
//    int min = seconds / 60;
//    int sec = (int)seconds % 60;
//    
//    if(min < 10) {
//        timeStr.appendWithFormat("0%i:", min);
//    } else {
//        timeStr.appendWithFormat("%i:", min);
//    }
//    
//    if(sec < 10) {
//        timeStr.appendWithFormat("0%i", sec);
//    } else {
//        timeStr.appendWithFormat("%i", sec);
//    }
//    
//    timeLabel->setString(timeStr.getCString());
//    
//    timeProgress->setPercentage(100 * seconds / GameConfig::sharedInstance()->gameTimer);
//}

//void GameUI::setScoreMultiplier(float multiplier) {
//    
//}

void GameUI::setScoreMultiplierProgress(float progress) {
    multiplierProgress->stopAllActions();
    multiplierProgress->runAction(ProgressTo::create(0.2, progress));
}

void GameUI::setRainbowGemsProgress(float progress) {
    rainbowGemsProgress->stopAllActions();
    
    rainbowGemsProgress->runAction(ProgressTo::create(0.2, progress));
}

void GameUI::onCoinsBtnPressed() {
    
}

void GameUI::onMakeFunBtnPressed() {
    gameLayer->onMakeFunBtnPressed();
    this->setMakeFunBtnEnabled(false);
}

void GameUI::setMakeFunBtnEnabled(bool enabled) {
    makeFunBtn->setEnabled(enabled);
}