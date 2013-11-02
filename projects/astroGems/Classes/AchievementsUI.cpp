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
    
    LayerColor *shadow = LayerColor::create({0, 0, 0, 150});
    this->addChild(shadow);
    
    // table view
    // get grid dimensions
    Sprite *grid = Sprite::create("field/grid.png");
    
    Sprite *item = Sprite::create("ui/achievementItemMount.png");
    itemHeight = item->getContentSize().height;
    itemWidth = item->getContentSize().width;
    
    tableHeight = grid->getContentSize().height * 1.175;
    tableWidth = itemWidth;;
    
    Point tableViewPos = {(visibleSize.width - itemWidth) * 0.5 , visibleSize.height - tableHeight};
    
    tableView = TableView::create(this, {tableWidth, tableHeight});
	tableView->setDirection(ScrollView::Direction::VERTICAL);
    //tableView->setAnchorPoint({0.5, 1});
	tableView->setPosition(tableViewPos);
	tableView->setDelegate(this);
    //tableView->setBounceable(true);
	tableView->setVerticalFillOrder(TableView::VerticalFillOrder::TOP_DOWN);

    this->addChild(tableView);
    
    tableView->reloadData();
    
    // play menu
    
    MenuItemImage *playBtn = MenuItemImage::create("ui/playBtn.png", "ui/playBtnOn.png", CC_CALLBACK_0(AchievementsUI::onPlayBtnPressed, this));
    playBtn->setAnchorPoint({0.5, 1.0});
    playBtn->setPosition({0, 0});
    
    playMenu = Menu::create(playBtn, NULL);
    playMenu->setPosition({visibleSize.width / 2, tableViewPos.y});
    
    this->addChild(playMenu);
    
    this->populateCells();
    
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

#pragma mark - Achievement cell

AchievementTableViewCell::~AchievementTableViewCell() {
    
}

AchievementTableViewCell::AchievementTableViewCell(int itemId) {
    //itemId = itemId - 1;
    
    String *fileName = String::createWithFormat("achievements/%s%i.png", kAchievementPrefix, itemId);
    
    background = Sprite::create("ui/achievementItemMount.png");
    background->setPosition(Point::ZERO);
	background->setAnchorPoint(Point::ZERO);
    
    this->addChild(background);
    
    icon = Sprite::create(fileName->getCString());
    icon->setPosition({background->getContentSize().width * 0.1, background->getContentSize().height * 0.5});
    
    background->addChild(icon);
    
    caption = LabelBMFont::create(GameConfig::sharedInstance()->achievements[itemId].description.c_str(), "time.fnt");
    caption->setAnchorPoint({0, 0.5});
    caption->setPosition({background->getContentSize().width * 0.2 + icon->getContentSize().width, background->getContentSize().height * 0.5});
    
    background->addChild(caption);
}

#pragma mark - ui callbacks

void AchievementsUI::onPlayBtnPressed() {
    //
}

#pragma mark - set/get

void AchievementsUI::setGameLayer(GameScene *layer) {
    this->gameLayer = layer;
}