#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define kFieldWidth 8
#define kFieldHeight 8

#define kGemTypeAmount 6

#define kTileSize 80.0f
#define kSwapTime 0.1f
#define kFallTime 0.1f
#define kDestructionTime 0.1f

#define kBonusChance 0.0f

#define kZOrderGem 2
#define kZOrderTile 3

#define kAITurnDelay 0.5f
#define kTipDelay 1.f

#define kTransformationTime 0.3f

#define kColumnsFallDelay 0
#define kRowsFallDelay 0

#define kPlayersAmount 2

#define kSpawnBonuses 1

const bool kPreloadField = false;

enum GemColour {
	GC_Random,
	GC_Guitar,
	GC_Keyboard,
	GC_Microphone,
	GC_Saxophone,
	GC_Plectrum,
    GC_Question,
	GC_Wild
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
	GS_Immovable
};

enum GemType {
	GT_Colour,
	GT_Cross,
	GT_LineHor,
	GT_LineVer,
	GT_Explosion,
    GT_WildMaker
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
};

#if(kSpawnBonuses == 1)
    const GemType kVerticalMatchFourBonus = GT_Explosion;
    const GemType kHorizontalMatchFourBonus = GT_Explosion;
    const GemType kVerticalMatchFiveBonus = GT_WildMaker;
    const GemType kHorizontalMatchFiveBonus = GT_WildMaker;
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