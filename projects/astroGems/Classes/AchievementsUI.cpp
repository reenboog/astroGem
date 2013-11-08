//
//  ItemsUI.cpp
//  astroGems
//
//  Created by Alex Gievsky on 01.11.13.
//
//

#include "AchievementsUI.h"

#include "GameScene.h"
#include "GameConfig.h"
#include "Localized.h"

AchievementsUI::~AchievementsUI() {
    
}

AchievementsUI::AchievementsUI(): Layer() {
    tableView = nullptr;
    playMenu = nullptr;
    
    gameLayer = nullptr;
    back = nullptr;
    
    tableWidth = 0;
    tableHeight = 0;
    
    itemWidth = 0;
    itemHeight = 0;
}

#pragma mark - init

bool AchievementsUI::init() {
    if(!Layer::init()) {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point screenCenter = {visibleSize.width / 2.0, visibleSize.height / 2.0};
    Point gridPos = GameConfig::sharedInstance()->gridPos;
    
    // table view
    // get grid dimensions
    Sprite *grid = Sprite::create("field/grid.png");
    
    Sprite *item = Sprite::create("ui/achievementItemMount.png");
    itemHeight = item->getContentSize().height;
    itemWidth = item->getContentSize().width;
    
    tableHeight = grid->getContentSize().height * 1.175;
    tableWidth = itemWidth;
    
    tableViewPos = Point((visibleSize.width - item->getContentSize().width) * 0.5 , visibleSize.height - tableHeight);
    backPos = Point(visibleSize.width * 0.5, tableViewPos.y - 20);
    playMenuPos = Point(visibleSize.width / 2, tableViewPos.y);
    
    back = Sprite::create("ui/achievementTableBack.png");
    back->setAnchorPoint({0.5, 0});
    back->setPosition(backPos + Point(0, visibleSize.height));
    back->setOpacity(200);
    
    this->addChild(back);
    
    tableView = TableView::create(this, {tableWidth, tableHeight});
	tableView->setDirection(ScrollView::Direction::VERTICAL);
	tableView->setPosition(tableViewPos + Point(0, visibleSize.height));
	tableView->setDelegate(this);
    //tableView->setBounceable(true);
	tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);

    this->addChild(tableView);
    
    // play menu
    
    MenuItemImage *playBtn = MenuItemImage::create("ui/playBtn.png", "ui/playBtnOn.png", CC_CALLBACK_0(AchievementsUI::onPlayBtnPressed, this));
    playBtn->setAnchorPoint({0.5, 1.0});
    playBtn->setPosition({0, 0});
    
    playMenu = Menu::create(playBtn, NULL);
    playMenu->setPosition(playMenuPos + Point(0, -visibleSize.height));
    
    this->addChild(playMenu);
    
    this->populateCells();
    tableView->reloadData();
    
    blackout = LayerColor::create(Color4B(0, 0, 0, 0));
	blackout->setPosition({0, 0});
	blackout->setZOrder(zShadow);
    
	this->addChild(blackout);
    
    return true;
}

#pragma mark - table view delegate

void AchievementsUI::tableCellTouched(TableView* table, TableViewCell* cell) {
}

Size AchievementsUI::tableCellSizeForIndex(TableView *table, unsigned int idx) {
	return Size(itemWidth, itemHeight + kAchievementCellBorderHeight);
}

TableViewCell* AchievementsUI::tableCellAtIndex(TableView *table, unsigned int idx) {
	TableViewCell *cell = table->cellAtIndex(idx);
    if(!cell) {
        
		cell = new AchievementTableViewCell(idx);
        cell->autorelease();
    }
    
	return cell;
}

unsigned int AchievementsUI::numberOfCellsInTableView(TableView *table) {
	//int n = GlobalConfig::getInstance().getNumberOfEquipmentWithParameters();
	return GameConfig::sharedInstance()->achievements.size();
}


#pragma mark - 
void AchievementsUI::populateCells() {
    //
}

#pragma mark - ui callbacks

void AchievementsUI::onPlayBtnPressed() {
    this->popOut();
}

#pragma mark - stackable stuff

void AchievementsUI::onEnter() {
    if(!this->isRunning()) {
        Layer::onEnter();
    }
}

void AchievementsUI::popUp(StackableLayer *baseLayer) {
    tableView->reloadData();

    this->onEnter();
    
	this->disableTouches();
    
	CallFuncN *showAction = CallFuncN::create(CC_CALLBACK_0(AchievementsUI::enableTouches, this));
    
    tableView->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, tableViewPos)));
    back->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, backPos)));
    playMenu->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, playMenuPos)));
    
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), showAction, NULL));
    this->setZOrder(zActiveLayer);
    
	this->baseLayer = baseLayer;
}

