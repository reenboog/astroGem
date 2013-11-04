#include "GemField.h"
#include "ccMacros.h"
#include <random>

#include "Gem.h"
#include "GameConfig.h"

#include "SimpleAudioEngine.h"

using namespace CocosDenshion;

using std::string;

GemField::~GemField() {
	CCLOG("GemField destructor");
    
	SafeDelete(selectedGemCoordinates);
}

GemField::GemField() {
    selectedGemCoordinates = nullptr;
}

#pragma mark - init

bool GemField::init() {
    if(!Node::init()) {
        return false;
    }
    
    std::srand(time(NULL));
    
	const int mask[kFieldHeight][kFieldWidth] = {
		{1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
		{1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1},
        {1,1,1,1,1,1,1,1}
	};
    
	std::memcpy(fieldMask, mask, sizeof(mask));
    
	const int freezers[kFieldHeight][kFieldWidth] = {
		{0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0},
        {0,0,0,0,0,0,0,0}
	};
    
	std::memcpy(freezeMask, freezers, sizeof(freezers));
    
	for(int y = 0; y < kFieldHeight; y++) {
		for(int x = 0; x < kFieldWidth; x++) {
			gems[y][x] = nullptr;
		}
	}
    
	// Generate gems on appropriate tiles
	bool gotMatches = true;
    
	while(gotMatches) {
		for(int y = 0; y < kFieldHeight; y++) {
			for(int x = 0; x < kFieldWidth; x++) {
				if(fieldMask[y][x] == 1) {
					if(gems[y][x] != nullptr) {
						gems[y][x]->reset(x, y);
					} else {
                        Gem *gem = new Gem();
                        gem->init(x, y);
                        
                        gems[y][x] = gem;
                    }
				} else {
					gems[y][x] = nullptr;
				}
			}
		}
		gotMatches = hasAnyMatches();
	}
	
	for(int y = 0; y < kFieldHeight; y++) {
		for(int x = 0; x < kFieldWidth; x++) {
			if(fieldMask[y][x] == 1) {
				this->addChild(gems[y][x]);
                gems[y][x]->autorelease();
			} else {
			}
		}
	}
    
    state = FS_Ready;
    
   	if(kPreloadField) {
		const int customField[kFieldHeight][kFieldWidth] = {
			{2,3,1,2,1,4,2,3},
            {1,2,1,2,1,4,2,3},
			{3,4,2,3,2,2,3,2},
			{4,1,1,4,1,3,2,3},
			{1,2,3,2,1,2,3,4},
			{3,4,3,2,2,3,4,2},
			{1,2,2,3,1,2,1,3},
            {4,1,3,2,2,1,3,1},
            {4,2,3,2,1,3,3,2}
		};
        
		const int customFieldType[kFieldHeight][kFieldWidth] = {
			{0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
			{0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0},
            {0,0,0,0,0,0,0,0}
		};
        
		for(int y = 0; y < kFieldHeight; y++) {
			for(int x = 0; x < kFieldWidth; x++) {
				if(fieldMask[y][x] == 1) {
					gems[y][x]->reset(x, y, (GemColour)customField[y][x], (GemType)customFieldType[y][x]);
				}
			}
		}
	}
    
    scheduleUpdate();
    
    return true;
}

#pragma mark - matching stuff

// Returns true if there is at least one match on the field
bool GemField::hasAnyMatches() {
	MatchList foundMatches = findMatches();
	return !foundMatches.empty();
}

MatchList GemField::findMatches() {
	MatchList foundMatches;
	for(int y = 0; y < kFieldHeight; y++) {
		foundMatches.splice(foundMatches.begin(), findMatchesInLine(0, y, kFieldWidth - 1, y));
	}
	for(int x = 0; x < kFieldWidth; x++) {
		foundMatches.splice(foundMatches.begin(), findMatchesInLine(x, 0, x, kFieldHeight - 1));
	}
	return foundMatches;
}

MatchList GemField::findMatchesInLine(int fromX, int fromY, int toX, int toY) {
	MatchList foundMatches;
	
	int stepX = 1;
	int stepY = 1;
    
	if(fromX == toX) {
		stepX = 0;
	}
	if(fromY == toY) {
		stepY = 0;
	}
    
	int x = fromX;
	int y = fromY;
    
	while(fieldMask[y][x] == 0) {
		x += stepX;
		y += stepY;
	}
    
	if(x > toX || y > toY) {
		return foundMatches;
	}
    
	GemColour currentColor = gems[y][x]->getGemColour();
    GemType currentType = gems[y][x]->getType();
    GemState currentState = gems[y][x]->getState();
	int chainLength = 1;
    
	x += stepX;
	y += stepY;
    
	while(x <= toX && y <= toY) {
		while((x <= toX && y <= toY) && fieldMask[y][x] == 1 && freezeMask[y][x] <= 1 && (gems[y][x]->getGemColour() == currentColor) &&
              !(gems[y][x]->getType() == GT_Hypercube && gems[y][x]->getState() != GS_Transformed) &&
              !(currentType == GT_Hypercube && currentState != GS_Transformed) &&
                currentType != GT_RainbowMaker && currentColor != GC_Rainbow &&
                gems[y][x]->getType() != GT_RainbowMaker && gems[y][x]->getGemColour() != GC_Rainbow &&
                currentColor != GC_Coin && gems[y][x]->getGemColour() != GC_Coin) {
			x += stepX;
			y += stepY;
			chainLength++;
		}
		if(chainLength >= 3) {
			if(fromX == toX) {
				foundMatches.push_front(Match(x, y - chainLength, x, y - 1, gems[y - 1][x]->getGemColour()));
			} else {
				foundMatches.push_front(Match(x - chainLength, y, x - 1, y, gems[y][x - 1]->getGemColour()));
			}
		}
        
		if(x > toX || y > toY) {
			return foundMatches;
		}
        
		while(fieldMask[y][x] == 0) {
			x += stepX;
			y += stepY;
		}
        
		if(x > toX || y > toY) {
			return foundMatches;
		}
        
		currentColor = gems[y][x]->getGemColour();
        currentType = gems[y][x]->getType();
        currentState = gems[y][x]->getState();

		chainLength = 1;
		x += stepX;
		y += stepY;
	}
    
	return foundMatches;
}

#pragma mark -

// Finds and resolves all matches
void GemField::resolveMatches() {
	MatchList foundMatches = findMatches();
    
    for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
		(*it)->onStartedResolvinMatches(foundMatches);
	}
    
	// First resolve all the matches
	for(MatchList::iterator it = foundMatches.begin(); it != foundMatches.end(); it++) {
		resolveMatch(*it);
	}
}

void GemField::resolveMatch(const Match &match) {
	bool bonusWasAdded = false;
	bool createBonusNext = false;
    
    // get match points
    int score = 0;
    
    for(int i = match.fromY; i <= match.toY; ++i) {
        for(int j = match.fromX; j <= match.toX; ++j) {
            score += Gem::scoreForGem(gems[i][j]->getType());
        }
    }
    
	for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
		(*it)->onGemsMatched(match.length, match.colour, match.fromX, match.fromY, match.toX, match.toY, score);
	}
    
	int stepX = 1;
	int stepY = 1;
    
	if(match.fromX == match.toX) {
		stepX = 0;
	} else {
		stepY = 0;
	}
    
	int x = match.fromX;
	int y = match.fromY;
    
	// Bonus types for each kind of match are defined at the bottom of Config.h
	while(y <= match.toY && x <= match.toX) {
		// If the gem is not already matched or transforming into a bonus
		if(gems[y][x]->getState() != GS_Matched && gems[y][x]->getState() != GS_Transforming) {
			// If we have a long enough match with no bonus added yet and the gem was recently moved
			if(!bonusWasAdded && match.length > 3 && gems[y][x]->getState() == GS_Moved) {
                
                SimpleAudioEngine::getInstance()->playEffect("bonusCreation.wav");
				// If it's a bonus, process it as usually, but turn the next gem into a bonus
				if(gems[y][x]->getType() != GT_Colour) {
					createBonusNext = true;
					gems[y][x]->match((MatchType)stepY);
				} else {
                    // If it's a regular gem, turn it into a bonus
					if(stepX == 0) {
						// Transform into bonus for vertical >3 in a row
						if(match.length == 4) {
							gems[y][x]->transformIntoBonus(kVerticalMatchFourBonus);
						} else if(match.length >= 5) {
							gems[y][x]->transformIntoBonus(kVerticalMatchFiveBonus);
						}
					} else {
						// Transform into bonus for horizontal >3 in a row
						if(match.length == 4) {
							gems[y][x]->transformIntoBonus(kHorizontalMatchFourBonus);
						} else if(match.length >= 5) {
                            gems[y][x]->transformIntoBonus(kHorizontalMatchFiveBonus);
						}
					}
					bonusWasAdded = true;
				}
			} else {
                // If the gem wasn't moved, but the one before it was a bonus one, turn it into a bonus
				if(createBonusNext) {
                    SimpleAudioEngine::getInstance()->playEffect("bonusCreation.wav");
                    
					if(stepX == 0) {
						// Transform into bonus for vertical >3 in a row
						if(match.length == 4) {
							gems[y][x]->transformIntoBonus(kVerticalMatchFourBonus);
						}
						if(match.length >= 5) {
							gems[y][x]->transformIntoBonus(kVerticalMatchFiveBonus);
						}
					} else {
						// Transform into bonus for horizontal >3 in a row
						if(match.length == 4) {
							gems[y][x]->transformIntoBonus(kHorizontalMatchFourBonus);
						}
						if(match.length >= 5) {
							gems[y][x]->transformIntoBonus(kHorizontalMatchFiveBonus);
						}
					}
					createBonusNext = false;
					bonusWasAdded = true;
				} else {
                    // Otherwise just mark it as matched as usually
					gems[y][x]->match((MatchType)stepY);
				}
			}
		} else {
            if(gems[y][x]->getType() != GT_Colour) {
                createBonusNext = true;
                gems[y][x]->match();
            } else {
                gems[y][x]->transformIntoBonus(kCrossMatchBonus);
                bonusWasAdded = true;
            }
		}
		y += stepY;
		x += stepX;
	}
}

