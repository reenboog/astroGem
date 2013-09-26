#ifndef __FIELD_WATCHER_H__
#define __FIELD_WATCHER_H__

#include "Constants.h"
#include <list>


class FieldWatcherDelegate {
public:
	virtual void onGemsMatched(int length, GemColour colour) = 0;
	virtual void onGemDestroyed(GemColour colour) = 0;
	virtual void onGemsToBeShuffled() = 0;
	virtual void onGemsStartedSwapping() = 0;
	virtual void onGemsFinishedMoving() = 0;
	
    // player specific stuff
	virtual void onMoveMade(bool legal) = 0;
};

typedef std::list<FieldWatcherDelegate *> FieldWatcherDelegatePool;


#endif // __FIELD_WATCHER_H__