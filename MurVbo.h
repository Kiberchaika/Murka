#pragma once

#ifdef MURKA_OF

#include "MurkaTypes.h"

class MurVbo {
    vector<MurkaPoint> verts;
    vector<MurkaPoint> texCoords;
    int usage = 0;

public:
    ofVbo internal;

    MurVbo(){};

	void setVertexData(MurkaPoint* verts, int total, int usage) {
        this->usage = usage;
        this->verts.resize(total);
        memcpy(this->verts.data(), verts, total * sizeof(MurkaPoint));

        internal.setVertexData(&verts[0].x, 2, total, usage, sizeof(MurkaPoint));
	}

    void setTexCoordData(MurkaPoint* texCoords, int total, int usage) {
        this->usage = usage;
        this->texCoords.resize(total);
        memcpy(this->texCoords.data(), texCoords, total * sizeof(MurkaPoint));

        internal.setTexCoordData(&texCoords[0].x, total, usage, sizeof(MurkaPoint));
    }
    
    void update() {
        internal.setVertexData(&this->verts[0].x, 2, this->verts.size(), this->usage, sizeof(MurkaPoint));
        internal.setTexCoordData(&this->texCoords[0].x, this->texCoords.size(), this->usage, sizeof(MurkaPoint));
    }
    
    void clear() {
        internal.clear();
    }
    

    /* // THESE, but through Renderer (with the *Renderer argument)
     

     context->vbo->draw(GL_TRIANGLES, 0, nverts);
     */
};

#else // Default version

class MurVbo {
public:
    MurVbo(){};
};

#endif
