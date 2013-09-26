//
//  Shared.h
//  1pGigTickets
//
//  Created by Alex Gievsky on 18.09.13.
//
//

#ifndef ___pGigTickets__Shared__
#define ___pGigTickets__Shared__

#include "cocos2d.h"

USING_NS_CC;

using std::string;

class Shared {
private:
    Shared() {};
    ~Shared() {};
public:
    static Animation* loadAnimation(string file, string name);
};

#endif /* defined(___pGigTickets__Shared__) */
