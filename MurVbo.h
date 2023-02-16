#pragma once

#include "MurkaTypes.h"

namespace murka {

#if defined(MURKA_OF)


class MurVbo {
    vector<MurkaPoint3D> verts;
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
	
	void setVertexData(MurkaPoint3D* _verts, int total) {
        this->verts.resize(total);
        memcpy(this->verts.data(), _verts, total * sizeof(MurkaPoint3D));
	}

    void setTexCoordData(MurkaPoint* texCoords, int total) {
        this->texCoords.resize(total);
        memcpy(this->texCoords.data(), texCoords, total * sizeof(MurkaPoint));
    }
    
    void update(int usage) {
		this->usage = usage;
		if (this->verts.size()) internal.setVertexData(&this->verts[0].x, 3, this->verts.size(), this->usage, sizeof(MurkaPoint3D));
		if (this->texCoords.size()) internal.setTexCoordData(&this->texCoords[0].x, this->texCoords.size(), this->usage, sizeof(MurkaPoint));
	}
    
    void clear() {
        internal.clear();
    }

	void internalDraw(GLuint drawMode, int first, int total) const {
		internal.draw(drawMode, first, total);
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

	std::vector<MurkaPoint3D> verts;
	std::vector<MurkaPoint> texCoords;
	std::vector<MurkaColor> colors;
	
	std::vector<unsigned int> indexes;

	struct VboData {
        VertexCoord vert;
		UVCoord texCoord;
		Col col;
	};

	std::vector<VboData> vboData;

    GLuint VAO = 0;
	GLuint VBO = 0;
	GLuint VBOID = 0;
	
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
		openGLContext->extensions.glGenBuffers(1, &VBOID);

        inited = true;
	}

	void clear() {
        if(inited) {
            openGLContext->extensions.glDeleteVertexArrays(1, &VAO);
            openGLContext->extensions.glDeleteBuffers(1, &VBO);
			openGLContext->extensions.glDeleteBuffers(1, &VBOID);

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

	void setColorData(const MurkaColor* colors, int total) {
		this->colors.resize(total);
		memcpy(this->colors.data(), colors, total * sizeof(MurkaColor));
	}

	void setIndexData(const unsigned int* indexes, int total) {
		this->indexes.resize(total);
		memcpy(this->indexes.data(), indexes, total * sizeof(unsigned int));
	}

	std::vector<MurkaPoint3D>& getVertices() {
		return verts;
	}

	std::vector<MurkaPoint>& getTexCoords() {
		return texCoords;
	}

	std::vector<MurkaColor>& getColors() {
		return colors;
	}

	std::vector<unsigned int>& getIndexes() {
		return indexes;
	}

	void addVertex(MurkaPoint3D vert) {
		this->verts.push_back(vert);
	}

	void addTexCoord(MurkaPoint texCoord) {
		this->texCoords.push_back(texCoord);
	}

	void addColor(MurkaColor col) {
		this->colors.push_back(col);
	}

	void addIndex(unsigned int index) {
		this->indexes.push_back(index);
	}

	void update(int usage, int attribLocationPosition, int attribLocationUv, int attribLocationCol) {
		if (!inited) return;

		int size = (std::max)(verts.size(), (std::max)(texCoords.size(), colors.size()));

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

		for (int i = 0; i < colors.size(); i++) {
			vboData[i].col.r = colors[i].r;
			vboData[i].col.g = colors[i].g;
			vboData[i].col.b = colors[i].b;
			vboData[i].col.a = colors[i].a;
		}

		openGLContext->extensions.glBindVertexArray(VAO);

		if (indexes.size() > 0) {
			openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOID);
			openGLContext->extensions.glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexes.size() * sizeof(unsigned int), indexes.data(), usage);
		}

        openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, VBO);
        
        if(!loaded || needToRecreate) {
            openGLContext->extensions.glVertexAttribPointer(attribLocationPosition, 3, GL_FLOAT, GL_FALSE, sizeof(VboData), (GLvoid*)offsetof(VboData, vert));
            openGLContext->extensions.glEnableVertexAttribArray(attribLocationPosition);

			openGLContext->extensions.glVertexAttribPointer(attribLocationUv, 2, GL_FLOAT, GL_FALSE, sizeof(VboData), (GLvoid*)offsetof(VboData, texCoord));
			openGLContext->extensions.glEnableVertexAttribArray(attribLocationUv);

			openGLContext->extensions.glVertexAttribPointer(attribLocationCol, 4, GL_FLOAT, GL_FALSE, sizeof(VboData), (GLvoid*)offsetof(VboData, col));
			openGLContext->extensions.glEnableVertexAttribArray(attribLocationCol);

			openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, size * sizeof(VboData), vboData.data(), usage);
			//openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, size * sizeof(VboData), 0, usage);
			//openGLContext->extensions.glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(VboData), vboData.data());

            needToRecreate = false;
			loaded = true;
		}
        else {
			openGLContext->extensions.glBufferData(GL_ARRAY_BUFFER, size * sizeof(VboData), vboData.data(), usage);
            //openGLContext->extensions.glBufferSubData(GL_ARRAY_BUFFER, 0, size * sizeof(VboData), vboData.data());
		}

		openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		openGLContext->extensions.glBindVertexArray(0);
	}

	void internalDraw(GLuint drawMode, int first, int total) const {
		if (!inited) return;

		openGLContext->extensions.glBindVertexArray(VAO);
		openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, VBO);
		if (indexes.size() > 0) {
			openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOID);
		}
		if (indexes.size() > 0) {
			glDrawElements(drawMode, total, GL_UNSIGNED_INT, 0); // ? , first);
		}
		else {
			glDrawArrays(drawMode, first, total);
		}
		
		openGLContext->extensions.glBindBuffer(GL_ARRAY_BUFFER, 0);
		if (indexes.size() > 0) {
			openGLContext->extensions.glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}
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
