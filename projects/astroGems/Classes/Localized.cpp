//
//  Localized.cpp
//  astroGems
//
//  Created by Alex Gievsky on 30.10.13.
//
//

#include "Localized.h"
#include "Shared.h"
#include "rapidjson/document.h"

Localized *Localized::__sharedInstance = nullptr;

Localized::~Localized() {
    //
}

Localized::Localized() {
    //
}

void Localized::load() {
    if(__sharedInstance == nullptr) {
        __sharedInstance = new Localized();
    } else {
        return;
    }

    LanguageType currentLanguage = Application::getInstance()->getCurrentLanguage();
    string languagePrefix = Shared::languageShortNameForType(currentLanguage);
    
    // load file paths
    unsigned long configDataSize = 0;
    unsigned char *t = (FileUtils::getInstance()->getFileData("Res/config.json", "r", &configDataSize));
    
    char *configData = new char[configDataSize + 1];
    memcpy(configData, t, configDataSize);
    configData[configDataSize] = NULL;
    
    delete[] t;
    t = NULL;
    
    //configData = configData.substr(0, configData.find_last_of('}') + 1);
    
    rapidjson::Document configDoc;
    configDoc.Parse<0>(configData);
    
    const auto &pathsArray = configDoc["filePaths"];
    if(pathsArray.IsArray()) {
        for(auto it = pathsArray.Begin(); it != pathsArray.End(); ++it) {
            string path = it->GetString();
            
            FileUtils::getInstance()->addSearchPath((path + "/" + languagePrefix).c_str());
            FileUtils::getInstance()->addSearchPath(path.c_str());
        }
    }
    
    delete[] configData;
    
    // load strings
    unsigned long stringsDataSize = 0;
    t = (FileUtils::getInstance()->getFileData("strings.json", "r", &stringsDataSize));
    //stringsData = stringsData.substr(0, stringsData.find_last_of('}') + 1);
    
    char *stringsData = new char[stringsDataSize + 1];
    memcpy(stringsData, t, stringsDataSize);
    stringsData[stringsDataSize] = NULL;
    
    delete[] t;
    t = NULL;
    
    rapidjson::Document stringsDoc;
    stringsDoc.Parse<0>(stringsData);
    
    const auto &stringsMap = stringsDoc["strings"];
    if(stringsMap.IsObject()) {
        for(auto it = stringsMap.MemberBegin(); it != stringsMap.MemberEnd(); ++it) {
            string key = it->name.GetString();
            string value = it->value.GetString();
            
            __sharedInstance->strings.insert(make_pair(key, value));
        }
    }
    
    delete[] stringsData;
}

void Localized::purge() {
    delete __sharedInstance;
    __sharedInstance = nullptr;
}

string Localized::getString(const string &key) {
    if(__sharedInstance == nullptr) {
        Localized::load();
    }
    
    auto stringIt = __sharedInstance->strings.find(key);
    
    if(stringIt != __sharedInstance->strings.end()) {
        return stringIt->second;
    } else {
        CCLOG("String not found for key: %s", key.c_str());
    }
    
    return "notFound";
}