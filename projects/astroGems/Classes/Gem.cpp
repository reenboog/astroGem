#include"Gem.h"

#include "cocos2d.h"
#include "ccMacros.h"
#include "Constants.h"

#include "GameConfig.h"

USING_NS_CC;

//temporal constants

#define kGemBonusCloudBackSpriteTag 1000
#define kGemBonusCloudFrontSpriteTag 1001
#define kGemBonusShine1SpriteTag 1002
#define kGemBonusShine2SpriteTag 1003
#define kGemSelectionTag 1004

using std::string;

#pragma mark - init

Gem::Gem(GemColour colour, GemType type) {
	// Generate a random gem type if it is not specified
	if(colour == GC_Random) {
		colour =  static_cast<GemColour>((int) ((CCRANDOM_0_1() * kGemTypeAmount) + 1));
	}
	
    this->colour = colour;
	this->type = type;
	this->state = GS_Idle;
}

Gem::~Gem() {
	CCLOG("Gem destructor");
}

void Gem::init(int x, int y, GemColour colour, GemType type) {
	// Get the file name for the gem image
	string fileName = "";
	int zOrder = zGem;
	if(colour == GC_Random) {
		colour =  static_cast<GemColour>((int) ((CCRANDOM_0_1() * kGemTypeAmount) + 1));
	}

    this->colour = colour;
	this->type = type;
	this->state = GS_Idle;

    setGemColour(colour);

	setPosition(convertCoordinatesToPixels(x,y));
	setZOrder(zOrder);
}

void Gem::reset(int x, int y, GemColour colour, GemType type) {
	// Set parameters
	if(colour == GC_Random) {
		colour =  static_cast<GemColour>((int) ((CCRANDOM_0_1() * kGemTypeAmount) + 1));
	}
	this->colour = colour;
	this->type = type;
	this->state = GS_Idle;
    
    this->stopAllActions();
    
    setGemColour(colour);
    
    setPosition(convertCoordinatesToPixels(x,y));
    
    setFlipX(false);
    setFlipY(false);
    setScale(1.0);
    setRotation(0);
    setOpacity(255);
    
    // remove temporal bonus sprites if any
//    this->removeChildByTag(kGemBonusCloudBackSpriteTag, true);
//    this->removeChildByTag(kGemBonusCloudFrontSpriteTag, true);
//    this->removeChildByTag(kGemBonusShine1SpriteTag, true);
//    this->removeChildByTag(kGemBonusShine2SpriteTag, true);
    this->removeAllChildren();
}

#pragma mark - bonuses

void Gem::transformIntoBonus(GemType type, float delay, GemState completionState) {
    if(state == GS_Transforming) {
        return;
    }
    
	if(type != GT_Colour) {
		this->type = type;
		this->state = GS_Transforming;
        
		Action *enlarge = ScaleTo::create(kTransformationTime / 3.f, 1.5f);
        
		switch(type) {
			case GT_Cross:
				break;
            case GT_Hypercube:
                setGemColour(GC_Hypercube);
                break;
			case GT_LineHor:
			case GT_LineVer:
            case GT_LineDestroyer:
			default:
				break;
		}
        
        Action *endTransformation = nullptr;
        
        switch(completionState) {
            case GS_AboutToExplodeByHypercube:
                endTransformation = CallFunc::create(CC_CALLBACK_0(Gem::prepareToExplodeByHypercube, this));
                break;
            case GS_Transformed:
            default:
                endTransformation = CallFuncN::create(CC_CALLBACK_1(Gem::onTransformationEnd, this));
                break;
        }
        
		Action *shrink = ScaleTo::create(kTransformationTime / 3.f, 1);
        Action *animation = CallFunc::create(CC_CALLBACK_0(Gem::applyBonusStyling, this));
		Action *destruction = Sequence::create(DelayTime::create(delay),
                                               (FiniteTimeAction*) enlarge,
                                               (FiniteTimeAction*) shrink,
                                               (FiniteTimeAction*) animation,
                                               (FiniteTimeAction*) endTransformation,
                                               NULL);
        runAction(destruction);
	} else {
		this->state = GS_Matched;
	}
}

