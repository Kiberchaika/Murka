#pragma once

#include "MurkaTypes.h"
#include <map>
#include <string>
#include <cstring>

namespace murka {

// Global defines
#define DEFAULT_LINE_HEIGHT 20

class MurkaRendererBase;

struct FontInfo {
    std::string fontName;
    int fontSize;
    bool fromData;
    char* data;
    int dataSize;

    FontInfo(const std::string& name, int size, char* fontData = nullptr, int fontDataSize = 0)
        : fontName(name), fontSize(size), fromData(fontData != nullptr), dataSize(fontDataSize) {
        if (fromData && fontDataSize > 0) {
            fromData = true;
            dataSize = fontDataSize;
            data = new char[fontDataSize];
            std::memcpy(data, fontData, fontDataSize);
        } else {
            fromData = false;
            dataSize = 0;
            data = nullptr;
        }
    }

    ~FontInfo() {
        if (data) {
            delete[] data;
        }
    }

    bool operator<(const FontInfo& other) const {
        return std::tie(fontName, fontSize) < 
               std::tie(other.fontName, other.fontSize);
    }
};

class MurkaAssets {
public:
    
    std::string resourcesPath;

    void setResourcesPath(std::string path) {
        resourcesPath = path;
    }
    
    std::string getResourcesPath() {
        return resourcesPath;
    }
    
    std::map<FontInfo, FontObject*> fonts;
    std::map<FontInfo, FontObject*>::iterator currentFont;

    FontObject* getCurrentFont() {
        if (fonts.empty()) {
            jassertfalse; // you need to load font
        }
        return currentFont->second;
    }

    void clearFontsCache() {
        for (auto& font : fonts) {
            delete font.second;
        }
        fonts.clear();
        currentFont = fonts.end();
    }
    
    void reloadFonts(MurkaRendererBase* renderer) {
        std::vector<FontInfo> fontsToReload;
        for (const auto& font : fonts) {
            fontsToReload.push_back(font.first);
        }
        
        clearFontsCache();
        
        for (const auto& fontInfo : fontsToReload) {
            if (fontInfo.fromData) {
                setFont(fontInfo.fontName, fontInfo.data, fontInfo.dataSize, fontInfo.fontSize, renderer);
            } else {
                setFont(fontInfo.fontName, fontInfo.fontSize, renderer);
            }
        }
    }
    
    bool setFont(std::string name, int size, MurkaRendererBase* renderer) {
        // todo: get scale from renderer

        FontInfo fontId = { name, size };
        auto font = fonts.find(fontId);
        bool res = true;

        if (font != fonts.end()) {
            currentFont = font;
        }
        else {
            auto fontObj = new FontObject(); 
            if (resourcesPath.empty() || !fontObj->load(resourcesPath + (resourcesPath.back() == '/' ? "" : "/") + name, size, true, renderer)) {
                jassertfalse; // can't load font
                res = false;
                delete fontObj;
            } else {
                fonts[fontId] = fontObj;
                currentFont = fonts.find(fontId);
            }
        }
        return res;
    }

    bool setFont(std::string name, const char* data, int dataSize, int size, MurkaRendererBase* renderer) {
        FontInfo fontId(name, size, const_cast<char*>(data), dataSize);
        auto font = fonts.find(fontId);
        bool res = true;

        if (font != fonts.end()) {
            currentFont = font;
        }
        else {
            auto fontObj = new FontObject();
            if (!fontObj->load(data, dataSize, size, true, renderer)) {
                jassertfalse; // can't load font
                res = false;
                delete fontObj;
            } else {
                fonts[fontId] = fontObj;
                currentFont = fonts.find(fontId);
            }
        }
        return res;
    }

    float getFontLineHeight(FontObject* font) {
        if (font == nullptr) {
            return DEFAULT_LINE_HEIGHT;
        } else {
            return font->getLineHeight();
        }
    }
};

}