#pragma mark - gem destruction and reset

void GemField::destroyMatchedGems() {
	MatchList foundMatches = findMatches();
    
	for(MatchList::iterator it = foundMatches.begin(); it != foundMatches.end(); it++) {
		destroyLine((*it).fromX, (*it).fromY, (*it).toX, (*it).toY, false);
	}
}

// Destroys a line of gems (should work for diagonal ones as well)
void GemField::destroyLine(int fromX, int fromY, int toX, int toY, bool destroyTransformed, float delay) {
	int stepX = 1;
	int stepY = 1;
	if(fromX == toX) {
		stepX = 0;
	}
	if(fromY == toY) {
		stepY = 0;
	}
	
    int y = fromY;
	int x = fromX;
    int i = 0;
	while(y <= toY && x <= toX) {
		if(!destroyTransformed && gems[y][x]->getState() == GS_Transformed) {
			//CCLOG("Fledgling @ %i, %i", x, y);
		} else {
			destroyGem(x, y, delay + i * delay / 20.0f);
            i++;
		}
		y += stepY;
		x += stepX;
	}
}

void GemField::destroyGem(int x, int y, float delay) {
	if(fieldMask[y][x] == 1 && gems[y][x]->getType() != GT_RainbowMaker && gems[y][x]->getGemColour() != GC_Rainbow) {
        GemState gemState = gems[y][x]->getState();
        
		if(gemState != GS_Destroying && gemState != GS_AboutToExplodeByHypercube) {
			if(freezeMask[y][x] > 1) {
				freezeGem(y, x, freezeMask[y][x] - 1);
			} else {
				if(freezeMask[y][x] == 1) {
					freezeGem(y, x, freezeMask[y][x] - 1);
				}
				if(gems[y][x]->getType() != GT_Colour && gems[y][x]->getType() != GT_Hypercube) {
                    if(gems[y][x]->getType() == GT_LineDestroyer) {
                        gems[y][x]->setType((GemType)(GT_LineHor + (GemType)(rand() % 2)));
                    }
                    
                    auto applyLightningAtPosWithXYDirectionsDelayAndColor = [=](const Point &pos, int xDir, int yDir, float delay, Color3B lightningColor) {
                        int squaresCount = MAX(kFieldHeight, kFieldWidth) * 2;
                        
                        Point currentIndices = {pos.x / kTileSize, kFieldHeight - pos.y / kTileSize};
                        
                        auto placeSprite = [this](const Point &index, float delay, Color3B color) {
                            if(index.x >= kFieldWidth || index.x < 0 || index.y >= kFieldHeight || index.y < 0) {
                                return;
                            }
                            
                            Sprite *lightning = Sprite::createWithSpriteFrameName("lightningSquare.png");
                            
                            lightning->setOpacity(0);
                            lightning->setColor(color);
                            
                            this->addChild(lightning, zGem - 1);
                            
                            Point pos = Gem::convertCoordinatesToPixels(index.x, index.y);
                            
                            lightning->setPosition(pos);
                            lightning->runAction(Sequence::create(DelayTime::create(delay),
                                                                  FadeTo::create(0.1, 150),
                                                                  DelayTime::create(0.16),
                                                                  CallFunc::create([=](){
                                lightning->removeFromParent();
                            }), NULL));

                        };
                        
                        for(int i = 0; i < squaresCount; ++i) {
                            placeSprite({currentIndices.x + xDir * i, currentIndices.y + yDir * i}, delay + 0.03 * i, lightningColor);
                            placeSprite({currentIndices.x - xDir * i, currentIndices.y - yDir * i}, delay + 0.03 * i, lightningColor);
                        }
                    };
                    
                    Point lightningPos = Gem::convertCoordinatesToPixels(x, y);
                    
                    Color3B lightningColor;
                    
                    switch(gems[y][x]->getGemColour()) {
                        case GC_Red:
                            lightningColor = {255, 50, 50};
                            break;
                        case GC_Green:
                            lightningColor = {50, 255, 50};
                            break;
                        case GC_Blue:
                            lightningColor = {50, 50, 255};
                            break;
                        case GC_Purple:
                            lightningColor = {255, 50, 255};
                            break;
                        case GC_Orange:
                            lightningColor = {255, 150, 0};
                            break;
                        case GC_Yellow:
                            lightningColor = {255, 255, 0};
                            break;
                        case GC_White:
                        case GC_Hypercube:
                        case GC_Random:
                            lightningColor = {255, 255, 255};
                        default:
                            break;
                    }
                    
					switch(gems[y][x]->getType()) {
                        case GT_Cross: {
                            gems[y][x]->destroy(delay);
                            
                            SimpleAudioEngine::getInstance()->playEffect("laser.wav");
                            
                            destroyLine(x, 0, x, kFieldHeight - 1, true, delay);
                            destroyLine(0, y, kFieldWidth - 1, y, true, delay);
                            
                            applyLightningAtPosWithXYDirectionsDelayAndColor(lightningPos, 1, 0, delay, lightningColor);
                            applyLightningAtPosWithXYDirectionsDelayAndColor(lightningPos, 0, 1, delay, lightningColor);
                        } break;
                        case GT_LineHor:
                            gems[y][x]->destroy(delay);
                            
                            //SimpleAudioEngine::getInstance()->playEffect("laser.wav");
                            
                            destroyLine(0, y, kFieldWidth - 1, y, true, delay);
                            
                            applyLightningAtPosWithXYDirectionsDelayAndColor(lightningPos, 1, 0, delay, lightningColor);
                            break;
                        case GT_LineVer:
                            gems[y][x]->destroy(delay);
                            
                            //SimpleAudioEngine::getInstance()->playEffect("laser.wav");
                            
                            destroyLine(x, 0, x, kFieldHeight - 1, true, delay);

                            applyLightningAtPosWithXYDirectionsDelayAndColor(lightningPos, 0, 1, delay, lightningColor);
                            break;
                        case GT_RectDestroyer:
                            gems[y][x]->destroy();
                            destroyRect(x, y, kRectBombDestructionSize, kRectBombDestructionSize);
                            break;
                        default:
                            gems[y][x]->destroy(delay);
                            break;
					}
				}
                
                int score = Gem::scoreForGem(gems[y][x]->getType());
                
				if(gemState != GS_Matched) {
					for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
						(*it)->onGemDestroyed(gems[y][x]->getGemColour(), x, y, score);
					}
				}
				gems[y][x]->destroy(delay);
			}
		}
	}
}

