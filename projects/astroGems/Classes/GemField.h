#ifndef __GEMFIELD__
#define __GEMFIELD__

#include "Constants.h"
#include "cocos2d.h"
#include "FieldWatcherDelegate.h"

class Gem;
class Player;

USING_NS_CC;

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

class GemField : public Node {
    
public:
    // init
	GemField();
	~GemField();
    
    virtual bool init();
    CREATE_FUNC(GemField);
    
    // delegation stuff
	void addWatcher(FieldWatcherDelegate *watcher);
    
    // debug
	void print();
	void printMask();
    
    // interface to operate the field
	void swipeAction(Point startCoordinates, int direction);
	void clickAction(Point clickCoordinates);
	
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
	void destroyLine(int fromX, int fromY, int toX, int toY, bool destroyTransformed = true);
	void destroyGem(int x, int y);
    
	bool hasAnyMatches();
    
	void resolveMatches();
	void resolveMatch(const Match &match);
    
	void refillLine(int lineNumber, int direction = D_Down, bool reset = true);
	void refillField(bool reset = true);
    
	bool areGemsBeingMoved();
    
    void visit();
	void update(float dt);
    
	void resetField();
	void shuffleField(bool reset = true);
    
	bool checkAvailableMoves();
	MoveList getMovesForLine(int fromX, int fromY, int toX, int toY);
	Move getGemMove(int x, int y, Direction direction);
	void addMoveToList(Move move, MoveList &list);
    
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
