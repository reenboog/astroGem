
#include "GameScene.h"
#include "GemField.h"
#include "Shared.h"
#include "GameUI.h"
#include "GameConfig.h"
#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

#pragma mark - cocos2d stuff

GameScene::~GameScene() {
}

GameScene::GameScene() {
    field = nullptr;
    back = nullptr;
    
    currentScore = 0;
    currentRainbowGems = 0;
    //timeLeft = GameConfig::sharedInstance()->gameTimer;
    
    //gameOver = true;
}

#pragma mark - init/reset

Scene * GameScene::scene() {
    Scene *scene = Scene::create();

    GameScene *gameLayer = GameScene::create();
    scene->addChild(gameLayer);
    
    GameUI *ui = GameUI::create();
    scene->addChild(ui);
    
    gameLayer->setUI(ui);
    ui->setGameLayer(gameLayer);
    
    
    // start should be here instead
    gameLayer->reset();
    
    return scene;
}

bool GameScene::init() {
    if(!Layer::init()) {
        return false;
    }
    
    // add sprite sheets
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("gems/gems.plist");
    
    // preload effects and music
    SimpleAudioEngine::getInstance()->preloadEffect("match.wav");
    SimpleAudioEngine::getInstance()->preloadEffect("wrongMove.wav");
    SimpleAudioEngine::getInstance()->preloadEffect("laser.wav");
    //
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    back = Sprite::create("backgrounds/bgr04.jpg");
    back->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    this->addChild(back, 0);
    
    Sprite *grid = Sprite::create("field/grid.png");
    grid->setPosition({visibleSize.width / 2, visibleSize.height / 2});
    
    GameConfig::sharedInstance()->gridPos = grid->getPosition();
    
    this->addChild(grid);
    
    field = GemField::create();
    field->addWatcher(this);
    
    this->addChild(field);
    
    float posX = (visibleSize.width - kTileSize * (kFieldWidth)) / 2;
	float posY = (visibleSize.height - kTileSize * (kFieldHeight)) / 2;
    
    field->setPosition(posX, posY);
    
    // prepare for touches
    canTouch = false;
    swipeEnded = false;

    firstTouchLocation = Point(-1, -1);

    Director::getInstance()->getTouchDispatcher()->addTargetedDelegate(this, 0, true);
    
    scheduleUpdate();
    
    return true;
}

void GameScene::reset() {
    currentScore = 0;
    //timeLeft = GameConfig::sharedInstance()->gameTimer;
    
    //gameOver = false;
    
    currentRainbowGems = 0;
    
    scoreMultiplier.rewind();
    
    ui->reset();
    
    ui->setScore(currentScore);
    ui->setRainbowGemsProgress(0);
    ui->setCoins(GameConfig::sharedInstance()->currentCoins);
    ui->setMakeFunBtnEnabled(false);
    //ui->setScoreMultiplier(scoreMultiplier);
    ui->setScoreMultiplierProgress(0);
    //ui->setTimeLeft(timeLeft);
}

#pragma mark - field watcher delegate

void GameScene::onGemDestroyed(GemColour colour, int x, int y, int score) {
    applyScoreMultiplierProgress(1);
    
    if(colour == GC_Coin) {
        this->applyCoins(1, x, y);
    }
    
    setScore(currentScore + score * scoreMultiplier.multiplier);
}

void GameScene::onGemsMatched(int length, GemColour colour, int startX, int startY, int endX, int endY, int score) {
    applyScoreMultiplierProgress(length);

    setScore(currentScore + score * scoreMultiplier.multiplier);
}

void GameScene::onGemsToBeShuffled() {
	CCLOG("I'm shuffling");
}

void GameScene::onGemsFinishedMoving() {
	canTouch = true;
    
    ui->setMakeFunBtnEnabled(true);
}

void GameScene::onMoveMade(bool legal) {
	if(!legal) {
        applyScoreMultiplierProgress(-6);
    }
}

void GameScene::onStartedResolvinMatches(const MatchList &founMatches) {
    float pitch = 1;

    if(scoreMultiplier.state == ScoreMultiplier::SMS_Normal) {
        //pitch = clampf(1 + scoreMultiplier.currentProgress / kScoreMultiplierMaxProgress - 0.34, 1, 1.3);
    } else if(scoreMultiplier.multiplier == kScoreExtraMultiplier) {
        pitch = 1.2;
    }
    
    // play some sound
    // if length == 3
    
    //SimpleAudioEngine::getInstance()->playEffect("match.wav", false, pitch, 0, 1);
}

void GameScene::onRainbowGemDestroyed(int x, int y) {
    currentRainbowGems++;
    
    if(currentRainbowGems >= GameConfig::sharedInstance()->rainbowGemsRequiredForAchievement) {
        currentRainbowGems = GameConfig::sharedInstance()->rainbowGemsRequiredForAchievement;
        //GameConfig::sharedInstance()->rainbowGemsRequiredForAchievement++;
        
        // show all the magic here;
    }
    
    // apply visual effect here
    Sprite *rainbow = Sprite::createWithSpriteFrameName("rainbow.png");
    rainbow->setPosition(this->convertFieldCoordinatesToWorldLocation({x, y}));
    
    this->addChild(rainbow);
    
    rainbow->runAction(Sequence::create(Spawn::create(MoveBy::create(kRainbowGemDestructionEffectTime, {0, -kTileSize}),
                                                      FadeOut::create(kRainbowGemDestructionEffectTime * 0.9),
                                                      NULL),
                                        CallFunc::create([=]() {
                                            ui->setRainbowGemsProgress(100 * (currentRainbowGems / GameConfig::sharedInstance()->rainbowGemsRequiredForAchievement));
                                            field->setState(FS_Destroying);
        
                                            rainbow->removeFromParent();
                                        }),
                                        NULL));
}

