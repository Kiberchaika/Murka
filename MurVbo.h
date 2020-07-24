#pragma once

#ifdef MURKA_OF

class MurVbo {
public:
    MurVbo(){};
    
    ofVbo internal;
    
    /* // THESE, but through Renderer (with the *Renderer argument)
     context->vbo->setVertexData((float*)verts, nverts, GL_DYNAMIC_DRAW);
     context->vbo->setTexCoordData(tcoords, nverts, GL_DYNAMIC_DRAW);
     

     context->vbo->draw(GL_TRIANGLES, 0, nverts);
     */
};

#else // Default version

class MurVbo {
public:
    MurVbo(){};
};

#endif
