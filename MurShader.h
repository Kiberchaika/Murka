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
	ofShader shader;
public:
	MurShader(){}
  
	bool load(std::string vert, std::string frag) {
		return shader.load(vert, frag);
	}

	int getAttributeLocation(std::string name) {
		return shader.getAttributeLocation(name);
	}

	void unload() {
		shader.unload();
	}

	void bind() {
		shader.begin();
	}

	void unbind() {
		shader.end();
	}

	void setUniform1i(std::string name, int v) {
		shader.setUniform1i(name, v);
	}

	void setUniform1f(std::string name, float v) {
		shader.setUniform1f(name, v);
	}

	void setUniform2f(std::string name, float v1, float v2) {
		shader.setUniform2f(name, v1, v2);
	}

	void setUniform3f(std::string name, float v1, float v2, float v3) {
		shader.setUniform3f(name, v1, v2, v3);
	}

	void setUniform4f(std::string name, float v1, float v2, float v3, float v4) {
		shader.setUniform4f(name, v1, v2, v3, v4);
	}

	void setUniformMatrix4f(std::string name, ofMatrix4x4 m) {
		shader.setUniformMatrix4f(name, m);
	}

};

#elif defined(MURKA_JUCE)

class MurShader {
	juce::OpenGLContext* openGLContext = nullptr;
	juce::OpenGLShaderProgram* shader = nullptr;
	std::map<std::string, juce::OpenGLShaderProgram::Uniform*> uniforms;
	std::map<std::string, int> attributes;
	
	bool isLoaded = false;

	juce::OpenGLShaderProgram::Uniform* getUniformByName(std::string name) {
		if (isLoaded) {
			if (uniforms.find(name) != uniforms.end()) {
				return uniforms[name];
			}
			else {
				if (openGLContext->extensions.glGetUniformLocation(shader->getProgramID(), name.c_str()) >= 0) {
					juce::OpenGLShaderProgram::Uniform* uniform = new juce::OpenGLShaderProgram::Uniform(*shader, name.c_str());
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
		shader = new juce::OpenGLShaderProgram(*openGLContext);

		if (
			shader->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vert)) &&
			shader->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(frag)) &&
			shader->link()
			) {
			isLoaded = true;
		}
		else {
			std::string err = shader->getLastError().toStdString();
			isLoaded = false;

			throw std::runtime_error("Error: " + err);
		}
	
		return isLoaded;
	}

	int getAttributeLocation(std::string name) {
		if (isLoaded) {
			if (attributes.find(name) != attributes.end()) {
				return attributes[name];
			}
			else {
				int loc = openGLContext->extensions.glGetAttribLocation(shader->getProgramID(), name.c_str());
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

		delete shader;
		shader = nullptr;

		isLoaded = false;
	}

	void use() {
		shader->use();
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
