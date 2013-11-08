//
//  AchievementUnlockedUI.cpp
//  astroGems
//
//  Created by Alex Gievsky on 07.11.13.
//
//

#include "AchievementUnlockedUI.h"
#include "GameScene.h"
#include "Localized.h"
#include "GameConfig.h"

#pragma mark - init

AchievementUnlockedUI::~AchievementUnlockedUI() {
    
}

AchievementUnlockedUI::AchievementUnlockedUI() {
    shine = nullptr;
    achievement = nullptr;
    labelMount = nullptr;
    achievementLabel = nullptr;
    whiteShadow = nullptr;
    achievementFrame = nullptr;
}

bool AchievementUnlockedUI::init() {
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
    
    String *achievementFileName = String::createWithFormat("%s%i.png", kAchievementPrefix, 0);
    
    achievement = Sprite::createWithSpriteFrameName(achievementFileName->getCString());
    achievement->setPosition({visibleSize.width / 2, visibleSize.height / 2});
    achievement->setVisible(false);
    
    this->addChild(achievement);
    
    // frame
    achievementFrame = Sprite::create("ui/achievementFrame.png");
    achievementFrame->setPosition(achievement->getPosition());
    achievementFrame->setVisible(false);

    this->addChild(achievementFrame);
    
    // "LevelUp" label
    achievementLabel = LabelBMFont::create("", "achievementTableCell.fnt");
    //levelUpLabel->setAnchorPoint({1, 0.5});
    achievementLabel->setPosition({labelMount->getContentSize().width * 0.6, labelMount->getContentSize().height * 0.5});
    
    labelMount->addChild(achievementLabel);
    
    whiteShadow = LayerColor::create({255, 255, 255, 255});
    whiteShadow->setOpacity(0);
    this->addChild(whiteShadow, 2);
    
    return true;
}

#pragma mark - stackable stuff

void AchievementUnlockedUI::onEnter() {
    if(!this->isRunning()) {
        Layer::onEnter();
    }
}

void AchievementUnlockedUI::popUp(StackableLayer *baseLayer) {
    this->onEnter();
    
	this->disableTouches();
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
	CallFuncN *showAction = CallFuncN::create(CC_CALLBACK_0(AchievementUnlockedUI::enableTouches, this));
    
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
    
    int newAchievementIndex = (GameConfig::sharedInstance()->currentAchievementIndex + 1) % kNumberOfAchievements;
    String *achievementFileName = String::createWithFormat("%s%i.png", kAchievementPrefix, newAchievementIndex);
    
    float achievementScale = 0.8 * achievementFrame->getContentSize().width / achievement->getContentSize().width;
    
    achievementLabel->setString(Localized::getString(String::createWithFormat("%s%i", kAchievementPrefix, newAchievementIndex)->getCString()).c_str());
    
    achievement->setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(achievementFileName->getCString()));
    achievement->setVisible(true);
    achievement->setOpacity(0);
    achievement->setScale(1);
    achievement->setPosition({visibleSize.width / 2, visibleSize.height / 2});
    
    achievementFrame->setVisible(true);
    achievementFrame->setOpacity(0);
    achievementFrame->setScale(0.5);
    achievementFrame->setPosition(achievement->getPosition());
    
    Point starPos = achievement->getPosition();
    starPos = starPos - Point(labelMount->getContentSize().width / 2.4, 0);
    
    achievementFrame->runAction(Sequence::create(DelayTime::create(0.2),
                                                 Spawn::create(FadeIn::create(0.6),
                                                               EaseSineOut::create(ScaleTo::create(0.7, 1)), NULL),
                                                 DelayTime::create(1),
                                                 EaseSineOut::create(MoveTo::create(0.2, starPos)),
                                                 NULL));

    
    achievement->runAction(Sequence::create(DelayTime::create(0.2),
                                            Spawn::create(FadeIn::create(0.6),
                                                          EaseSineOut::create(ScaleTo::create(0.7, achievementScale)), NULL),
                                            DelayTime::create(1),
                                            EaseSineOut::create(MoveTo::create(0.2, starPos)),
                                            DelayTime::create(1),
                                            CallFunc::create([=]() {
                                                whiteShadow->runAction(Sequence::create(DelayTime::create(0.5),
                                                                                        FadeIn::create(0.5), NULL));
        
                                                shine->runAction(Spawn::create(ScaleTo::create(1, 5),
                                                                               RotateBy::create(1, 360),
                                                                               Sequence::create(DelayTime::create(1),
                                                                                                CallFunc::create([=]() {
                                                                                   gameLayer->applyNextBackground();
                                                                                   gameLayer->setCurrentAchievementIndex(newAchievementIndex);
                                                                               }), NULL), NULL));
                                            }),
                                            DelayTime::create(1.4),
                                            CallFunc::create([this]() {
                                                this->popOut();
                                            }), NULL));
    
    //
    labelMount->setVisible(true);
    labelMount->setScaleX(0);
    labelMount->setOpacity(255);
    labelMount->runAction(Sequence::create(DelayTime::create(2.1),
                                           ScaleTo::create(0.2, 1, 1),
                                           NULL));
    
    achievementLabel->setOpacity(255);
    
	this->baseLayer = baseLayer;
}

void AchievementUnlockedUI::popOut() {
	this->disableTouches();
    
	Sequence *hideAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(AchievementUnlockedUI::enableTouches, this)), // disable touches?
                                            CallFuncN::create(CC_CALLBACK_0(AchievementUnlockedUI::onExit, this)), NULL);
    
    
    achievement->setOpacity(0);
    achievementFrame->setOpacity(0);
    shine->setOpacity(0);
    labelMount->setOpacity(0);
    achievementLabel->setOpacity(0);
    
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

void AchievementUnlockedUI::putOn(StackableLayer *overlay) {
	this->disableTouches();
    
	FadeTo *hideEffect = FadeTo::create(kStackTransitionTime, kStackShadowOpacity);
	blackout->runAction(hideEffect);
    
	Sequence *hideAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(AchievementUnlockedUI::enableTouches, this)), // disable touches?
                                            CallFuncN::create(CC_CALLBACK_0(AchievementUnlockedUI::onExit, this)), NULL);
    
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), hideAction, NULL));
    
	overlay->popUp(this);
}

void AchievementUnlockedUI::takeOff() {
	if(!this->isRunning()) {
        this->onEnter();
    }
    
	FadeTo *showEffect = FadeTo::create(kStackTransitionTime, 0);
	blackout->runAction(showEffect);
    
	Sequence *showAction = Sequence::create(CallFuncN::create(CC_CALLBACK_0(AchievementUnlockedUI::enableTouches, this)),
                                            //CallFuncN::create(CC_CALLBACK_0(GameScene::updatePlayerPosition, this)),
                                            NULL);
	this->runAction(Sequence::create(DelayTime::create(kStackTransitionTime), showAction, NULL));
}

void AchievementUnlockedUI::disableTouches() {
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

void AchievementUnlockedUI::enableTouches() {
	if(touchBlocker) {
		touchBlocker->removeFromParentAndCleanup(true);
		touchBlocker = nullptr;
	}
}

#pragma mark - set/get

void AchievementUnlockedUI::setGameLayer(GameScene *layer) {
    this->gameLayer = layer;
}