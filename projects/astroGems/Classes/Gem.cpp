#include"Gem.h"

#include "cocos2d.h"
#include "ccMacros.h"
#include "Constants.h"

USING_NS_CC;

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
	int zOrder = kZOrderGem;
	if(colour == GC_Random) {
		colour =  static_cast<GemColour>((int) ((CCRANDOM_0_1() * kGemTypeAmount) + 1));
	}

    this->colour = colour;
	this->type = type;
	this->state = GS_Idle;

    setGemColour(colour);

	setPosition(convertCoordinatesToPixels(x,y));
	setZOrder(zOrder);

//	switch(type) {
//		case GT_Cross: setOpacity(125);
//				break;
//		case GT_LineHor: setFlipY(true);
//				break;
//		case GT_LineVer: setFlipX(true);
//				break;
//		default:
//				break;
//	}
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
    
    // Add bonus styling
//    switch(type) {
//        case GT_Cross: setOpacity(125);
//            break;
//        case GT_LineHor: setFlipY(true);
//            break;
//        case GT_LineVer: setFlipX(true);
//            break;
//        default:
//            break;
//    }
}

#pragma mark - bonuses

void Gem::transformIntoBonus(GemType type) {
	if(type != GT_Colour) {
		this->type = type;
		this->state = GS_Transforming;
        
		//applyBonusStyling();
        
		Action *enlarge = ScaleTo::create(kTransformationTime / 3.f, 1.5f);
		Action *restyle = nullptr;
        
		switch(type) {
			case GT_Cross:
				restyle = FadeTo::create(kTransformationTime / 3.f, 125);
				break;
            case GT_HypercubeMaker:
                setGemColour(GC_Hypercube);
                
                restyle = Sequence::create(ScaleTo::create(kTransformationTime / 6.f, 1, 0),
                                           RotateTo::create(0.2, 40),
                                           ScaleTo::create(kTransformationTime / 6.f, 1, 1),
                                           NULL);
                break;
			case GT_LineDestroyer:
				restyle = Sequence::create(ScaleTo::create(kTransformationTime / 6.f, 1, 0),
                                           RotateTo::create(0.2, 40),
                                           ScaleTo::create(kTransformationTime / 6.f, 1, 1),
                                           NULL);
				break;
//            case GT_LineHor:
//				restyle = Sequence::create(ScaleTo::create(kTransformationTime / 6.f, 1, 0),
//                                           FlipY::create(true),
//                                           ScaleTo::create(kTransformationTime / 6.f, 1, 1),
//                                           NULL);
//				break;
//			case GT_LineVer:
//				restyle = Sequence::create(ScaleTo::create(kTransformationTime / 6.f, 0, 1),
//                                           FlipX::create(true),
//                                           ScaleTo::create(kTransformationTime / 6.f, 1, 1),
//                                           NULL);
//				break;
			default:
				restyle = DelayTime::create(kTransformationTime / 3.f);
				break;
		}
        
		Action *shrink = ScaleTo::create(kTransformationTime / 3.f, 1);
		Action *endTransformation = CallFuncN::create(CC_CALLBACK_1(Gem::onTransformationEnd, this));
		Action *destruction = Sequence::create((FiniteTimeAction*) enlarge,
                                               (FiniteTimeAction*) restyle,
                                               (FiniteTimeAction*) shrink,
                                               (FiniteTimeAction*) endTransformation,
                                               NULL);
        runAction(destruction);
	} else {
		this->state = GS_Matched;
	}
}

void Gem::onTransformationEnd(Object *sender) {
	state = GS_Transformed;
    
    string animationName = "";
    
    switch(colour) {
		case GC_Red: animationName = "guitar"; break;
		case GC_Green: animationName = "keyboard"; break;
        case GC_Blue: animationName = "mic"; break;
		case GC_Purple: animationName = "plectrum"; break;
        case GC_Yellow: animationName = "mark"; break;
		case GC_Orange: animationName = "sax"; break;
        //case GC_Wild: animationName = "note"; break;
            
        default: CCLOG("default gem color in reset!");
	}
    
    //Animate *action = Animate::create(AnimationCache::getInstance()->animationByName(animationName.c_str()));
    //runAction(RepeatForever::create(action));
}

void Gem::applyBonusStyling() {
	// Add bonus styling
    switch(type) {
        case GT_Cross: setOpacity(125);
            break;
        case GT_LineHor: setFlipY(true);
            break;
        case GT_LineVer: setFlipX(true);
            break;
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
}

void Gem::deselect() {
	state = GS_Idle;
	setScale(1);
}

#pragma mark - swapping

void Gem::swapTo(int x, int y, bool goBack) {
	moveTo(x, y, kSwapTime, goBack);
}

void Gem::fallTo(int x, int y, int blocksToWait, int rowsToWait) {
	moveTo(x, y, kFallTime, false, blocksToWait, rowsToWait);
}

void Gem::moveTo(int x, int y, float time, bool goBack, int blocksToWait, int rowsToWait) {
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
	Action *endMove = CallFuncN::create( CC_CALLBACK_1(Gem::onMovementEnd, this));
	Action *movement;

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

GemType Gem::getType() {
	return type;
}

GemColour Gem::getGemColour() {
	return colour;
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
        //case GC_Wild: fileName = "rainbow.png"; break;
            
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

void Gem::destroy() {
	if(state != GS_Destroying) {
		Action *enlarge = ScaleTo::create(kDestructionTime / 3.f, 1.2f);
		Action *shrink = ScaleTo::create(kDestructionTime / 3.f * 2.f, 0);
		Action *endDestruction = CallFuncN::create(CC_CALLBACK_1(Gem::onDestructionEnd, this));
		Action *destruction = Sequence::create((FiniteTimeAction*) enlarge,
											   (FiniteTimeAction*) shrink,
											   (FiniteTimeAction*) endDestruction, NULL);
        
		state = GS_Destroying;
        
		runAction(destruction);
        
        if(this->getParent()) {
//            // apply destruction particle effect
            LabelTTF *label = LabelTTF::create("+30", "Trebuchet", 36);
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
	CCLOG("%f", getScale());
	CCLOG("%i", power);
	state = GS_Immovable;
	runAction(ScaleTo::create(kDestructionTime, scale, scale));
	setZOrder(kZOrderTile);
}

#pragma mark - internals

Point Gem::convertCoordinatesToPixels(int x, int y) {
	return Point(kTileSize * (x + 0.5), kTileSize * (kFieldHeight - 1 - y + 0.5));  
}

/*
void Gem::makeBomb(GemType type) {
	this->type = type;
	
	String fileName = "bonus";
	switch (type)
	{
	case GT_Cross: fileName.append("Cross");
		break;
	case GT_LineHor: fileName.append("Horizontal");
		break;
	case GT_LineVer: fileName.append("Vertical");
		break;
	default:
		break;
	}
	fileName.append(".png");
	initWithFile(fileName.getCString());
}
*/