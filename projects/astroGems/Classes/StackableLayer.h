//
//  StackableLayer.h
//  astroGems
//
//  Created by Alex Gievsky on 02.11.13.
//
//

#ifndef astroGems_StackableLayer_h
#define astroGems_StackableLayer_h

#include "cocos2d.h"

class StackableLayer {
public:
    StackableLayer() {
        baseLayer = nullptr;
        blackout = nullptr;
		touchBlocker = nullptr;
    }
    
	virtual void popUp(StackableLayer *baseLayer) = 0;
	virtual void popOut() = 0;
	virtual void putOn(StackableLayer *overlay) = 0;
	virtual void takeOff() = 0;
    
	virtual void disableTouches() = 0;
	virtual void enableTouches() = 0;
protected:
	cocos2d::LayerColor *blackout;
	StackableLayer *baseLayer;
	cocos2d::Menu *touchBlocker;
};

#endif
