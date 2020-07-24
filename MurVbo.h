#pragma once

#ifdef MURKA_OF

#include "MurkaTypes.h"

class MurVbo {
public:
    MurVbo(){};
    
    ofVbo internal;
    
	void setVertexData(MurkaPoint* verts, int total, int usage) {
		internal.setVertexData(&verts[0].x, 2, total, usage, sizeof(MurkaPoint));
	}

	void setTexCoordData(MurkaPoint* texCoords, int total, int usage) {
		internal.setTexCoordData(&texCoords[0].x, total, usage, sizeof(MurkaPoint));
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