void Gem::onTransformationEnd(Object *sender) {
	state = GS_Transformed;
}

void Gem::applyBonusStyling() {
	// Add bonus styling
    switch(type) {
        case GT_Cross: {
            Point pos = {this->getContentSize().width / 2.0f, this->getContentSize().height / 2.0f};

            Sprite *backCloud = Sprite::createWithSpriteFrameName("gemCloudBack.png");
            backCloud->runAction(RepeatForever::create(RotateBy::create(1.0, -36.0f)));
            backCloud->setPosition(pos);
            backCloud->setOpacity(50);
            backCloud->setTag(kGemBonusCloudBackSpriteTag);
            
            this->addChild(backCloud);
            this->reorderChild(backCloud, -1);
            
            Sprite *front = Sprite::createWithSpriteFrameName("gemCloudFront.png");
            front->setPosition(pos);
            front->setTag(kGemBonusCloudFrontSpriteTag);
            
            Sprite *innerFrontCloud = Sprite::createWithSpriteFrameName("gemCloudFront.png");
            innerFrontCloud->setAnchorPoint({0.5, 0.5});
            innerFrontCloud->setPosition(pos);
            innerFrontCloud->setOpacity(0);
            innerFrontCloud->setScale(0);
            
            front->addChild(innerFrontCloud);
            
            innerFrontCloud->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.3),
                                                                              Spawn::create(FadeIn::create(0.5),
                                                                                            ScaleTo::create(0.5, 1.0), NULL),
                                                                              Spawn::create(FadeOut::create(0.2), NULL),
                                                                              ScaleTo::create(0.0001, 0.0),
                                                                              NULL)));
            
            this->addChild(front);
            this->reorderChild(front, 1);
        } break;
        case GT_LineHor:
        case GT_LineVer:
        case GT_LineDestroyer: {
            
            Point pos = {this->getContentSize().width / 2.0f, this->getContentSize().height / 2.0f};
            
            Sprite *backCloud = Sprite::createWithSpriteFrameName("gemCloudBack.png");
            backCloud->runAction(RepeatForever::create(RotateBy::create(1.0, 36.0f)));
            backCloud->setPosition(pos);
            backCloud->setOpacity(80);
            backCloud->setTag(kGemBonusCloudBackSpriteTag);
            
            this->addChild(backCloud);
            this->reorderChild(backCloud, -1);
            
            pos = Point(this->getContentSize().width * 0.4f, this->getContentSize().height * 0.6f);
            
            Sprite *shine1 = Sprite::createWithSpriteFrameName("shine.png");
            shine1->runAction(RepeatForever::create(RotateBy::create(1.0, -36.0f)));
            shine1->setPosition(pos);
            shine1->setScale(0.6);
            shine1->setOpacity(100);
            shine1->setTag(kGemBonusShine1SpriteTag);
            
            this->addChild(shine1, 1);
            
            Sprite *shine2 = Sprite::createWithSpriteFrameName("shine.png");
            shine2->runAction(RepeatForever::create(Spawn::create(RotateBy::create(1.0, -10.0f),
                                                                  Sequence::create(FadeOut::create(2),
                                                                                   DelayTime::create(1),
                                                                                   FadeIn::create(1), NULL), NULL)));
            shine2->setPosition(pos);
            shine2->setOpacity(100);
            shine2->setScale(0.4);
            shine2->setTag(kGemBonusShine2SpriteTag);
            
            this->addChild(shine2, 2);

        } break;
        case GT_Hypercube: {
            this->runAction(RepeatForever::create(RotateBy::create(1, -40)));
            
            Point pos = Point(this->getContentSize().width / 2.0f, this->getContentSize().height / 2.0f);
            
//            Sprite *shine1 = Sprite::createWithSpriteFrameName("shine.png");
//            shine1->runAction(RepeatForever::create(RotateBy::create(1.0, -36.0f)));
//            shine1->setPosition(pos);
//            shine1->setScale(0.6);
//            shine1->setOpacity(100);
//            shine1->setTag(kGemBonusShine1SpriteTag);
//            
//            this->addChild(shine1, 1);
//            
//            Sprite *shine2 = Sprite::createWithSpriteFrameName("shine.png");
//            shine2->runAction(RepeatForever::create(Spawn::create(RotateBy::create(1.0, -10.0f),
//                                                                  Sequence::create(FadeOut::create(2),
//                                                                                   DelayTime::create(1),
//                                                                                   FadeIn::create(1), NULL), NULL)));
//            shine2->setPosition(pos);
//            shine2->setOpacity(100);
//            shine2->setScale(0.4);
//            shine2->setTag(kGemBonusShine2SpriteTag);
//            
//            this->addChild(shine2, 2);
            
            Sprite *backCloud = Sprite::createWithSpriteFrameName("gemCloudBack.png");
            backCloud->runAction(RepeatForever::create(RotateBy::create(1.0, -40.0f)));
            backCloud->setPosition(pos);
            backCloud->setOpacity(80);
            backCloud->setScale(0.6);
            backCloud->setTag(kGemBonusCloudBackSpriteTag);
            
            this->addChild(backCloud);
            this->reorderChild(backCloud, -1);

        } break;
        default:
            break;
    }
}

