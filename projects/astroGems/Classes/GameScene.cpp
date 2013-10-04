
#include "GameScene.h"
#include "GemField.h"
#include "Shared.h"

#pragma mark - cocos2d stuff

GameScene::~GameScene() {
}

GameScene::GameScene() {
    field = nullptr;
}

#pragma mark - init

Scene * GameScene::scene() {
    Scene *scene = Scene::create();

    GameScene *layer = GameScene::create();
    scene->addChild(layer);

    return scene;
}

bool GameScene::init() {
    if(!Layer::init()) {
        return false;
    }
    
    // add sprite sheets
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("gems/gems.plist");


//    Shared::loadAnimation("animations.plist", "gemDeath");
    //
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point origin = Director::getInstance()->getVisibleOrigin();

    back = Sprite::create("backgrounds/bgr04.jpg");
    back->setPosition(Point(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y));

    this->addChild(back, 0);
    
    Sprite *grid = Sprite::create("temporal/gems/png/grid.png");
    grid->setPosition({visibleSize.width / 2, visibleSize.height / 2});
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
    
    return true;
}

#pragma mark - field watcher delegate

void GameScene::onGemDestroyed(GemColour colour, int x, int y, int score) {
	// get is destroyed
}

void GameScene::onGemsMatched(int length, GemColour colour, int startX, int startY, int endX, int endY, int score) {
	// on match
}

void GameScene::onGemsToBeShuffled() {
	CCLOG("I'm shuffling");
}

void GameScene::onGemsFinishedMoving() {
	canTouch = true;
}

void GameScene::onMoveMade(bool legal) {
	if(!legal) {
		//
	}
}

void GameScene::onGemsStartedSwapping() {
	canTouch = false;
}

#pragma mark - update logic

void GameScene::update(float dt) {
    
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