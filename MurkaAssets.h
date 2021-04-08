#pragma once

#include "MurkaTypes.h"

#define DEFAULT_LINE_HEIGHT 20


namespace murka {

class MurkaRendererBase;


class MurkaAssets {
public:
    
    std::string resourcesPath;

    void setResourcesPath(std::string path) {
        resourcesPath = path;
    }
    
    std::string getResourcesPath() {
        return resourcesPath;
    }
    
    typedef tuple<string /* fontName */, int /* fontSize */> FontInfo;
    
    std::map<FontInfo, FontObject*> fonts;
    std::map<FontInfo, FontObject*>::iterator currentFont;

     FontObject* getCurrentFont() {
        return currentFont->second;
    }

    void clearFontsCache() {
        fonts.clear();
        currentFont = fonts.begin();
    }
    
    void updateFontsTextures(MurkaRendererBase* renderer) {
        for (auto const& font : fonts) {
            font.second->updateTexture(renderer);
        }
    }
    
    void clearFontsTextures() {
        for (auto const& font : fonts) {
            font.second->clearTexture();
        }
    }
    
    void setFont(std::string name, int size, MurkaRendererBase* renderer) {
		// todo: get scale from renderer

        FontInfo fontId = {name, size};
        auto font = fonts.find(fontId);
        if (font != fonts.end()) {
            currentFont = font;
        } else {
            auto font = new FontObject();
            font->load(resourcesPath +
                       (resourcesPath[resourcesPath.length() - 1] == '/' ? "" : "/") + name, size, true, renderer);
            fonts[fontId] = font;
            currentFont = fonts.find(fontId);
        }
    }

    
    float getFontLineHeight(FontObject* font) {
        if (font == NULL) {
            return DEFAULT_LINE_HEIGHT;
        } else {
            return font->getLineHeight();
        }
    }
};

}