void GemField::destroyEntireField() {
    SpriteBatchNode *batch = SpriteBatchNode::create("gems/gems.png");
    batch->setPosition({0, 0});
    this->addChild(batch);
    
    auto applyLightningAtPosWithXYDirectionsDelayAndColor = [=](const Point &pos, int xDir, int yDir, float delay, Color3B lightningColor) {
        int squaresCount = MAX(kFieldHeight, kFieldWidth) * 2;
        
        Point currentIndices = {pos.x / kTileSize, kFieldHeight - pos.y / kTileSize};
        
        auto placeSprite = [=](const Point &index, float delay, Color3B color) {
            if(index.x >= kFieldWidth || index.x < 0 || index.y >= kFieldHeight || index.y < 0) {
                return;
            }
            
            Sprite *lightning = Sprite::createWithSpriteFrameName("lightningSquare.png");
            
            lightning->setOpacity(0);
            lightning->setColor(color);
            
            batch->addChild(lightning, zGem - 1);
            
            Point pos = Gem::convertCoordinatesToPixels(index.x, index.y);
            
            lightning->setPosition(pos);
            lightning->runAction(Sequence::create(DelayTime::create(delay * 0.4),
                                                  FadeTo::create(0.1 * delay, 150),
                                                  DelayTime::create(0.56 * delay),
                                                  CallFunc::create([=](){
                lightning->removeFromParent();
            }), NULL));
            
        };
        
        for(int i = 0; i < squaresCount; ++i) {
            placeSprite({currentIndices.x + xDir * i, currentIndices.y + yDir * i}, delay + 0.03 * i, lightningColor);
            placeSprite({currentIndices.x - xDir * i, currentIndices.y - yDir * i}, delay + 0.03 * i, lightningColor);
        }
    };
    
    float delay = 0.05;
    
    for(int i = 0; i < kFieldHeight; ++i) {
        for(int j = 0; j < kFieldWidth; ++j) {
            if(fieldMask[i][j] == 1) {
                this->destroyGem(j, i, 0.1 + delay * i);
            }
        }
    }
    
    for(int i = 0; i < kFieldHeight; ++i) {
        applyLightningAtPosWithXYDirectionsDelayAndColor(Gem::convertCoordinatesToPixels(kFieldWidth / 2, i), 1, 0, delay * i,
                                                         {255 * CCRANDOM_0_1(), 255 * CCRANDOM_0_1(), 255 * CCRANDOM_0_1()});
    }
    
    batch->runAction(Sequence::create(DelayTime::create(delay * kFieldWidth * kFieldHeight * 4),
                                      CallFunc::create([=]() {
        batch->removeFromParent();
    }),
                                      NULL));
    
    state = FS_Destroying;

}

void GemField::removeGem(int x, int y) {
	if(fieldMask[y][x] == 1) {
		gems[y][x]->remove();
	}
}

void GemField::destroyStraightLine(int originX, int originY, bool isHorizontal, int length) {
	int stepX = 1;
	int stepY = 1;
    
	if(isHorizontal) {
		stepY = 0;
	} else {
		stepX = 0;
	}
    
	if(length == -1) {
		length = kFieldWidth + kFieldHeight;
	}
    
	int distance = 1;
    
	destroyGem(originX, originY);
    
	while(distance <= length) {
		if(areIndicesWithinField(originX + stepX * distance, originY + stepY * distance)) {
			destroyGem(originX + stepX * distance, originY + stepY * distance);
		}
		if(areIndicesWithinField(originX - stepX * distance, originY - stepY * distance)) {
			destroyGem(originX - stepX * distance, originY - stepY * distance);
		}
		distance++;
	}
	for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
		(*it)->onGemsStartedSwapping();
	}
	state = FS_Destroying;
}

void GemField::destroyCross(int originX, int originY, int length) {
	int stepX = 1;
	int stepY = 1;
    
	if(length == -1) {
		length = kFieldWidth + kFieldHeight;
	}
    
	int distance = 1;
    
	destroyGem(originX, originY);
    
	while(distance <= length) {
		if(areIndicesWithinField(originX + stepX * distance, originY)) {
			destroyGem(originX + stepX * distance, originY);
		}
		if(areIndicesWithinField(originX - stepX * distance, originY)) {
			destroyGem(originX - stepX * distance, originY);
		}
		if(areIndicesWithinField(originX, originY + stepY * distance)) {
			destroyGem(originX, originY + stepY * distance);
		}
		if(areIndicesWithinField(originX, originY - stepY * distance)) {
			destroyGem(originX, originY - stepY * distance);
		}
		distance++;
	}
	for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
		(*it)->onGemsStartedSwapping();
	}
	state = FS_Destroying;
}

