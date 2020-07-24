#pragma once

#include "MurkaTypes.h"

class MurkaRendererBase;

namespace murka {
    
// Global defines

#define DEFAULT_LINE_HEIGHT 20

class MurkaAssets {
public:
    FontObject* paragraphFont = NULL;
    FontObject* headerFont = NULL;
    FontObject* monoFont = NULL;
    
    std::map<std::string, FontObject*> fontMap;
    
    void setupFonts(std::string paragraphFontFilename, float paragraphSize,
                    std::string headerFontFilename, float headerSize,
                    std::string monoFontFilename, float monofontSize,
                    bool isAbsolutePath = false,
					MurkaRendererBase* renderer = nullptr) {
#ifdef MURKA_OF
        paragraphFont = new FontObject();
        headerFont = new FontObject();
        monoFont = new FontObject();
        paragraphFont->load(paragraphFontFilename, paragraphSize, isAbsolutePath, renderer);
        headerFont->load(headerFontFilename, headerSize, isAbsolutePath, renderer);
        monoFont->load(monoFontFilename, monofontSize, isAbsolutePath, renderer);
#endif
    }
    
    void addFont(std::string fontId, std::string fontName, float fontSize) {
#ifdef MURKA_OF
        fontMap[fontId] = new FontObject();
        fontMap[fontId]->load(fontName, fontSize);
#endif
    }
    
    MurkaColor widgetBgColor = {0.1, 0.1, 0.1};
    MurkaColor widgetFgColor = {0.98, 0.98, 0.98};
    
    float getFontLineHeight(FontObject* font) {
        if (font == NULL) {
            return DEFAULT_LINE_HEIGHT;
        } else {
#ifdef MURKA_OF
            return font->getLineHeight();
#endif
        }
    }
};

}
