#pragma once

#include "MurkaTypes.h"
#include "MurMatrix.h"

#if defined(MURKA_OF)
#include "ofMain.h"
#elif defined(MURKA_JUCE) && !defined(WIN32)
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#endif

namespace murka {

class MurkaRenderer;


#if defined(MURKA_OF)

class MurShader {
public:
	MurShader(){}
    
};

#elif defined(MURKA_JUCE)

class MurShader {
	juce::OpenGLContext* openGLContext = nullptr;
	juce::OpenGLShaderProgram* shaderMain = nullptr;
	std::map<std::string, juce::OpenGLShaderProgram::Uniform*> uniforms;
	std::map<std::string, int> attributes;
	
	bool isLoaded = false;

	juce::OpenGLShaderProgram::Uniform* getUniformByName(std::string name) {
		if (isLoaded) {
			if (uniforms.find(name) != uniforms.end()) {
				return uniforms[name];
			}
			else {
				if (openGLContext->extensions.glGetUniformLocation(shaderMain->getProgramID(), name.c_str()) >= 0) {
					juce::OpenGLShaderProgram::Uniform* uniform = new juce::OpenGLShaderProgram::Uniform(*shaderMain, name.c_str());
					uniforms[name] = uniform;
					return uniform;
				}
			}
		}
		return nullptr;
	}


public:
	MurShader() {

	};
	
	~MurShader() {
		unload();
	};

	void setOpenGLContext(juce::OpenGLContext* ctx) {
		openGLContext = ctx;
	}

	bool load(std::string vert, std::string frag) {
		shaderMain = new juce::OpenGLShaderProgram(*openGLContext);

		if (
			shaderMain->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vert)) &&
			shaderMain->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(frag)) &&
			shaderMain->link()
			) {
			isLoaded = true;
		}
		else {
			string err = shaderMain->getLastError().toStdString();
			isLoaded = false;
		}
	
		return isLoaded;
	}

	int getAttributeLocation(std::string name) {
		if (isLoaded) {
			if (attributes.find(name) != attributes.end()) {
				return attributes[name];
			}
			else {
				int loc = openGLContext->extensions.glGetAttribLocation(shaderMain->getProgramID(), name.c_str());
				attributes[name] = loc;
				return loc;
			}
		}
		return -1;
	}

	void unload() {
		for (auto it = uniforms.begin(); it != uniforms.end(); it++) {
			delete it->second;
		}
		uniforms.clear();

		delete shaderMain;
		shaderMain = nullptr;

		isLoaded = false;
	}

	void bind() {
		shaderMain->use();
	}

	void setUniform1i(std::string name, int v) {
		juce::OpenGLShaderProgram::Uniform* uniform = getUniformByName(name);
		if (uniform) {
			uniform->set(v);
		}
	}

	void setUniform1f(std::string name, float v) {
		juce::OpenGLShaderProgram::Uniform* uniform = getUniformByName(name);
		if (uniform) {
			uniform->set(v);
		}
	}

	void setUniform2f(std::string name, float v1, float v2) {
		juce::OpenGLShaderProgram::Uniform* uniform = getUniformByName(name);
		if (uniform) {
			uniform->set(v1, v2);
		}
	}

	void setUniform3f(std::string name, float v1, float v2, float v3) {
		juce::OpenGLShaderProgram::Uniform* uniform = getUniformByName(name);
		if (uniform) {
			uniform->set(v1, v2, v3);
		}
	}

	void setUniform4f(std::string name, float v1, float v2, float v3, float v4) {
		juce::OpenGLShaderProgram::Uniform* uniform = getUniformByName(name);
		if (uniform) {
			uniform->set(v1, v2, v3, v4);
		}
	}

	void setUniformMatrix4f(std::string name, MurMatrix<float> m) {
		juce::OpenGLShaderProgram::Uniform* uniform = getUniformByName(name);
		if (uniform) {
			uniform->setMatrix4((GLfloat*)&(m.mat[0]), 1, false);
		}
	}

};

#else // Default version
#endif

}