void GemField::destroyRect(int originX, int originY, int width, int height) {
    //	int stepX = 1;
    //	int stepY = 1;
    
	int distance = 1;
    
    if(width == -1) {
		width = kFieldWidth * 2;
	}
	if(height == -1) {
		height = kFieldHeight * 2;
	}
    
	destroyGem(originX, originY);
    
	while(distance <= width || distance <= height) {
        
		for(int y = 0; y < kFieldHeight; y++) {
			for(int x = 0; x < kFieldWidth; x++) {
				if(distance <= width && abs(x - originX) == distance && abs(y - originY) <= distance && abs(y - originY) <= height) {
					destroyGem(x, y);
				} else {
					if (distance <= height && abs(y - originY) == distance && abs(x - originX) <= distance && abs(x - originX) <= width) {
						destroyGem(x, y);
					}
				}
			}
		}
		distance++;
	}
	for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
		(*it)->onGemsStartedSwapping();
	}

	state = FS_Destroying;
}

#pragma mark - swapping stuff

void GemField::swapGems(int fromX, int fromY, int toX, int toY) {
	Gem *first = nullptr;
	Gem *second = nullptr;
    
	// pointers for gems we will swap
	first = gems[fromY][fromX];
	second = gems[toY][toX];
    
    bool skipMatchLookup = false;
    
    // check super swaps here
    if(first->getType() == GT_Hypercube && second->getType() == GT_Hypercube) {
        skipMatchLookup = true;
        
        first->swapTo(toX, toY, false, GS_AboutToDestroyByHypercube);
        second->swapTo(fromX, fromY, false, GS_AboutToDestroyByHypercube);
        
        state = FS_SwappingTwoHypercubes;
        
    } else if(first->getType() == GT_Hypercube || second->getType() == GT_Hypercube) {
        skipMatchLookup = true;
        
        Gem *hypercubeMaker = nullptr;
        Gem *gem = nullptr;
        
        if(first->getType() == GT_Hypercube) {
            hypercubeMaker = first;
            gem = second;
            
            int tx = fromX;
            fromX = toX;
            toX = tx;
            
            int ty = fromY;
            fromY = toY;
            toY = ty;
            
        } else {
            hypercubeMaker = second;
            gem = first;
            
        }
        
        switch(gem->getType()) {
            case GT_Colour:
                state = FS_SwappingHypercubeWithNormalIcon;
                
                gem->swapTo(toX, toY, false, GS_AboutToDestroyByHypercube);
                hypercubeMaker->swapTo(fromX, fromY, false, GS_AboutToDestroyByHypercube);
                
                // todo: we don't take care about mask here
                for(int i = 0; i < kFieldHeight; ++i) {
                    for(int j = 0; j < kFieldWidth; ++j) {
                        if(fieldMask[i][j] == 1 && gem != gems[i][j] && hypercubeMaker != gems[i][j] && gems[i][j]->getType() == GT_Colour && gem->getGemColour() == gems[i][j]->getGemColour()) {
                            gems[i][j]->prepareToBeDestroyedByHypercube();
                        }
                    }
                }
                break;
            case GT_LineDestroyer:
            case GT_Cross:
            case GT_RectDestroyer:
            case GT_Explosion:
                state = FS_SwappingHypercubeWithFourInRowIcon;
                
                gem->swapTo(toX, toY, false, GS_AboutToTurnIntoBomb);
                hypercubeMaker->swapTo(fromX, fromY, false, GS_AboutToDestroyByHypercube);
                
                for(int i = 0; i < kFieldHeight; ++i) {
                    for(int j = 0; j < kFieldWidth; ++j) {
                        if(fieldMask[i][j] == 1 && gem != gems[i][j] && hypercubeMaker != gems[i][j] && gems[i][j]->getType() == GT_Colour && gem->getGemColour() == gems[i][j]->getGemColour()) {
                            gems[i][j]->prepareToTurnIntoBombByHypercube();
                        }
                    }
                }
                break;
        }
    } else if((first->getType() == GT_LineDestroyer && second->getType() == GT_LineDestroyer) ||
              (first->getType() == GT_Cross && second->getType() == GT_Cross)) {
        skipMatchLookup = true;
        state = FS_SwappingTwoFourInRowIcons;
        
        first->swapTo(toX, toY, false, GS_AboutToExplodeWithCross);
        second->swapTo(fromX, fromY, false, GS_AboutToExplodeWithCross);
    } else {
        state = FS_Moving;
    }
    
	swapGemsIndices(fromX, fromY, toX, toY);
    bool isValidMove = skipMatchLookup;
    
    if(!skipMatchLookup) {
        bool hasMatches = hasAnyMatches();
        isValidMove = hasMatches;
        
        //Change gems' positions - if there is no match, they move forward then backward
        first->swapTo(toX, toY, !hasMatches);
        second->swapTo(fromX, fromY, !hasMatches);
        
        // If there was no match swap indices again
        if(!hasMatches) {
            swapGemsIndices(fromX, fromY, toX, toY);
        }
    }
    
	for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
		(*it)->onMoveMade(isValidMove);
		(*it)->onGemsStartedSwapping();
	}
}

void GemField::swapGemsIndices(int fromX, int fromY, int toX, int toY) {
	Gem *tmp;
	//Swap gems in the array
	tmp = gems[fromY][fromX];
	gems[fromY][fromX] = gems[toY][toX];
	gems[toY][toX] = tmp;
}

void GemField::selectGem(int x, int y) {
    SafeDelete(selectedGemCoordinates);
    
	selectedGemCoordinates = new Point(x, y);
	gems[y][x]->select();
}

void GemField::deselectGem(int x, int y) {
    SafeDelete(selectedGemCoordinates);
	
	selectedGemCoordinates = nullptr;
	gems[y][x]->deselect();
}

#pragma mark - public API for players

