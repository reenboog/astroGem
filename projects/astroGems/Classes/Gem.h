#ifndef __GEM_H__
#define __GEM_H__

#include "Constants.h"
#include "cocos2d.h"

USING_NS_CC;

class Gem : public Sprite {
public:
	Gem(GemColour colour = GC_Random, GemType type = GT_Colour);
	~Gem();

	void init(int x, int y, GemColour colour = GC_Random, GemType type = GT_Colour);

	void select();
	void deselect();

	void swapTo(int x, int y, bool goBack = false);
	void fallTo(int x, int y, int blocksToWait = 0, int rowsToWait = 0);

	void match(MatchType matchType = MT_None);
	void destroy();
    void transformIntoBonus(GemType type);
	void remove();

	void reset(int x, int y, GemColour = GC_Random, GemType = GT_Colour);

	GemState getState();
	GemType getType();

    void setGemColour(GemColour color);
	GemColour getGemColour();
    void resetState();

	void setFreeze(int power);
	//void makeBomb(GemType effect);
	static Point convertCoordinatesToPixels(int x, int y);
private:
    void applyBonusStyling();
	void moveTo(int x, int y, float time, bool goBack = false, int blocksToWait = 0, int rowsToWait = 0);
	void onMovementEnd(Object *sender);
	void onDestructionEnd(Object *sender);
    void onTransformationEnd(Object *sender);
private:
	GemState state;
	GemColour colour;
	GemType type;
};

#endif // __GEM_H__