#pragma mark - movement/selection

void Gem::onMovementEnd(Object *sender) {
	state = GS_Moved;
}

void Gem::select() {
	state = GS_Selected;
	setScale(1.2f);
    
//    Point pos = {this->getContentSize().width / 2.0, this->getContentSize().height / 2.0};
//    
//    Sprite *selection = Sprite::createWithSpriteFrameName("selection.png");
//    this->addChild(selection);
//
//    selection->setScale(2);
//    selection->setOpacity(0);
//    selection->setPosition(pos);
//    selection->setTag(kGemSelectionTag);
//    
//    selection->runAction(Spawn::create(FadeIn::create(0.2),
//                                       ScaleTo::create(0.12, 1),
//                                       NULL));
}

void Gem::deselect() {
	state = GS_Idle;
	setScale(1);
    
//    Node *selection = this->getChildByTag(kGemSelectionTag);
//    
//    if(selection) {
//        selection->runAction(Sequence::create(FadeOut::create(0.1),
//                                              CallFunc::create([=](){
//                                                    selection->removeFromParent();
//                                            }), NULL));
//    }
}

#pragma mark - swapping

void Gem::swapTo(int x, int y, bool goBack, GemState completionState) {
	moveTo(x, y, kSwapTime, goBack, 0, 0, completionState);
}

void Gem::fallTo(int x, int y, int blocksToWait, int rowsToWait) {
	moveTo(x, y, kFallTime, false, blocksToWait, rowsToWait);
}

void Gem::moveTo(int x, int y, float time, bool goBack, int blocksToWait, int rowsToWait, GemState completionState) {
    
    if(state == GS_Moving) {
        return;
    }
    
	Point newLocation = convertCoordinatesToPixels(x, y);
	
    Action *wait = DelayTime::create(blocksToWait * kFallTime * kColumnsFallDelay + rowsToWait * kFallTime * kRowsFallDelay);
	Action *move = nullptr;
    
    if(blocksToWait >= 1) {
		Action *moveDown = MoveTo::create((this->getPosition().getDistance(newLocation) / kTileSize) * time * 0.7f,
                                          newLocation + Point(0, kTileSize * 0.1f));
        
		Action *moveUp = MoveTo::create((this->getPosition().getDistance(newLocation) / kTileSize) * time * 0.2f,
                                        newLocation - Point(0, kTileSize * 0.1f));
        
		Action *moveDownAgain = MoveTo::create((this->getPosition().getDistance(newLocation) / kTileSize) * time * 0.1f,
                                               newLocation);
        
		move = Sequence::create((FiniteTimeAction*) moveDown, (FiniteTimeAction*) moveUp, (FiniteTimeAction*) moveDownAgain, NULL);
	} else {
		move = MoveTo::create((this->getPosition().getDistance(newLocation) / kTileSize) * time, newLocation);
	}
    
	Action *moveBack = MoveTo::create((this->getPosition().getDistance(newLocation) / kTileSize) * time, this->getPosition());
	Action *movement = nullptr;
    
    Action *endMove = nullptr;
    
    switch(completionState) {
        case GS_AboutToDestroyByHypercube:
            endMove = CallFunc::create(CC_CALLBACK_0(Gem::prepareToBeDestroyedByHypercube, this));
            break;
        case GS_AboutToTurnIntoBomb:
            endMove = CallFunc::create(CC_CALLBACK_0(Gem::prepareToTurnIntoBombByHypercube, this));
            break;
        case GS_AboutToExplodeWithCross:
            endMove = CallFunc::create(CC_CALLBACK_0(Gem::prepareToTurnIntoCrossExplosion, this));
            break;
        case GS_Moved:
        default:
            endMove = CallFuncN::create(CC_CALLBACK_1(Gem::onMovementEnd, this));
    }
    
	if(goBack) {
		movement = Sequence::create((FiniteTimeAction*) wait, (FiniteTimeAction*) move, (FiniteTimeAction*) moveBack, (FiniteTimeAction*) endMove, NULL);
	} else {
		movement = Sequence::create((FiniteTimeAction*) wait, (FiniteTimeAction*) move, (FiniteTimeAction*) endMove, NULL);
	}
    
	state = GS_Moving;
	runAction(movement);
}