void GemField::swipeAction(const Point &startCoordinates, int direction) {
    string file = "btnClick.wav";
    
	int fromX = startCoordinates.x;
	int fromY = startCoordinates.y;
	
	int toX = fromX;
	int toY = fromY;
    
	if(fromX >= 0 && fromX < kFieldWidth && fromY >= 0 && fromY < kFieldHeight) {
		switch (direction) {
			case D_Up:
				if(fromY > 0) {
					toY -= 1;
				} break;
			case D_Down:
				if(fromY < kFieldHeight - 1) {
					toY += 1;
				} break;
			case D_Left:
				if(fromX > 0) {
					toX -= 1;
				} break;
			case D_Right:
				if(fromX < kFieldWidth - 1) {
					toX += 1;
				} break;
			default:
				break;
		}
	}
	if((toX - fromX) + (toY - fromY) != 0) {
		if(selectedGemCoordinates != nullptr) {
            //file = "click2.wav";
			deselectGem(selectedGemCoordinates->x, selectedGemCoordinates->y);
		}
		if(fieldMask[fromY][fromX] == 1 && freezeMask[fromY][fromX] == 0 && fieldMask[toY][toX] == 1  && freezeMask[toY][toX] == 0 &&
           !((gems[fromY][fromX]->getGemColour() == GC_Hypercube && gems[toY][toX]->getGemColour() == GC_Rainbow) ||
             (gems[toY][toX]->getGemColour() == GC_Hypercube && gems[fromY][fromX]->getGemColour() == GC_Rainbow))) {
            Gem *first = gems[fromY][fromX];
            Gem *second = gems[toY][toX];

            if(first->getGemColour() == GC_Coin || second->getGemColour() == GC_Coin) {
                state = FS_Destroying;
                
                for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
                    (*it)->onMoveMade(true);
                    (*it)->onGemsStartedSwapping();
                }
                
                if(first->getGemColour() == GC_Coin) {
                    destroyGem(fromX, fromY);
                }
                
                if(second->getGemColour() == GC_Coin) {
                    destroyGem(toX, toY);
                }
            } else {
                swapGems(fromX, fromY, toX, toY);
            }
		}
	}
    
    SimpleAudioEngine::getInstance()->playEffect(file.c_str());
}

void GemField::clickAction(const Point &clickCoordinates) {
	int toX = clickCoordinates.x;
	int toY = clickCoordinates.y;
    
    //string file = "click1.wav";

	if(toX >= 0 && toX < kFieldWidth && toY >= 0 && toY < kFieldHeight) {
		if(fieldMask[toY][toX] == 1 && freezeMask[toY][toX] == 0) {
			if(selectedGemCoordinates == nullptr) {
                if(gems[toY][toX]->getGemColour() == GC_Coin) {
                    state = FS_Destroying;
                    for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
                        (*it)->onMoveMade(true);
                        (*it)->onGemsStartedSwapping();
                    }
                    
                    this->destroyGem(toX, toY);
                } else {
                    selectGem(toX, toY);
                }
			} else {
				int selectedX = selectedGemCoordinates->x;
				int selectedY = selectedGemCoordinates->y;
				// Same gem
				if(selectedGemCoordinates->getDistance(clickCoordinates) == 0) {
					deselectGem(selectedX, selectedY);
                    //file = "click0.wav";
				} else {
                    // Different gems
					// Nearby gem
					if(selectedGemCoordinates->getDistance(clickCoordinates) == 1) {
						if(toX >= 0 && toX < kFieldWidth && toY >= 0 && toY < kFieldHeight) {
							deselectGem(selectedX, selectedY);
                            
                            if(gems[toY][toX]->getGemColour() == GC_Coin) {
                                state = FS_Destroying;
                                for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
                                    (*it)->onMoveMade(true);
                                    (*it)->onGemsStartedSwapping();
                                }
                                
                                this->destroyGem(toX, toY);
                            } else {
                                swapGems(selectedX, selectedY, toX, toY);
                            }
						}
					} else {
						deselectGem(selectedX, selectedY);
                        
                        //file = "click2.wav";
                        
                        if(gems[toY][toX]->getGemColour() == GC_Coin) {
                            state = FS_Destroying;
                            for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
                                (*it)->onMoveMade(true);
                                (*it)->onGemsStartedSwapping();
                            }
                            
                            this->destroyGem(toX, toY);
                        } else {
                            selectGem(toX, toY);
                        }
					}
				}
			}
		}
	}
    
    //SimpleAudioEngine::getInstance()->playEffect(file.c_str());
}

#pragma mark - update

void GemField::visit() {
    glEnable(GL_SCISSOR_TEST);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    Point pos = this->getPosition();
    int x = 0, y = 0;
    
    y = (visibleSize.height - kTileSize * (kFieldHeight)) / 2;
    
    //y = abs(pos.y - kTileSize * kFieldHeight / 4) + 140;
    
    glScissor(x, y, visibleSize.width, kTileSize * kFieldHeight);
    
    Node::visit();
    
    glDisable(GL_SCISSOR_TEST);
}

void GemField::update(float dt) {
	switch(state) {
		case FS_Ready:
			if(checkAvailableMoves()) {
				for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
					(*it)->onGemsFinishedMoving();
				}
				resetGemsState();
				state = FS_Waiting;
			} else {
				shuffleField(false);
				for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
					(*it)->onGemsToBeShuffled();
				}
			}
			break;
		case FS_Moving:
			if(!areGemsBeingMoved()) {
				//CCLOG("-GP_Moving");
				state = FS_Searching;
                
            }
			break;
        case FS_ApplyingRainbowGem:
