//
//  LevelUpUI.cpp
//  astroGems
//
//  Created by Alex Gievsky on 04.11.13.
//
//

#include "LevelUpUI.h"
#include "GameScene.h"
#include "Localized.h"
#include "GameConfig.h"

#pragma mark - init

LevelUpUI::~LevelUpUI() {
    
}

LevelUpUI::LevelUpUI() {
    shine = nullptr;
    star = nullptr;
    labelMount = nullptr;
    lvLabel = nullptr;
    levelUpLabel = nullptr;
    whiteShadow = nullptr;
}

bool LevelUpUI::init() {
    if(!Layer::init()) {
        return false;
    }
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    Point screenCenter = {visibleSize.width / 2.0, visibleSize.height / 2.0};
    
    // shine
    shine = Sprite::create("ui/achievementShine.png");
    shine->setPosition({visibleSize.width / 2, visibleSize.height / 2});
    shine->setVisible(false);
    
    this->addChild(shine);
    
    // label mount
    labelMount = Sprite::create("ui/achievementLabelMount.png");
    labelMount->setAnchorPoint({0, 0.5});
    labelMount->setPosition({visibleSize.width / 2 - labelMount->getContentSize().width / 2, visibleSize.height / 2});
    labelMount->setVisible(false);
    
    this->addChild(labelMount);
    
    // star
    
    star = Sprite::create("ui/levelUpStar.png");
    star->setPosition({visibleSize.width / 2, visibleSize.height / 2});
    star->setVisible(false);
    
    this->addChild(star);
    
    // "LevelUp" label
    levelUpLabel = LabelBMFont::create(Localized::getString("levelUp").c_str(), "achievementTableCell.fnt");
    //levelUpLabel->setAnchorPoint({1, 0.5});
    levelUpLabel->setPosition({labelMount->getContentSize().width * 0.6, labelMount->getContentSize().height * 0.5});
    
    labelMount->addChild(levelUpLabel);
    
    // lv label
    lvLabel = LabelBMFont::create("0", "levelUpLevel.fnt");
    lvLabel->setAnchorPoint({1, 0});
    lvLabel->setPosition({star->getContentSize().width * 0.5, star->getContentSize().height * 0.5});
    lvLabel->setVisible(false);
    
    star->addChild(lvLabel);
    
    whiteShadow = LayerColor::create({255, 255, 255, 255});
    whiteShadow->setOpacity(0);
    this->addChild(whiteShadow, 2);
    
    return true;
}

#pragma mark - stackable stuff

void LevelUpUI::onEnter() {
    if(!this->isRunning()) {
        Layer::onEnter();
    }
}

