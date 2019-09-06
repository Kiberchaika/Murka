#pragma once

#include "MurkaTypes.h"

namespace murka {
    
// Global defines

#define DEFAULT_LINE_HEIGHT 20



class MurkaAssets {
public:
    FontObject* paragraphFont = NULL;
    FontObject* headerFont = NULL;
    FontObject* monoFont = NULL;
    
    void setupFonts(std::string paragraphFontFilename, float paragraphSize,
                    std::string headerFontFilename, float headerSize,
                    std::string monoFontFilename, float monofontSize) {
#ifdef MURKA_OF
        paragraphFont = new FontObject();
        headerFont = new FontObject();
        monoFont = new FontObject();
        paragraphFont->load(paragraphFontFilename, paragraphSize);
        headerFont->load(headerFontFilename, headerSize);
        monoFont->load(monoFontFilename, monofontSize);
//        ofLog() << "is paragraph font loaded?" << paragraphFont->isLoaded();
//        ofLog() << "is header font loaded?" << headerFont->isLoaded();
#endif
    }
    
    MurkaColor widgetBgColor = {0.1, 0.1, 0.1};
    MurkaColor widgetFgColor = {0.8, 0.8, 0.8};
    
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