#pragma mark -

void Gem::match(MatchType matchType) {
	state = GS_Matched;
    
    if(type == GT_LineDestroyer && matchType != MT_None) {
        switch(matchType) {
            case MT_Vertical:
                type = GT_LineVer; break;
            case MT_Horizontal:
                type = GT_LineHor; break;
            default: type = GT_Colour;
        }
    }
}

#pragma mark - set/get

GemState Gem::getState() {
	return state;
}

void Gem::setState(GemState state) {
    this->state = state;
}

GemType Gem::getType() {
	return type;
}

void Gem::setType(GemType type) {
    this->type = type;
}

GemColour Gem::getGemColour() {
	return colour;
}

#pragma mark - bonus helpers

void Gem::prepareToBeDestroyedByHypercube() {
    state = GS_AboutToDestroyByHypercube;
}

void Gem::prepareToTurnIntoBombByHypercube() {
    state = GS_AboutToTurnIntoBomb;
}

void Gem::prepareToTurnIntoCrossExplosion() {
    state = GS_AboutToExplodeWithCross;
}

void Gem::prepareToExplodeByHypercube() {
    state = GS_AboutToExplodeByHypercube;
}

void Gem::setGemColour(GemColour color) {
    string fileName = "";
    
    switch(color) {
        case GC_Red: fileName = "red.png"; break;;
		case GC_Green: fileName = "green.png"; break;
        case GC_Blue: fileName = "blue.png"; break;
		case GC_Purple: fileName = "purple.png"; break;
        case GC_Yellow: fileName = "yellow.png"; break;
		case GC_Orange: fileName = "orange.png"; break;
        case GC_White: fileName = "white.png"; break;
        case GC_Hypercube: fileName = "hyperCube.png"; break;
            
        default: CCLOG("default gem color in reset!");
	}
    if(getTexture() == nullptr) {
        initWithSpriteFrameName(fileName.c_str());
    } else {
        setDisplayFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(fileName.c_str()));
    }
    
    if(color != GC_Hypercube) {
        this->colour = color;
    }
}

void Gem::resetState() {
	state = GS_Idle;
}

#pragma mark - destruction

void Gem::onDestructionEnd(Object *sender) {
	state = GS_Destroyed;    
}

