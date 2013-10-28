#ifndef __FIELD_WATCHER_H__
#define __FIELD_WATCHER_H__

#include "Constants.h"
#include <list>


class FieldWatcherDelegate {
public:
	virtual void onGemsMatched(int length, GemColour colour, int startX, int startY, int endX, int endY, int score) = 0;
	virtual void onGemDestroyed(GemColour colour, int x, int y, int score) = 0;
	virtual void onGemsToBeShuffled() = 0;
	virtual void onGemsStartedSwapping() = 0;
	virtual void onGemsFinishedMoving() = 0;
    virtual void onStartedResolvinMatches(const MatchList &founMatches) = 0;
    virtual void onRainbowGemDestroyed(int x, int y) = 0;
	
    // player specific stuff
	virtual void onMoveMade(bool legal) = 0;
};

typedef std::list<FieldWatcherDelegate *> FieldWatcherDelegatePool;


#endif // __FIELD_WATCHER_H__