void LevelUpUI::popUp(StackableLayer *baseLayer) {
    this->onEnter();
    
	this->disableTouches();
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
	CallFuncN *showAction = CallFuncN::create(CC_CALLBACK_0(LevelUpUI::enableTouches, this));
    
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), showAction, NULL));
    this->setZOrder(zActiveLayer);
    
    whiteShadow->setOpacity(0);
    
    shine->setVisible(true);
    shine->setOpacity(0);
    shine->setScale(0.5);
    shine->setRotation(0);
    
    shine->runAction(Sequence::create(DelayTime::create(0.7),
                                      Spawn::create(ScaleTo::create(0.2, 1),
                                                    FadeIn::create(0.15),
                                                    NULL),
                                      RotateTo::create(15, 360 * 3),
                                      NULL));
    
    star->setVisible(true);
    star->setOpacity(0);
    star->setScale(0.5);
    star->setPosition({visibleSize.width / 2, visibleSize.height / 2});
    
    Point starPos = star->getPosition();
    starPos = starPos - Point(labelMount->getContentSize().width / 2.4, 0);
    
    star->runAction(Sequence::create(DelayTime::create(0.2),
                                     Spawn::create(FadeIn::create(0.6),
                                                   ScaleTo::create(0.7, 1),
                                                   EaseSineOut::create(RotateTo::create(0.8, 360 * 2)),
                                                   NULL),
                                     DelayTime::create(1),
                                     EaseSineOut::create(MoveTo::create(0.2, starPos)),
                                     NULL));
    
    labelMount->setVisible(true);
    labelMount->setScaleX(0);
    labelMount->setOpacity(255);
    labelMount->runAction(Sequence::create(DelayTime::create(2.1),
                                           ScaleTo::create(0.2, 1, 1),
                                           NULL));
    
    levelUpLabel->setOpacity(255);
    
    int newLevel = GameConfig::sharedInstance()->currentLevel + 1;
    // lv label
    String *lvString = String::createWithFormat("%i", newLevel);
    lvLabel->setString(lvString->getCString());
    lvLabel->setVisible(true);
    lvLabel->setOpacity(0);
    lvLabel->setPosition({star->getContentSize().width / 2 + lvLabel->getContentSize().width / 2, star->getContentSize().height / 2.08 - lvLabel->getContentSize().height / 2});
    
    Point levelPos = GameConfig::sharedInstance()->levelLabelGUIPos - starPos + Point(-lvLabel->getContentSize().width / 2, lvLabel->getContentSize().height / 2);
    
    // og gosh, forgive me for this (
    lvLabel->runAction(Sequence::create(DelayTime::create(2.4),
                                        Spawn::create(FadeIn::create(0.2),
                                                      EaseElasticOut::create(ScaleTo::create(0.2, 1)),
                                                      NULL),
                                        DelayTime::create(0.5),
                                        Spawn::create(EaseSineOut::create(MoveBy::create(0.5, levelPos)),
                                                      Sequence::create(DelayTime::create(0.4),
                                                                       CallFunc::create([=]() {
                                                                            gameLayer->setCurrentLevel(newLevel);
                                                                        }),
                                                                       FadeOut::create(0.25),
                                                                       NULL),
                                                      NULL),
                                        CallFunc::create([=]() {
                                            whiteShadow->runAction(Sequence::create(DelayTime::create(0.5),
                                                                                    FadeIn::create(0.5),
                                                                                    NULL));
        
                                            //shine->stopAllActions();
                                            shine->runAction(Spawn::create(ScaleTo::create(1, 5),
                                                                           RotateBy::create(1, 360),
                                                                           Sequence::create(DelayTime::create(1),
                                                                                            CallFunc::create([=]() {
                                                                                                gameLayer->applyNextBackground();
                                                                                            }),
                                                                                            NULL),
                                                                           NULL));
                                        }),
                                        DelayTime::create(1.4),
                                        CallFunc::create([this]() {
                                            this->popOut();
                                        }),
                                        NULL));
    
	this->baseLayer = baseLayer;
}

void LevelUpUI::popOut() {
	this->disableTouches();
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
//    tableView->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, tableViewPos + Point(0, visibleSize.height))));
//    back->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, backPos + Point(0, visibleSize.height))));
//    playMenu->runAction(EaseSineOut::create(MoveTo::create(kStackTransitionTime, playMenuPos + Point(0, -visibleSize.height))));
	
	Sequence *hideAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(LevelUpUI::enableTouches, this)), // disable touches?
                                            CallFuncN::create(CC_CALLBACK_0(LevelUpUI::onExit, this)), NULL);
    
    
    star->setOpacity(0);
    shine->setOpacity(0);
    labelMount->setOpacity(0);
    lvLabel->setOpacity(0);
    levelUpLabel->setOpacity(0);

    whiteShadow->runAction(Sequence::create(DelayTime::create(0.2),
                                            FadeOut::create(0.3),
                                            NULL));
    
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), hideAction, NULL));
    
	if(baseLayer) {
		baseLayer->takeOff();
	}
    
	baseLayer = nullptr;
    
	this->setZOrder(zInactiveLayer);
}

void LevelUpUI::putOn(StackableLayer *overlay) {
	this->disableTouches();
    
	FadeTo *hideEffect = FadeTo::create(kStackTransitionTime, kStackShadowOpacity);
	blackout->runAction(hideEffect);
    
	Sequence *hideAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(LevelUpUI::enableTouches, this)), // disable touches?
                                            CallFuncN::create(CC_CALLBACK_0(LevelUpUI::onExit, this)), NULL);
    
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), hideAction, NULL));
    
	overlay->popUp(this);
}

void LevelUpUI::takeOff() {
	if(!this->isRunning()) {
        this->onEnter();
    }
    
	FadeTo *showEffect = FadeTo::create(kStackTransitionTime, 0);
	blackout->runAction(showEffect);
    
	Sequence *showAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(LevelUpUI::enableTouches, this)),
                                            //CallFuncN::create(CC_CALLBACK_0(GameScene::updatePlayerPosition, this)),
                                            NULL);
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), showAction, NULL));
}

void LevelUpUI::disableTouches() {
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

void LevelUpUI::enableTouches() {
	if(touchBlocker) {
		touchBlocker->removeFromParentAndCleanup(true);
		touchBlocker = nullptr;
	}
}

#pragma mark - set/get

void LevelUpUI::setGameLayer(GameScene *layer) {
    this->gameLayer = layer;
}