//            if(!areGemsBeingMoved()) {
//                state = FS_Refilling;
//            }
            break;
		case FS_Searching:
			//CCLOG("-GP_Searching");
			if(hasAnyMatches()) {
				// If we do - find and resolve them
				resolveMatches();
				state = FS_Transforming;
			} else {
				state = FS_Ready;
                
                bool isRainbowGemDestroyed = false;
                
                // first, check whether there's a rainbow gem and it is at the bottom of the grid
                for(int j = 0; j < kFieldWidth; ++j) {
                    if(fieldMask[kFieldHeight - 1][j] == 1 && gems[kFieldHeight - 1][j]->getGemColour() == GC_Rainbow) {
                        isRainbowGemDestroyed = true;

                        this->removeGem(j, kFieldHeight - 1);
                        
                        // apply any other temoral color
                        gems[kFieldHeight - 1][j]->setGemColour(GC_Red);
                        
                        for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
                            (*it)->onRainbowGemDestroyed(j, kFieldHeight - 1);
                        }
                        
                        state = FS_ApplyingRainbowGem;

                        break;
                    }
                }
                
                if(!isRainbowGemDestroyed) {
                    // generate a new one if none found
                    bool isThereAnyRainbowGemAlready = false;
                    
                    for(int i = 0; i < kFieldHeight; ++i) {
                        for(int j = 0; j < kFieldWidth; ++j) {
                            if(fieldMask[i][j] == 1 && gems[i][j]->getGemColour() == GC_Rainbow) {
                                isThereAnyRainbowGemAlready = true;
                                break;
                            }
                        }
                    }
                    
                    if(!isThereAnyRainbowGemAlready && (CCRANDOM_0_1() * 10) > 6) {
                        int rainbowGemX = clampf(CCRANDOM_0_1() * kFieldWidth, 3, kFieldWidth - 4);
                        int rainbowGemY = 0;
                        
                        if(fieldMask[rainbowGemY][rainbowGemX] != 1) {
                            for(int i = 0; i < kFieldHeight; i++) {
                                for(int j = 0; j < kFieldWidth; ++j) {
                                    
                                    // assume there's at least one valid cell
                                    if(fieldMask[i][j] == 1) {
                                        rainbowGemX = j;
                                        rainbowGemY = i;
                                        break;
                                    }
                                }
                            }
                        }
                        
                        gems[rainbowGemY][rainbowGemX]->transformIntoBonus(GT_RainbowMaker);
                        
                        state = FS_Transforming;
                    }
                }
			}
			break;
		case FS_Transforming:
			if(!areGemsBeingMoved()) {
				destroyMatchedGems();
				state = FS_Destroying;
			}
			break;
		case FS_Destroying:
			if(!areGemsBeingMoved()) {
				//CCLOG("-GP_Destroying");
				resetGemsState();
				refillField();
				state = FS_Refilling;
			}
			break;
		case FS_Refilling:
			if(!areGemsBeingMoved()) {
				//CCLOG("-GP_Refilling");
				state = FS_Searching;
			}
			break;
		case FS_Shuffling:
			if(!areGemsBeingMoved()) {
				//CCLOG("-GP_Shuffling");
				state = FS_Searching;
			}
			break;
        case FS_SwappingHypercubeWithNormalIcon:
            if(!areGemsBeingMoved()) {
                
                // get a hypercube first
                Gem *hypercubeMaker = nullptr;
                
                for(int i = 0; i < kFieldHeight; ++i) {
                    for(int j = 0; j < kFieldWidth; ++j) {
                        if(hypercubeMaker == nullptr && fieldMask[i][j] == 1 && gems[i][j]->getState() == GS_AboutToDestroyByHypercube && gems[i][j]->getType() == GT_Hypercube) {
                            hypercubeMaker = gems[i][j];
                            break;
                        }
                    }
                }
                
                auto applyLightning = [this](const Point &startPos, const Point &endPos, float delay, GemColour color) {
                    Point pt1 = endPos;
                    Point pt2 = endPos;
                    
                    ccBezierConfig config = {endPos, pt1, pt2};

                    Color3B lightningColor = {255, 255, 255};
                    
                    switch(color) {
                        case GC_Red:
                            lightningColor = {255, 50, 50};
                            break;
                        case GC_Green:
                            lightningColor = {50, 255, 50};
                            break;
                        case GC_Blue:
                            lightningColor = {50, 50, 255};
                            break;
                        case GC_Purple:
                            lightningColor = {255, 50, 255};
                            break;
                        case GC_Orange:
                            lightningColor = {255, 150, 0};
                            break;
                        case GC_Yellow:
                            lightningColor = {255, 255, 0};
                            break;
                        case GC_White:
                        case GC_Hypercube:
                        case GC_Random:
                            lightningColor = {255, 255, 255};
                        default:
                            break;
                    }
                    
                    MotionStreak *streak = MotionStreak::create(delay * 0.45, 2, 25, lightningColor, "gems/lightningSquare.png");
                    streak->setColor(lightningColor);
                    streak->setPosition(startPos);
                    
                    this->addChild(streak, zGem - 1);
                    
                    streak->runAction(Sequence::create(DelayTime::create(delay * 0.2),
                                                       BezierTo::create(delay * 0.45, config),
                                                       DelayTime::create(delay *  0.4),
                                                       CallFunc::create([=]() {
                                                        streak->removeFromParent();
                                                       }),
                                                       NULL));
                };
                
                int streakNumber = 0;
                
                for(int i = 0; i < kFieldHeight; ++i) {
                    for(int j = 0; j < kFieldWidth; ++j) {
                        Gem *gem = gems[i][j];
                        
                        if(fieldMask[i][j] == 1 && gem->getState() == GS_AboutToDestroyByHypercube) {
                            
                            float delay = 0.6;
                            this->destroyGem(j, i, delay);
                            // apply lightning
                            Point endPos = Gem::convertCoordinatesToPixels(j, i);
                            applyLightning(hypercubeMaker->getPosition(), endPos, 0.7, gems[i][j]->getGemColour());
                            
                            streakNumber++;
                        }
                    }
                }
                
                state = FS_DestroyingNormalIconsAfterSwipe;
            } break;
        case FS_DestroyingNormalIconsAfterSwipe:
            if(!areGemsBeingMoved()) {
                state = FS_Destroying;
            } break;
        case FS_SwappingTwoFourInRowIcons:
            if(!areGemsBeingMoved()) {
                for(int i = 0; i < kFieldHeight; ++i) {
                    for(int j = 0; j < kFieldWidth; ++j) {
                        Gem *gem = gems[i][j];
                        
                        if(fieldMask[i][j] == 1 && gem->getState() == GS_AboutToExplodeWithCross) {
                            gem->setType(GT_Cross);
                            this->destroyGem(j, i);
                        }
                    }
                }
                
                state = FS_Destroying;
            } break;
        case FS_SwappingHypercubeWithFourInRowIcon:
            if(!areGemsBeingMoved()) {
                for(int i = 0; i < kFieldHeight; ++i) {
                    for(int j = 0; j < kFieldWidth; ++j) {
                        Gem *gem = gems[i][j];
                        
                        if(fieldMask[i][j] == 1 && gem->getState() == GS_AboutToTurnIntoBomb) {
                            gem->transformIntoBonus(GT_LineDestroyer, i * 0.06 + j * 0.04, GS_AboutToExplodeByHypercube);
                            // apply state here
                        }
                    }
                }
                
                state = FS_TurningGemsToFourInRowIcons;
            } break;
        case FS_TurningGemsToFourInRowIcons:
            if(!areGemsBeingMoved()) {
                for(int i = 0; i < kFieldHeight; ++i) {
                    for(int j = 0; j < kFieldWidth; ++j) {
                        Gem *gem = gems[i][j];
                        
                        if(fieldMask[i][j] == 1 && (gem->getState() == GS_AboutToExplodeByHypercube || gem->getState() == GS_AboutToDestroyByHypercube)) {
                            //gem->transformIntoBonus(GT_LineDestroyer, i * 0.06 + j * 0.04);
                            gem->setState(GS_ExplodingByHypercube);
                            this->destroyGem(j, i, i * 0.06 + j * 0.04);
                        }
                    }
                }
                
                state = FS_DestroyingFourInRowIcons;
                
                // destroy with delay
            } break;
        case FS_DestroyingFourInRowIcons:
            if(!areGemsBeingMoved()) {
                state = FS_Destroying;
            }
            break;
        case FS_SwappingTwoHypercubes:
            if(!areGemsBeingMoved()) {
                // turn into plectrums
                for(int i = 0; i < kFieldHeight; ++i) {
                    for(int j = 0; j < kFieldWidth; ++j) {
                        if(fieldMask[i][j] == 1 && gems[i][j]->getState() != GS_AboutToDestroyByHypercube) {
                            //gems[i][j]->transformIntoPlectrum();
                            // destroy something may be?
                            
                        }
                    }
                }
                
                state = FS_TwoHypercubesSwapped;
            } break;
        case FS_TwoHypercubesSwapped:
            if(!areGemsBeingMoved()) {
                this->destroyEntireField();
            }
            break;
		default:
			break;
	}
}