void Gem::destroy(float delay) {
	if(state != GS_Destroying) {
        
        auto showScores = [this]() {
            if(this->getParent()) {
                String scoreStr;
                scoreStr.appendWithFormat("%i", Gem::scoreForGem(getType()));
                LabelTTF *label = LabelTTF::create(scoreStr.getCString(), "Trebuchet", 36);
                label->setPosition(this->getPosition());
                label->setOpacity(0);
                label->setScale(0.5);
                
                FiniteTimeAction *destructionAnim = nullptr;
                
                destructionAnim = Sequence::create(Spawn::create(ScaleTo::create(0.2, 1.0f),
                                                                 FadeIn::create(0.33),
                                                                 MoveBy::create(0.5, {0, 100}),
                                                                 NULL),
                                                   Spawn::create(FadeOut::create(0.14),
                                                                 ScaleTo::create(0.24, 0.5), NULL), NULL);
                
                auto cleanFogUp = CallFunc::create([=](){
                    label->removeFromParent();
                });
                
                label->runAction(Sequence::create(destructionAnim, cleanFogUp, NULL));
                
                // we're definetly sure that the gem has a parent
                Node *parent = this->getParent();
                parent->addChild(label, this->getZOrder());
            }

        };
        
        Action *wait = DelayTime::create(delay);
		Action *enlarge = ScaleTo::create(kDestructionTime / 3.f, 1.2f);
		Action *shrink = ScaleTo::create(kDestructionTime / 3.f * 2.f, 0);
		Action *endDestruction = CallFuncN::create(CC_CALLBACK_1(Gem::onDestructionEnd, this));
		Action *destructionSequence = Sequence::create((FiniteTimeAction*) wait,
                                               (FiniteTimeAction*) enlarge,
											   (FiniteTimeAction*) shrink,
											   (FiniteTimeAction*) endDestruction, NULL);
        
        Action *showScoreAction = CallFunc::create(showScores);
        
        Action *destruction = Spawn::create((FiniteTimeAction*) destructionSequence,
                                            Sequence::create(DelayTime::create(kDestructionTime / 20.0),
                                                            (FiniteTimeAction*) showScoreAction,
                                                             NULL),
                                            NULL);
        
		state = GS_Destroying;
        
		runAction(destruction);
        
        if(this->getParent()) {
//            LabelTTF *label = LabelTTF::create("+30", "Trebuchet", 36);
//            label->setPosition(this->getPosition());
//            label->setOpacity(0);
//            label->setScale(0.5);
//            
//            FiniteTimeAction *destructionAnim = nullptr;
//            
//            destructionAnim = Sequence::create(Spawn::create(ScaleTo::create(0.2, 1.0f),
//                                                             FadeIn::create(0.33),
//                                                             MoveBy::create(0.5, {0, 100}),
//                                                             NULL),
//                                               Spawn::create(FadeOut::create(0.14),
//                                                             ScaleTo::create(0.24, 0.5), NULL), NULL);
//            
//            auto cleanFogUp = CallFunc::create([=](){
//                label->removeFromParent();
//            });
//            
//            label->runAction(Sequence::create(destructionAnim, cleanFogUp, NULL));
//            
//            // we're definetly sure that the gem has a parent
//            Node *parent = this->getParent();
//            parent->addChild(label, this->getZOrder());

        }
	}
}

void Gem::remove() {
	state = GS_Destroyed;
}

void Gem::setFreeze(int power) {
	float scale;
	if(power == 0)
		scale = 1.f;
	if(power == 1)
		scale = 0.8f;
	if(power == 2)
		scale = 0.6f;
	if(power == 3)
		scale = 0.4f;

	state = GS_Immovable;
	runAction(ScaleTo::create(kDestructionTime, scale, scale));
	//setZOrder(zTile);
}

#pragma mark - helpers

Point Gem::convertCoordinatesToPixels(int x, int y) {
	return Point(kTileSize * (x + 0.5), kTileSize * (kFieldHeight - 1 - y + 0.5));
}

int Gem::scoreForGem(GemType type) {
    int score = 0;
    
    switch(type) {
        case GT_Explosion:
        case GT_LineDestroyer:
        case GT_LineHor:
        case GT_LineVer:
        case GT_Cross:
        case GT_RectDestroyer:
            score = GameConfig::sharedInstance()->fourInRowIconValue;
            break;
        case GT_Hypercube:
            score = GameConfig::sharedInstance()->hypercubeIconValue;
            break;
        case GT_Colour:
            score = GameConfig::sharedInstance()->baseIconValue;
            break;

    }
    
    return score;
}