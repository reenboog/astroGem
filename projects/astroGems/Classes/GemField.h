#ifndef __GEMFIELD__
#define __GEMFIELD__

#include "Constants.h"
#include "cocos2d.h"
#include "FieldWatcherDelegate.h"

class Gem;
class Player;

USING_NS_CC;

struct Move {
	Move(int fromX, int fromY, int toX, int toY, MatchList matches) {
		this->fromX = fromX;
		this->fromY = fromY;
		this->toX = toX;
		this->toY = toY;
        
		this->matches = matches;
		this->legal = !matches.empty();
	}
    
	Move() { this->legal = false; }
    
	bool legal;
    
	int fromX;
	int fromY;
	int toX;
	int toY;
    
	MatchList matches;
};

typedef std::list<Move> MoveList;

class GemField: public Node {
public:
    // init
	GemField();
	~GemField();
    
    virtual bool init();
    CREATE_FUNC(GemField);
    
    // delegation stuff
	void addWatcher(FieldWatcherDelegate *watcher);
    
    void setState(FieldState state);
    
    void shuffleField(bool reset = true);
    
    // interface to operate the field
    
	void swipeAction(const Point &startCoordinates, int direction);
	void clickAction(const Point &clickCoordinates);
    
    // helper methods for destruction
	void destroyStraightLine(int originX, int originY, bool isHorizontal, int length = -1);
	void destroyCross(int originX, int originY, int length = -1);
	void destroyRect(int originX, int originY, int width, int height);
	
	MoveList getMoves();
	void showTip();
private:
	MatchList findMatches();
	MatchList findMatchesInLine(int fromX, int fromY, int toX, int toY);
    
	void selectGem(int x, int y);
	void deselectGem(int x, int y);
    
	void swapGems(int fromX, int fromY, int toX, int toY);
	void swapGemsIndices(int fromX, int fromY, int toX, int toY);
	void moveGem(int fromX, int fromY, int toX, int toY, int rowsToWait = 0);
    
	void freezeGem(int x, int y, int power);
	void removeGem(int x, int y);
    
	void destroyMatchedGems();
	void destroyLine(int fromX, int fromY, int toX, int toY, bool destroyTransformed = true, float delay = 0.0);
	void destroyGem(int x, int y, float delay = 0.0f);
    
	bool hasAnyMatches();
    
	void resolveMatches();
	void resolveMatch(const Match &match);
    
	void refillLine(int lineNumber, int direction = D_Down, bool reset = true);
	void refillField(bool reset = true);
    
    bool areIndicesWithinField(int x, int y);
    
	bool areGemsBeingMoved();
    
    void visit();
	void update(float dt);
    
	void resetField();
    
	bool checkAvailableMoves();
	MoveList getMovesForLine(int fromX, int fromY, int toX, int toY);
	Move getGemMove(int x, int y, Direction direction);
    
	void addMoveToList(const Move &move, MoveList &list);
    
	bool compareGemsColors(int firstX, int firstY, int secondX, int secondY);
    
	void resetGemsState();
private:
	int fieldMask[kFieldHeight][kFieldWidth];
	int freezeMask[kFieldHeight][kFieldWidth];
    
	Gem* gems[kFieldHeight][kFieldWidth];
	
	Point *selectedGemCoordinates;
    
	FieldState state;
    
	int columnsWithMatches;
    
	FieldWatcherDelegatePool watchers;
};



#endif // __GEMFIELD__