#pragma mark - field shuffle & reset

void GemField::shuffleField(bool reset) {
	state = FS_Shuffling;
    
	if(reset) {
		resetField();
	} else {
        std::vector<Gem *> unusedGems;
        
        for(int x = 0; x < kFieldWidth; x++) {
			for(int y = kFieldHeight - 1; y >= 0; y--) {
				if(fieldMask[y][x] == 1) {
					unusedGems.push_back(gems[y][x]);
				}
			}
		}
        
        for(int x = 0; x < kFieldWidth; x++) {
			for(int y = kFieldHeight - 1; y >= 0; y--) {
				if(fieldMask[y][x] == 1) {
                    
                    int unusedGemIndex = rand() % unusedGems.size();
					gems[y][x] = unusedGems[unusedGemIndex];
                    gems[y][x]->reset(x, y, gems[y][x]->getGemColour(), gems[y][x]->getType());
                    
                    removeGem(x, y);
                    
                    unusedGems.erase(unusedGems.begin() + unusedGemIndex);
				}
			}
		}
	}
    
	refillField(false);
}

void GemField::resetField() {
	bool gotMatches = true;
	bool hasTurns = false;
	while(gotMatches || !hasTurns) {
		for(int x = 0; x < kFieldWidth; x++) {
			for(int y = kFieldHeight - 1; y >= 0; y--) {
				if(fieldMask[y][x] == 1) {
					gems[y][x]->reset(x, y);
					removeGem(x, y);
				}
			}
		}
		gotMatches = hasAnyMatches();
		hasTurns = checkAvailableMoves();
	}
}

#pragma mark -

void GemField::refillField(bool reset) {
	columnsWithMatches = 0;
	for(int x = 0; x < kFieldWidth; x++) {
		refillLine(x, D_Down, reset);
	}
}

void GemField::refillLine(int lineNumber, int direction, bool reset) {
	if(direction == D_Down) {
		int y = kFieldHeight - 1;
		int destroyedGems = 0;
		int emptyBlocks = 0;
		int timesToFall = 0;
		int distanceBetweenEmpty = 0;
		int lastDistance = 0;
        
        bool playFallEffect = true;
        
		// Until we reach the top of the line
		while(y >= 0) {
			// If there is a gem
			if(fieldMask[y][lineNumber] == 1 && gems[y][lineNumber]->getState() != GS_Immovable) {
				// If a gem is destroyed - account for it
				if(gems[y][lineNumber]->getState() == GS_Destroyed) {
					distanceBetweenEmpty++;
					destroyedGems++;
				} else {
                    // If the gem is not destroyed - move it down
					distanceBetweenEmpty++;
					this->moveGem(lineNumber, y, lineNumber, y + destroyedGems + emptyBlocks, columnsWithMatches, playFallEffect);
                    
                    playFallEffect = false;

					if(emptyBlocks > 0) {
						timesToFall--;
						while(timesToFall <= 0 && emptyBlocks > 0) {
							emptyBlocks--;
							timesToFall = lastDistance;
						}
					}
				}
			} else {
                // If there is no gem - count that as a non-existing block
				if(destroyedGems > 0) {
					if(timesToFall > 0) {
						emptyBlocks++;
					} else {
						timesToFall = destroyedGems;
						emptyBlocks = 1;
					}
					lastDistance = distanceBetweenEmpty;
					distanceBetweenEmpty = 0;
				}
			}
			y--;
		}
        
		y += destroyedGems + emptyBlocks;
        
        playFallEffect = true;
        
		int gemsFallen = 0;
		while(y >= 0) {
			if(fieldMask[y][lineNumber] == 1 && gems[y][lineNumber]->getState() != GS_Immovable) {
				GemType type;
				GemColour colour;
				if(reset) {
					if(CCRANDOM_0_1() < kBonusChance) {
						type = (GemType) ((int) (CCRANDOM_0_1() * 3));
						colour = GC_Random;
					} else {
						type = GT_Colour;
						colour = GC_Random;
					}
				} else {
					type = gems[y][lineNumber]->getType();
					colour = gems[y][lineNumber]->getGemColour();
				}
				gems[y][lineNumber]->reset(lineNumber, y - destroyedGems - emptyBlocks, colour, type);
                
				gems[y][lineNumber]->fallTo(lineNumber, y, gemsFallen, columnsWithMatches, playFallEffect);
                
                playFallEffect = false;
				gemsFallen++;
				if(emptyBlocks > 0) {
					timesToFall--;
					if(timesToFall <= 0) {
						emptyBlocks--;
						y--;
						timesToFall = lastDistance;
					}
				}
			} else {
				emptyBlocks--;
			}
			y--;
		}
		if(destroyedGems > 0) {
			columnsWithMatches++;
		}
	}
	
}

#pragma mark -

void GemField::moveGem(int fromX, int fromY, int toX, int toY, int rowsToWait, bool playFallEffect) {
//    if(fieldMask[fromY][fromX] == 0 || fieldMask[toY][toX] == 0) {
//        CCLOG("null! %i, %i, %i, %i", fromX, fromY, toX, toY);
//    } else
    if(fromX != toX || fromY != toY) {
		swapGemsIndices(fromX, fromY, toX, toY);
		gems[toY][toX]->fallTo(toX, toY, 0, rowsToWait, playFallEffect);
	}
}

void GemField::resetGemsState() {
	for(int y = 0; y < kFieldHeight; y++) {
		for(int x = 0; x < kFieldWidth; x++) {
			if(fieldMask[y][x] == 1) {
				if(gems[y][x]->getState() != GS_Destroyed) {
					gems[y][x]->resetState();
				}
			}
		}
	}
}

// Freezes a gem in place. It acts like a hole
// - if power == 1 - cant'be moved, may be matched with others or destroyed by explosion
// - if power == 2 - must be hit by explosion in order to become matchable
void GemField::freezeGem(int x, int y, int power) {
	freezeMask[y][x] = power;
	gems[y][x]->setFreeze(power);
}

bool GemField::compareGemsColors(int firstX, int firstY, int secondX, int secondY) {
	if(firstX >= 0 && firstX < kFieldWidth && firstY >= 0 && firstY < kFieldHeight && secondX >= 0 && secondX < kFieldWidth && secondY >= 0 && secondY < kFieldHeight) {
		return fieldMask[firstY][firstX] == 1 && fieldMask[secondY][secondX] == 1 && gems[firstY][firstX]->getGemColour() == gems[secondY][secondX]->getGemColour();
	}
	return false;
}

bool GemField::areIndicesWithinField(int x, int y) {
	return x >= 0 && x < kFieldWidth && y >= 0 && y < kFieldHeight;
}

#pragma mark - moves & hints

