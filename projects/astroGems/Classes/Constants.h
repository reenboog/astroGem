#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <vector>
#include <list>

#define kFieldWidth 8
#define kFieldHeight 9

#define kGemTypeAmount 7

#define kTileSize 80.0f
#define kSwapTime 0.1f
#define kFallTime 0.1f
#define kDestructionTime 0.205f

#define kRectBombDestructionSize 1

#define kBonusChance 0.0f

#define kGemCloudSpritesTag 1000

#define zGem 2
#define zTile 3
#define zLighting 90
#define zMatchScore 100

#define kTipDelay 1.f

#define kTransformationTime 0.2f

#define kColumnsFallDelay 0.5
#define kRowsFallDelay 0.3

#define kSpawnBonuses 1

#define kScoreMultiplierFadeOutSpeed 4
#define kScoreMultiplierFadeInSpeed 2
#define kScoreMultiplierMaxProgress 20

const bool kPreloadField = false;

enum GemColour {
	GC_Random,
	GC_Red,
	GC_Green,
	GC_Blue,
	GC_Orange,
	GC_Yellow,
    GC_Purple,
    GC_White,
    GC_Hypercube
};

enum GemState {
	GS_Idle,
	GS_Moving,
	GS_Moved,
	GS_Selected,
	GS_Matched,
	GS_Transforming,
	GS_Transformed,
	GS_Destroying,
	GS_Destroyed,
    GS_AboutToDestroyByHypercube,
    GS_AboutToExplodeByHypercube,
    GS_ExplodingByHypercube,
    GS_AboutToTurnIntoBomb,
    GS_AboutToExplodeWithCross,
	GS_Immovable
};

enum GemType {
	GT_Colour,
	GT_Cross,
	GT_LineHor,
	GT_LineVer,
    GT_LineDestroyer,
    GT_RectDestroyer,
	GT_Explosion,
    GT_Hypercube,
};

enum Direction {
	D_Down,
	D_Up,
	D_Left,
	D_Right,
};

enum MatchType {
    MT_Horizontal,
    MT_Vertical,
    MT_None
};

enum FieldState {
	FS_Ready,
	FS_Waiting,
	FS_Moving,
	FS_Searching,
	FS_Transforming,
	FS_Destroying,
	FS_Refilling,
	FS_Shuffling,
    FS_SwappingHypercubeWithNormalIcon,
    FS_DestroyingNormalIconsAfterSwipe,
    FS_SwappingTwoFourInRowIcons,
    FS_SwappingHypercubeWithFourInRowIcon,
    FS_TurningGemsToFourInRowIcons,
    FS_DestroyingFourInRowIcons,
};

struct Match {
	Match(int fromX, int fromY, int toX, int toY, GemColour colour) {
		this->fromX = fromX;
		this->fromY = fromY;
		this->toX = toX;
		this->toY = toY;
		this->length = (toX - fromX) + (toY - fromY) + 1;
		this->colour = colour;
	}
    
	Match() {}
    
	int fromX;
	int fromY;
	int toX;
	int toY;
	int length;
	GemColour colour;
};

typedef std::list<Match> MatchList;


#if(kSpawnBonuses == 1)
    const GemType kVerticalMatchFourBonus = GT_LineDestroyer;
    const GemType kHorizontalMatchFourBonus = GT_LineDestroyer;
    const GemType kVerticalMatchFiveBonus = GT_Hypercube;
    const GemType kHorizontalMatchFiveBonus = GT_Hypercube;
    const GemType kCrossMatchBonus = GT_Cross;
#else
    const GemType kVerticalMatchFourBonus = GT_Colour;
    const GemType kHorizontalMatchFourBonus = GT_Colour;
    const GemType kVerticalMatchFiveBonus = GT_Colour;
    const GemType kHorizontalMatchFiveBonus = GT_Colour;
    const GemType kCrossMatchBonus = GT_Colour;
#endif

#define SafeDelete(p) {if(p) {delete p; p = NULL;}}

#endif // __CONSTANTS_H__