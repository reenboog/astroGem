//
//  Localized.h
//  astroGems
//
//  Created by Alex Gievsky on 30.10.13.
//
//

#ifndef __astroGems__Localized__
#define __astroGems__Localized__

#include <map>
#include <string>

#include "cocos2d.h"

USING_NS_CC;

using namespace std;

class Localized {
    typedef map<string, string> StringStringMap;
private:
    virtual ~Localized();
    Localized();
public:
    static string getString(const string &key);
    static void purge();
    
    static void load();
private:
    StringStringMap strings;
    static Localized *__sharedInstance;
};

#endif /* defined(__astroGems__Localized__) */
