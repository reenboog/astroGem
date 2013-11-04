//
//  ItemsUI.h
//  astroGems
//
//  Created by Alex Gievsky on 01.11.13.
//
//

#ifndef __astroGems__AchievementsUI__
#define __astroGems__AchievementsUI__

#include "StackableLayer.h"
#include "cocos2d.h"
#include "cocos-ext.h"

USING_NS_CC;
USING_NS_CC_EXT;

class GameScene;

class AchievementsUI: public Layer, public TableViewDataSource, public TableViewDelegate, public StackableLayer {
public:
    virtual ~AchievementsUI();
    AchievementsUI();
    // cocos2d stuff
    virtual bool init();
    CREATE_FUNC(AchievementsUI);

    // table delegates
    virtual void scrollViewDidScroll(cocos2d::extension::ScrollView* view) {};
    virtual void scrollViewDidZoom(cocos2d::extension::ScrollView* view) {}
    virtual void tableCellTouched(cocos2d::extension::TableView* table, cocos2d::extension::TableViewCell* cell);
    virtual cocos2d::Size tableCellSizeForIndex(cocos2d::extension::TableView *table, unsigned int idx);
    virtual cocos2d::extension::TableViewCell* tableCellAtIndex(cocos2d::extension::TableView *table, unsigned int idx);
    virtual unsigned int numberOfCellsInTableView(cocos2d::extension::TableView *table);

    void setGameLayer(GameScene *layer);
    
    // stackable layer stuff
    virtual void popUp(StackableLayer *baseLayer);
	virtual void popOut();
	virtual void putOn(StackableLayer *overlay);
	virtual void takeOff();
	virtual void disableTouches();
	virtual void enableTouches();
    
    void onEnter();
private:
    void populateCells();
    void onPlayBtnPressed();
private:
    TableView *tableView;
    Sprite *back;
    
    int tableWidth;
    int tableHeight;

    int itemWidth;
    int itemHeight;
    
    Point tableViewPos;
    Point backPos;
    Point playMenuPos;
    
    Menu *playMenu;
    
    GameScene *gameLayer;
};

class AchievementTableViewCell: public TableViewCell
{
public:
	AchievementTableViewCell(int itemId);
	virtual ~AchievementTableViewCell();
private:
	Sprite *background;
    Sprite *icon;
    LabelBMFont *caption;
};


#endif /* defined(__astroGems__ItemsUI__) */