void AchievementsUI::popOut() {
	this->disableTouches();
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    tableView->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, tableViewPos + Point(0, visibleSize.height))));
    back->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, backPos + Point(0, visibleSize.height))));
    playMenu->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, playMenuPos + Point(0, -visibleSize.height))));
	
	Sequence *hideAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(AchievementsUI::enableTouches, this)), // disable touches?
                                            CallFuncN::create(CC_CALLBACK_0(AchievementsUI::onExit, this)), NULL);
    
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), hideAction, NULL));
    
	if(baseLayer) {
		baseLayer->takeOff();
	}
    
	baseLayer = nullptr;
    
	this->setZOrder(zInactiveLayer);
}

void AchievementsUI::putOn(StackableLayer *overlay) {
	this->disableTouches();
    
	FadeTo *hideEffect = FadeTo::create(kStackTransitionTime, kStackShadowOpacity);
	blackout->runAction(hideEffect);
    
	Sequence *hideAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(AchievementsUI::enableTouches, this)), // disable touches?
                                            CallFuncN::create(CC_CALLBACK_0(AchievementsUI::onExit, this)), NULL);
    
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), hideAction, NULL));
    
	overlay->popUp(this);
}

void AchievementsUI::takeOff() {
	if(!this->isRunning()) {
        this->onEnter();
    }
    
	FadeTo *showEffect = FadeTo::create(kStackTransitionTime, 0);
	blackout->runAction(showEffect);
    
	Sequence *showAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(AchievementsUI::enableTouches, this)),
                                            //CallFuncN::create(CC_CALLBACK_0(GameScene::updatePlayerPosition, this)),
                                            NULL);
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), showAction, NULL));
}

void AchievementsUI::disableTouches() {
	Size visibleSize = Director::getInstance()->getVisibleSize();
    
	if(!touchBlocker) {
		MenuItemImage *touchBlockerButton = MenuItemImage::create("blankPixel.png", "blankPixel.png", "blankPixel.png");
        
        touchBlockerButton->setContentSize(visibleSize);
		touchBlockerButton->setPosition({0, 0});
		//touchBlockerButton->setAnchorPoint({0.5, 0.5});
		touchBlockerButton->setOpacity(0);
		touchBlockerButton->setZOrder(10);
        
		touchBlocker = Menu::createWithItem(touchBlockerButton);
		touchBlocker->setPosition(visibleSize.width / 2, visibleSize.height / 2);
        
		this->addChild(touchBlocker, 10);
        
		touchBlocker->setTouchPriority(-250);
	}
}

void AchievementsUI::enableTouches() {
	if(touchBlocker) {
		touchBlocker->removeFromParentAndCleanup(true);
		touchBlocker = nullptr;
	}
}


#pragma mark - set/get

void AchievementsUI::setGameLayer(GameScene *layer) {
    this->gameLayer = layer;
}

#pragma mark - Achievement cell

AchievementTableViewCell::~AchievementTableViewCell() {
    
}

AchievementTableViewCell::AchievementTableViewCell(int itemId) {
    //itemId = itemId - 1;
    
    String *fileName = String::createWithFormat("%s%i.png", kAchievementPrefix, itemId);
    
    background = Sprite::create("ui/achievementItemMount.png");
    background->setPosition(Point::ZERO);
	background->setAnchorPoint(Point::ZERO);
    
    this->addChild(background);
    
    icon = Sprite::createWithSpriteFrameName(fileName->getCString());
    icon->setPosition({background->getContentSize().width * 0.08, background->getContentSize().height * 0.5});
    
    background->addChild(icon);
    
    caption = LabelBMFont::create(GameConfig::sharedInstance()->achievements[itemId].description.c_str(), "achievementTableCell.fnt");
    caption->setAnchorPoint({0, 0.5});
    caption->setPosition({background->getContentSize().width * 0.05 + icon->getContentSize().width, background->getContentSize().height * 0.5});
    
    background->addChild(caption);
    
    if(GameConfig::sharedInstance()->currentAchievementIndex < itemId) {
        Sprite *shadow = Sprite::create("ui/achievementItemMount.png");
        shadow->setPosition({background->getContentSize().width / 2, background->getContentSize().height / 2});
        shadow->setOpacity(200);
        
        background->addChild(shadow);
    }
}