MoveList GemField::getMoves() {
	MoveList availableMoves;
	for(int y = 0; y < kFieldHeight; y++) {
		availableMoves.splice(availableMoves.begin(), getMovesForLine(0, y, kFieldWidth - 1, y));
	}
	for(int x = 0; x < kFieldWidth; x++) {
		availableMoves.splice(availableMoves.begin(), getMovesForLine(x, 0, x, kFieldHeight - 1));
	}
	return availableMoves;
}

MoveList GemField::getMovesForLine(int fromX, int fromY, int toX, int toY) {
	MoveList availableMoves;
    
	int stepX = 1;
	int stepY = 1;
    
	if(fromX == toX) {
		stepX = 0;
		//CCLOG("Checking line %i", fromX);
	}
	if(fromY == toY) {
		stepY = 0;
		//CCLOG("Checking line %i", fromY);
	}
    
	int x = fromX;
	int y = fromY;
    
	while(fieldMask[y][x] == 0) {
		x += stepX;
		y += stepY;
	}
    
	if(x > toX || y > toY) {
		return availableMoves;
	}
    
	GemColour currentColor = gems[y][x]->getGemColour();
    GemType currentType = gems[y][x]->getType();
    GemState currentState = gems[y][x]->getState();

	int chainLength = 1;
    
	x += stepX;
	y += stepY;
    
	while(x <= toX + 1 && y <= toY + 1) {
        while((x <= toX && y <= toY) && fieldMask[y][x] == 1 && freezeMask[y][x] <= 1 && (gems[y][x]->getGemColour() == currentColor) &&
              !(gems[y][x]->getType() == GT_Hypercube && gems[y][x]->getState() != GS_Transformed) &&
              !(currentType == GT_Hypercube && currentState != GS_Transformed) &&
                currentType != GT_RainbowMaker && currentColor != GC_Rainbow &&
                gems[y][x]->getType() != GT_RainbowMaker && gems[y][x]->getGemColour() != GC_Rainbow) {
            
			x += stepX;
			y += stepY;
			chainLength++;
		}
		// Find turns for 3s with gaps
		if(chainLength == 1) {
			if(stepY == 0) {
				if(x - 3 >= fromX) {
					if(compareGemsColors(x - 1, y, x - 3, y)) {
						addMoveToList(getGemMove(x - 2, y, D_Up), availableMoves);
						addMoveToList(getGemMove(x - 2, y, D_Down), availableMoves);
					}
				}
			} else {
				if(y - 3 >= fromY) {
					if(compareGemsColors(x, y - 1, x, y - 3)) {
						addMoveToList(getGemMove(x, y - 2, D_Left), availableMoves);
						addMoveToList(getGemMove(x, y - 2, D_Right), availableMoves);
					}
				}
			}
		}
		
		// Find couples and look for 3s, 4s, 5s
		if(chainLength == 2) {
			// If we're checking a horizontal line
			if(stepY == 0) {
				// If there is a gem to the left of the pair
				if(x - 3 >= fromX) {
					addMoveToList(getGemMove(x - 3, y, D_Up), availableMoves);
					addMoveToList(getGemMove(x - 3, y, D_Down), availableMoves);
					addMoveToList(getGemMove(x - 3, y, D_Left), availableMoves);
				}
				// If there is a gem to the right of the pair
				if(x <= toX) {
					addMoveToList(getGemMove(x, y, D_Up), availableMoves);
					addMoveToList(getGemMove(x, y, D_Down), availableMoves);
					addMoveToList(getGemMove(x, y, D_Right), availableMoves);
				}
			} else {
                // If we're checking a vertical line
				// If there is a gem above the couple
				if(y - 3 >= fromY) {
					addMoveToList(getGemMove(x, y - 3, D_Left), availableMoves);
					addMoveToList(getGemMove(x, y - 3, D_Right), availableMoves);
					addMoveToList(getGemMove(x, y - 3, D_Up), availableMoves);
				}
				// If there is a gem below the couple
				if(y <= toY) {
					addMoveToList(getGemMove(x, y, D_Left), availableMoves);
					addMoveToList(getGemMove(x, y, D_Right), availableMoves);
					addMoveToList(getGemMove(x, y, D_Down), availableMoves);
				}
			}
		}
		
		if(x > toX || y > toY) {
			return  availableMoves;
		}
        
		while(fieldMask[y][x] == 0) {
			x += stepX;
			y += stepY;
		}
        
		if(x > toX || y > toY) {
			return  availableMoves;
		}
        
		currentColor = gems[y][x]->getGemColour();
        currentType = gems[y][x]->getType();
        currentState = gems[y][x]->getState();

		chainLength = 1;
		x += stepX;
		y += stepY;
	}
	return  availableMoves;
}

Move GemField::getGemMove(int x, int y, Direction direction) {
	int stepX = 0;
	int stepY = 0;
	switch(direction) {
		case D_Down: stepY = 1;
			break;
		case D_Up: stepY = -1;
			break;
		case D_Left: stepX = -1;
			break;
		case D_Right: stepX = 1;
			break;
		default:
			break;
	}
    
	if(x >= 0 && x < kFieldWidth && y >=0 && y < kFieldHeight && x + stepX >= 0 && x + stepX < kFieldWidth && y + stepY >=0 && y + stepY < kFieldHeight) {
		if(fieldMask[y + stepY][x + stepX] == 1 && fieldMask[y][x] == 1 && freezeMask[y + stepY][x + stepX] == 0 && freezeMask[y][x] == 0) {
			swapGemsIndices(x, y, x + stepX, y + stepY);
            
			MatchList foundMatches = findMatches();
			swapGemsIndices(x, y, x + stepX, y + stepY);
			return Move(x, y, x + stepX, y + stepY, foundMatches);
		}
	}
    
	return Move();
}

void GemField::addMoveToList(const Move &move, MoveList &list) {
	if(move.legal) {
		list.push_front(move);
	}
}

void GemField::showTip() {
	MoveList moves = getMoves();
    
	int moveNumber = moves.size() / 2;
	MoveList::iterator it = moves.begin();
	while(moveNumber > 0) {
		it++;
		moveNumber--;
	}
    
	gems[(*it).fromY][(*it).fromX]->swapTo((*it).toX, (*it).toY, true);
	gems[(*it).toY][(*it).toX]->swapTo((*it).fromX, (*it).fromY, true);
}

bool GemField::checkAvailableMoves() {
	MoveList availableMoves = getMoves();
	return !availableMoves.empty();
}

bool GemField::areGemsBeingMoved() {
	for(int y = 0; y < kFieldHeight; y++) {
		for(int x = 0; x < kFieldWidth; x++) {
			if(fieldMask[y][x] == 1) {
				if(gems[y][x]->getState() == GS_Moving || gems[y][x]->getState() == GS_Destroying || gems[y][x]->getState() == GS_Transforming) {
					return true;
				}
			}
		}
	}
    
	return false;
}

#pragma mark - watcher delegation

void GemField::addWatcher(FieldWatcherDelegate *watcher) {
	watchers.push_front(watcher);
}

void GemField::setState(FieldState state) {
    this->state = state;
}
