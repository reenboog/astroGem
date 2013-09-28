#include "GemField.h"
#include "ccMacros.h"
#include <random>

#include "Gem.h"
//#include "Player.h"

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
                //				// Fill holes with tiles
                //				Sprite *tile = Sprite::create("tile.png");
                //				tile->setZOrder(kZOrderTile);
                //				tile->setPosition(Gem::convertCoordinatesToPixels(x, y));
                //				this->addChild(tile);
			}
		}
	}
    
    state = FS_Ready;
    
	// Test specific loadouts here
    
	/*
     // Field with no matches (and no turns)
     {2,3,4,2,3,4,2,3,4},
     {3,4,2,3,4,2,3,4,2},
     {4,2,3,4,2,3,4,2,3},
     {2,3,4,2,3,4,2,3,4},
     {3,4,2,3,4,2,3,4,2},
     {4,2,3,4,2,3,4,2,3}
     
     // Square of doom
     {2,3,4,2,3,4,2,3,4},
     {3,4,2,3,4,2,3,4,2},
     {4,2,3,1,1,1,4,2,3},
     {2,3,4,1,1,1,2,3,4},
     {3,4,2,1,1,1,3,4,2},
     {4,2,3,4,2,3,4,2,3}
     
     // Cross that is also a match 5
     {2,3,4,2,3,4,2,3,4},
     {3,4,2,1,4,2,3,4,2},
     {4,1,1,4,1,1,4,2,3},
     {2,3,4,1,3,4,2,3,4},
     {3,4,2,1,4,2,3,4,2},
     {4,2,3,4,2,3,4,2,3}
     
     // A tricky situation
     {2,3,4,2,1,4,2,3,4},
     {3,4,2,3,5,2,3,4,2},
     {4,1,1,4,5,3,4,2,3},
     {5,5,5,1,5,1,1,3,4},
     {3,4,2,3,1,2,3,4,2},
     {4,2,3,4,1,3,4,2,3}
     
     // Match 4 by bonus
     {2,3,4,2,3,4,2,3,4},
     {3,4,2,3,4,2,3,4,2},
     {4,1,1,4,1,3,4,2,3},
     {2,3,4,1,3,4,2,3,4},
     {3,4,2,3,4,2,3,4,2},
     {4,2,3,4,2,3,4,2,3}
     
     {0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0},
     {0,0,0,1,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0}
     
     // Match 4 which has a bonus
     {2,3,4,2,3,4,2,3,4},
     {3,4,2,3,4,2,3,4,2},
     {4,1,1,4,1,3,4,2,3},
     {2,3,4,1,3,4,2,3,4},
     {3,4,2,3,4,2,3,4,2},
     {4,2,3,4,2,3,4,2,3}
     
     {0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0},
     {0,0,1,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0},
     {0,0,0,0,0,0,0,0,0}
     */
	if(kPreloadField) {
		const int customField[kFieldHeight][kFieldWidth] = {
			{2,3,4,2,1,4,2,3},
			{3,4,2,3,5,2,3,2},
			{4,1,1,4,5,3,2,3},
			{5,5,5,1,5,2,3,4},
			{3,4,2,3,2,3,4,2},
			{4,2,3,1,3,4,2,3},
            {4,2,3,1,3,4,2,3},
            {4,2,3,1,3,4,2,3}
		};
        
		const int customFieldType[kFieldHeight][kFieldWidth] = {
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
	shuffleField(false);
    
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
    
	int currentValue = gems[y][x]->getGemColour();
	int chainLength = 1;
    
	x += stepX;
	y += stepY;
    
	while(x <= toX && y <= toY) {
		while((x <= toX && y <= toY) && fieldMask[y][x] == 1 && freezeMask[y][x] <= 1 && (gems[y][x]->getGemColour() == currentValue || (gems[y][x]->getGemColour() == GC_Wild  && chainLength >= 2) || currentValue == GC_Wild)) {
            //
            //while((x <= toX && y <= toY) && fieldMask[y][x] == 1 && freezeMask[y][x] <= 1 && (gems[y][x]->getGemColour() == currentValue && currentValue != GC_Wild)) {
            //
            
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
        
		currentValue = gems[y][x]->getGemColour();
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
    
	// First resolve all the matches
	for(MatchList::iterator it = foundMatches.begin(); it != foundMatches.end(); it++) {
		resolveMatch(*it);
	}
}

void GemField::resolveMatch(const Match &match) {
	bool bonusWasAdded = false;
	bool createBonusNext = false;
	for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
		(*it)->onGemsMatched(match.length, match.colour);
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
					bonusWasAdded = true;
				}
			} else {
                // If the gem wasn't moved, but the one before it was a bonus one, turn it into a bonus
				if(createBonusNext) {
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
            // If we come arcoss a gem that is already matched or turninig into a bonus
			// Transform into bonus for cross
            //			if(gems[y][x]->getType() != GT_Colour) {
            //				createBonusNext = true;
            //				gems[y][x]->match();
            //			} else {
            //				gems[y][x]->transformIntoBonus(kCrossMatchBonus);
            //				bonusWasAdded = true;
            //			}
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
void GemField::destroyLine(int fromX, int fromY, int toX, int toY, bool destroyTransformed) {
	int stepX = 1;
	int stepY = 1;
	if(fromX == toX) {
		stepX = 0;
	}
	if(fromY == toY) {
		stepY = 0;
	}
	//CCLOG("Destroy line %i, %i - %i", fromX, fromY, toY);
	int y = fromY;
	int x = fromX;
	while(y <= toY && x <= toX) {
		if(!destroyTransformed && gems[y][x]->getState() == GS_Transformed) {
			CCLOG("Fledgling @ %i, %i", x, y);
		} else {
			destroyGem(x, y);
		}
		y += stepY;
		x += stepX;
	}
}

void GemField::destroyGem(int x, int y) {
	if(fieldMask[y][x] == 1) {
		if(gems[y][x]->getState() != GS_Destroying) {
			if(freezeMask[y][x] > 1) {
				freezeGem(y, x, freezeMask[y][x]-1);
			} else {
				if(freezeMask[y][x] == 1) {
					freezeGem(y, x, freezeMask[y][x]-1);
				}
				if(gems[y][x]->getType() != GT_Colour) {
                    if(gems[y][x]->getType() == GT_Explosion) {
                        gems[y][x]->transformIntoBonus((GemType)(GT_LineHor + (GemType)CCRANDOM_0_1()));
                    }
                    
					switch(gems[y][x]->getType()) {
                        case GT_Cross:
                            gems[y][x]->destroy();
                            destroyLine(x, 0, x, kFieldHeight - 1);
                            destroyLine(0, y, kFieldWidth - 1, y);
                            break;
                        case GT_LineHor :
                            gems[y][x]->destroy();
                            destroyLine(0, y, kFieldWidth - 1, y);
                            break;
                        case GT_LineVer :
                            gems[y][x]->destroy();
                            destroyLine(x, 0, x, kFieldHeight - 1);
                            break;
                        default:
                            gems[y][x]->destroy();
                            break;
					}
				}
				if(gems[y][x]->getState() != GS_Matched) {
					for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
						(*it)->onGemDestroyed(gems[y][x]->getGemColour());
					}
				}
				gems[y][x]->destroy();
			}
		}
	}
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
	Gem *first;
	Gem *second;
    
	//Pointers for gems we will swap
	first = gems[fromY][fromX];
	second = gems[toY][toX];
    
    // check super swaps here
    //if(first->getType() == something && second->getType() == something) {
    //
    //}
    
	swapGemsIndices(fromX, fromY, toX, toY);
    
	bool hasMatches = hasAnyMatches();
    
	//Change gems' positions - if there is no match, they move forward then backward
	first->swapTo(toX, toY, !hasMatches);
	second->swapTo(fromX, fromY, !hasMatches);
    
	// If there was no match swap indices again
	if(!hasMatches) {
		swapGemsIndices(fromX, fromY, toX, toY);
	}
    
	for(FieldWatcherDelegatePool::iterator it = watchers.begin(); it != watchers.end(); it++) {
		(*it)->onMoveMade(hasMatches);
		(*it)->onGemsStartedSwapping();
	}
    
	state = FS_Moving;
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
	int fromX = startCoordinates.x;
	int fromY = startCoordinates.y;
	
	int toX = fromX;
	int toY = fromY;
    
	if(fromX >=0 && fromX < kFieldWidth && fromY >= 0 && fromY < kFieldHeight) {
		switch (direction)
		{
			case D_Up :
				if(fromY > 0) {
					toY -= 1;
				}
				break;
			case D_Down :
				if(fromY < kFieldHeight - 1) {
					toY += 1;
				}
				break;
			case D_Left :
				if(fromX > 0) {
					toX -= 1;
				}
				break;
			case D_Right :
				if(fromX < kFieldWidth - 1) {
					toX += 1;
				}
				break;
			default:
				break;
		}
	}
	if((toX - fromX) + (toY - fromY) != 0) {
		if(selectedGemCoordinates != nullptr) {
			deselectGem(selectedGemCoordinates->x, selectedGemCoordinates->y);
		}
		if(fieldMask[fromY][fromX] == 1 && freezeMask[fromY][fromX] == 0 && fieldMask[toY][toX] == 1  && freezeMask[toY][toX] == 0) {
			swapGems(fromX, fromY, toX, toY);
		}
	}
}

void GemField::clickAction(const Point &clickCoordinates) {
	int toX = clickCoordinates.x;
	int toY = clickCoordinates.y;
	if(toX >= 0 && toX < kFieldWidth && toY >= 0 && toY < kFieldHeight) {
		if(fieldMask[toY][toX] == 1 && freezeMask[toY][toX] == 0)
		{
			if(selectedGemCoordinates == nullptr) {
				selectGem(toX, toY);
				//CCLOG("Selected %i, %i", toX, toY);
			} else {
				int selectedX = selectedGemCoordinates->x;
				int selectedY = selectedGemCoordinates->y;
				// Same gem
				if(selectedGemCoordinates->getDistance(clickCoordinates) == 0)
				{
					//CCLOG("Deselected %f, %f", selectedGemCoordinates->x, selectedGemCoordinates->y);
					deselectGem(selectedX, selectedY);
				} else {
                    // Different gems
					// Nearby gem
					if(selectedGemCoordinates->getDistance(clickCoordinates) == 1) {
						if(toX >= 0 && toX < kFieldWidth && toY >= 0 && toY < kFieldHeight) {
							//CCLOG("Swap %f, %f with %f, %f", selectedGemCoordinates->x, selectedGemCoordinates->y, clickCoordinates.x, clickCoordinates.y);
							deselectGem(selectedX, selectedY);
							swapGems(selectedX, selectedY, toX, toY);
						}
					} else {
                        // Faraway gem
						//CCLOG("Deselected %f, %f - selected %f, %f", selectedGemCoordinates->x, selectedGemCoordinates->y, clickCoordinates.x, clickCoordinates.y);
						deselectGem(selectedX, selectedY);
						selectGem(toX, toY);
					}
				}
			}
		}
	}
}

#pragma mark - update

void GemField::visit() {
    glEnable(GL_SCISSOR_TEST);
    
    Size visibleSize = Director::getInstance()->getVisibleSize();
    
    Point pos = this->getPosition();
    int x = 0, y = 0;
    
    y = abs(pos.y - kTileSize * kFieldHeight / 4);
    
    glScissor(x, y, visibleSize.width, kTileSize * (kFieldHeight + 2.2));
    
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
				shuffleField(true);
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
		case FS_Searching:
			//CCLOG("-GP_Searching");
			if(hasAnyMatches()) {
				// If we do - find and resolve them
				resolveMatches();
				state = FS_Transforming;
			} else {
				state = FS_Ready;
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
		for(int x = 0; x < kFieldWidth; x++) {
			for(int y = kFieldHeight - 1; y >= 0; y--) {
				if(fieldMask[y][x] == 1) {
					gems[y][x]->reset(x, y, gems[y][x]->getGemColour(), gems[y][x]->getType());
					removeGem(x, y);
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
					moveGem(lineNumber, y, lineNumber, y + destroyedGems + emptyBlocks, columnsWithMatches);
                    
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
				gems[y][lineNumber]->fallTo(lineNumber, y, gemsFallen, columnsWithMatches);
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

void GemField::moveGem(int fromX, int fromY, int toX, int toY, int rowsToWait) {
	if(fromX != toX || fromY != toY)
	{
		swapGemsIndices(fromX, fromY, toX, toY);
		gems[toY][toX]->fallTo(toX, toY, 0, rowsToWait);
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
    
	int currentValue = gems[y][x]->getGemColour();
	int chainLength = 1;
    
	x += stepX;
	y += stepY;
    
    
	while(x <= toX + 1 && y <= toY + 1) {
		while((x <= toX && y <= toY) && fieldMask[y][x] == 1 && freezeMask[y][x] <= 1 && (gems[y][x]->getGemColour() == currentValue || (gems[y][x]->getGemColour() == GC_Wild  && chainLength >= 2) || currentValue == GC_Wild)) {
			x += stepX;
			y += stepY;
			chainLength++;
		}
//      while((x <= toX && y <= toY) && fieldMask[y][x] == 1 && freezeMask[y][x] <= 1 && (gems[y][x]->getGemColour() == currentValue || (gems[y][x]->getGemColour() == GC_Note  &&
//          chainLength >= 2) || currentValue == GC_Note)) {
//			x += stepX;
//			y += stepY;
//			chainLength++;
//		}
		//CCLOG("Stopped at %i,%i with length %i", x, y, chainLength);
		
        
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
        
		currentValue = gems[y][x]->getGemColour();
		chainLength = 1;
		x += stepX;
		y += stepY;
	}
	return  availableMoves;
}

Move GemField::getGemMove(int x, int y, Direction direction) {
	int stepX = 0;
	int stepY = 0;
	switch (direction)
	{
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

#pragma mark - debug

void GemField::print() {
	cocos2d::String* str = cocos2d::String::create("");
	for(int y = 1; y <= kFieldHeight; y++) {
		str->_string = "";
		for(int x = 1; x <= kFieldWidth; x++) {
			if(gems[y][x]->getState() == GS_Destroyed) {
				str->appendWithFormat("%c ", "x");
			} else {
				str->appendWithFormat("%i ", gems[y][x]->getType());
			}
		}
		CCLOG("%s", str->getCString());
	}
	CCLOG("\n----------------\n");
}