void GameScene::onGemsStartedSwapping() {
	canTouch = false;
    
    ui->setMakeFunBtnEnabled(false);
}

#pragma mark - update logic

void GameScene::update(float dt) {
//    if(gameOver) {
//        return;
//    }
    
    switch(scoreMultiplier.state) {
        case ScoreMultiplier::SMS_Active:
            scoreMultiplier.activityTimer -= dt;
            if(scoreMultiplier.activityTimer <= 0) {
                scoreMultiplier.activityTimer = 0;
                scoreMultiplier.fadeOut();
            }
            break;
        case ScoreMultiplier::SMS_FadingOut:
            this->applyScoreMultiplierProgress(-dt * kScoreMultiplierFadeOutSpeed);
            break;
        case ScoreMultiplier::SMS_Normal:
            this->applyScoreMultiplierProgress(-dt * kScoreMultiplierFadeInSpeed);
            break;
    }
    
//    timeLeft -= dt;
//    setTimeLeft(timeLeft);
}

#pragma mark - callbacks

void GameScene::onMakeFunBtnPressed() {
    field->destroyEntireField();
}

#pragma mark - setters/getters

void GameScene::setUI(GameUI *ui) {
    this->ui = ui;
}

void GameScene::applyScoreMultiplierProgress(float progress) {
    switch(scoreMultiplier.state) {
        case ScoreMultiplier::SMS_Active:
            break;
        case ScoreMultiplier::SMS_FadingOut:
            if(progress < 0) {
                scoreMultiplier.currentProgress += progress;
                if(scoreMultiplier.currentProgress <= 0) {
                    scoreMultiplier.currentProgress = 0;
                    scoreMultiplier.rewind();
                }
            }
            
            this->setScoreMultiplierProgress(scoreMultiplier.currentProgress);
            break;
        case ScoreMultiplier::SMS_Normal:
            scoreMultiplier.currentProgress += progress;
            
            if(scoreMultiplier.currentProgress >= kScoreMultiplierMaxProgress) {
                scoreMultiplier.currentProgress = kScoreMultiplierMaxProgress;
                scoreMultiplier.apply();
            } else if(scoreMultiplier.currentProgress < 0) {
                scoreMultiplier.currentProgress = 0;
            }
            
            this->setScoreMultiplierProgress(scoreMultiplier.currentProgress);
            break;
    }
}

void GameScene::setScoreMultiplierProgress(float progress) {
    scoreMultiplier.currentProgress = progress;
    ui->setScoreMultiplierProgress(100 * (scoreMultiplier.currentProgress / kScoreMultiplierMaxProgress));
}

//void GameScene::setTimeLeft(float time) {
//    if(timeLeft <= 0) {
//        timeLeft = 0;
//        // game over
//        gameOver = true;
//    }
//    
//    ui->setTimeLeft(time);
//}

void GameScene::setScore(int score) {
    currentScore = score;
    
    ui->setScore(currentScore);
}

void GameScene::applyCoins(int coins, int fromX, int fromY) {    
    GameConfig::sharedInstance()->currentCoins += coins;
    
    Point pos = this->convertFieldCoordinatesToWorldLocation({fromX, fromY});
    
    ui->setCoins(GameConfig::sharedInstance()->currentCoins, pos.x, pos.y);
}

#pragma mark - touches

bool GameScene::ccTouchBegan(Touch *touch, Event *event) {
    Point location = touch->getLocation();
    location = this->convertToNodeSpace(location);
    
	firstTouchLocation = location;
    
	swipeEnded = false;
	return true;
}

void GameScene::ccTouchMoved(Touch *touch, Event *event) {
	Point touchLocation = convertTouchToNodeSpace(touch);

	if(!swipeEnded && canTouch && firstTouchLocation.getDistance(touchLocation) > (kTileSize / 2)) {
		swipeEnded = true;
        
		float differenceX = touchLocation.x - firstTouchLocation.x;
		float differenceY = touchLocation.y - firstTouchLocation.y;
        
		Direction direction;
		if(fabs(differenceX) >= fabs(differenceY)) {
			if(differenceX >= 0) {
				direction = D_Right;
			} else {
				direction = D_Left;
			}
		} else {
			if(differenceY >= 0) {
				direction = D_Up;
			} else {
				direction = D_Down;
			}
		}
		//field->makeMove(convertToCoordinates(firstTouchLocation), direction);
        field->swipeAction(convertLocationToFieldCoordinates(firstTouchLocation), direction);
	}
}

void GameScene::ccTouchEnded(Touch *touch, Event *event) {
    Point location = touch->getLocation();
    location = this->convertToNodeSpace(location);

	if(!swipeEnded && canTouch) {
		field->clickAction(convertLocationToFieldCoordinates(location));
	}
}

#pragma mark -

Point GameScene::convertLocationToFieldCoordinates(Point point) {
	return Point((int) ((point.x - field->getPositionX()) / kTileSize), (int) (kFieldHeight - (point.y - field->getPositionY()) / kTileSize));
}

Point GameScene::convertFieldCoordinatesToWorldLocation(const Point &point) {
    return Point(point.x * kTileSize + field->getPositionX() + kTileSize / 2.0,
                 (kFieldHeight - point.y) * kTileSize + field->getPositionY() - kTileSize / 2.0);
}