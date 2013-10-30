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
    unsigned long configFileSize = 0;
    string configData = reinterpret_cast<const char*>(FileUtils::getInstance()->getFileData("Res/config.json", "r", &configFileSize));
    
    rapidjson::Document configDoc;
    configDoc.Parse<0>(configData.c_str());
    
    if(configDoc["filePaths"].IsArray()) {
        for(auto it = configDoc["filePaths"].Begin(); it != configDoc["filePaths"].End(); ++it) {
            string path = it->GetString();
            
            FileUtils::getInstance()->addSearchPath((path + "/" + languagePrefix).c_str());
            FileUtils::getInstance()->addSearchPath(path.c_str());
        }
    }

    // load strings
    unsigned long stringsFileSize = 0;
    string stringsData = reinterpret_cast<const char*>(FileUtils::getInstance()->getFileData("strings.json", "r", &stringsFileSize));

    rapidjson::Document stringsDoc;
    stringsDoc.Parse<0>(stringsData.c_str());
    
    if(stringsDoc["strings"].IsObject()) {
        for(auto it = stringsDoc["strings"].MemberBegin(); it != stringsDoc["strings"].MemberEnd(); ++it) {
            string key = it->name.GetString();
            string value = it->value.GetString();
            
            __sharedInstance->strings.insert(make_pair(key, value));
        }
    }
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