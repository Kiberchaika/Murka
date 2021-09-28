#pragma once

#include "MurkaTypes.h"

namespace murka {

#if defined(MURKA_OF)


class MurVbo {
    vector<MurkaPoint> verts;
    vector<MurkaPoint> texCoords;
    int usage = 0;

public:
    ofVbo internal;

    MurVbo(){}

	bool isInited() {
		return internal.getIsAllocated();
	}

	void setup() {
	}
	
	void setVertexData(MurkaPoint* _verts, int total) {
        this->verts.resize(total);
        memcpy(this->verts.data(), _verts, total * sizeof(MurkaPoint));
	}

    void setTexCoordData(MurkaPoint* texCoords, int total) {
        this->texCoords.resize(total);
        memcpy(this->texCoords.data(), texCoords, total * sizeof(MurkaPoint));
    }
    
    void update(int usage) {
		this->usage = usage;
		internal.setVertexData(&this->verts[0].x, 2, this->verts.size(), this->usage, sizeof(MurkaPoint));
        internal.setTexCoordData(&this->texCoords[0].x, this->texCoords.size(), this->usage, sizeof(MurkaPoint));
    }
    
    void clear() {
        internal.clear();
    }
};

#elif defined(MURKA_JUCE)

class MurVbo {
	struct UVCoord {
		float u = 0;
		float v = 0;
	};

	struct VertexCoord {
		float x = 0;
		float y = 0;
		float z = 0;
	};

	struct Col {
		float r = 1;
		float g = 1;
		float b = 1;
		float a = 1;
	};

	vector<MurkaPoint3D> verts;
	vector<MurkaPoint> texCoords;
	vector<MurkaColor> cols;

	struct VboData {
        VertexCoord vert;
		UVCoord texCoord;
		Col col;
	};

	vector<VboData> vboData;

	GLuint VAO = 0;
	GLuint VBO = 0;
	juce::OpenGLContext* openGLContext = nullptr;
	bool loaded = false;
    bool inited = false;

public:
	MurVbo() {

	}

	~MurVbo() {
		if (isInited()) clear();
	}

	bool isInited() {
		return inited;
	}

	void setOpenGLContext(juce::OpenGLContext* openGLContext) {
		this->openGLContext = openGLContext;
	}

	void setup() {
		openGLContext->extensions.glGenVertexArrays(1, &VAO);
		openGLContext->extensions.glGenBuffers(1, &VBO);
        
        inited = true;
	}

	void clear() {
        if(inited) {
            openGLContext->extensions.glDeleteVertexArrays(1, &VAO);
            openGLContext->extensions.glDeleteBuffers(1, &VBO);

            inited = false;
        }
	}

	void setVertexData(const MurkaPoint3D* verts, int total) {
        this->verts.resize(total);
        memcpy(this->verts.data(), verts, total * sizeof(MurkaPoint3D));
	}

	void setTexCoordData(const MurkaPoint* texCoords, int total) {
		this->texCoords.resize(total);
		memcpy(this->texCoords.data(), texCoords, total * sizeof(MurkaPoint));
	}

	void setColData(const MurkaColor* cols, int total) {
		this->cols.resize(total);
		memcpy(this->cols.data(), cols, total * sizeof(MurkaColor));
	}

	void update(int usage, int attribLocationPosition, int attribLocationUv, int attribLocationCol) {
		if (!inited) return;

		int size = (std::max)(verts.size(), (std::max)(texCoords.size(), cols.size()));

		bool needToRecreate = false;
		if (vboData.size() != size) {
			vboData.resize(size);
			needToRecreate = true;
		}

		for (int i = 0; i < verts.size(); i++) {
			vboData[i].vert.x = verts[i].x;
			vboData[i].vert.y = verts[i].y;
			vboData[i].vert.z = verts[i].z;
		}
		 
		for (int i = 0; i < texCoords.size(); i++) {
			vboData[i].texCoord.u = texCoords[i].x;
			vboData[i].texCoord.v = texCoords[i].y;
		}

		for (int i = 0; i < cols.size(); i++) {
			vboData[i].col.r = cols[i].r;
			vboData[i].col.g = cols[i].g;
			vboData[i].col.b = cols[i].b;
			vboData[i].col.a = cols[i].a;
		}

		openGLContext->extensions.glBindVertexArray(VAO);
        openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        if(!loaded || needToRecreate) {
            openGLContext->extensions.glVertexAttribPointer(attribLocationPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VboData), (GLvoid*)offsetof(VboData, vert));
            openGLContext->extensions.glEnableVertexAttribArray(attribLocationPosition);

			openGLContext->extensions.glVertexAttribPointer(attribLocationUv, 2, GL_FLOAT, GL_FALSE, sizeof(VboData), (GLvoid*)offsetof(VboData, texCoord));
			openGLContext->extensions.glEnableVertexAttribArray(attribLocationUv);

			openGLContext->extensions.glVertexAttribPointer(attribLocationCol, 4, GL_FLOAT, GL_FALSE, sizeof(VboData), (GLvoid*)offsetof(VboData, col));
			openGLContext->extensions.glEnableVertexAttribArray(attribLocationCol);

			openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, size * sizeof(VboData), 0, usage);
			openGLContext->extensions.glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(VboData), vboData.data());

            needToRecreate = false;
			loaded = true;
		}
        else {
            openGLContext->extensions.glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(VboData), vboData.data());
		}

        openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		openGLContext->extensions.glBindVertexArray(0);
	}

	void internalDraw(GLuint drawMode, int first, int total) const {
		if (!inited) return;

		openGLContext->extensions.glBindVertexArray(VAO);
		openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glDrawArrays(drawMode, first, total);
		openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		openGLContext->extensions.glBindVertexArray(0);
	}
    
};

#else // Default version

class MurVbo {
public:
    MurVbo(){};
};

#endif

}
