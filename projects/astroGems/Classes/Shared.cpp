//
//  Shared.cpp
//  1pGigTickets
//
//  Created by Alex Gievsky on 18.09.13.
//
//

#include "Shared.h"

Animation* Shared::loadAnimation(string file, string name) {
    Animation *animation = nullptr;
    
    file = FileUtils::getInstance()->fullPathForFilename(file.c_str());
    
    Dictionary *fileDict = Dictionary::createWithContentsOfFile(file.c_str());
    Dictionary *animationDict = (Dictionary*)fileDict->objectForKey(name);
    
    animation = Animation::create();
    
    float delay = animationDict->valueForKey("delay")->floatValue();
    animation->setDelayPerUnit(delay);
    
    string frameNameStr = name;
    
    const String *baseFrameName = animationDict->valueForKey("baseFrameName");
    if(baseFrameName->compare("")) {
        frameNameStr = baseFrameName->getCString();
    }
    
    const String *framesStr = animationDict->valueForKey("frames");
    Array *indices = framesStr->clone()->componentsSeparatedByString(",");
    
    for(int i = 0; i < indices->count(); ++i) {
        String *frameName = String::create(frameNameStr);
        String *index = (String *)indices->objectAtIndex(i);
        frameName->appendWithFormat("%s.png", index->getCString());
        
        animation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName->getCString()));
    }
    
    AnimationCache::getInstance()->addAnimation(animation, name.c_str());
    
    return animation;
}