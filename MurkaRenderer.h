#pragma once

#include "MurkaTypes.h"
#include "MurkaRendererBase.h"
#include "MurImage.h"
#include "MurVbo.h"
#include "MurMatrix.h"


#if defined(MURKA_OF)
#include "ofMain.h"
#elif defined(MURKA_JUCE) && !defined(WIN32)
#include <GLUT/glut.h>
#include <OpenGL/OpenGL.h>
#endif

namespace murka {

#if defined(MURKA_OF)


class MurkaRenderer: public MurkaRendererBase {
	ofAppBaseWindow* ofWindow = nullptr;
    ofBaseGLRenderer* ofRenderer = nullptr;
    
	std::function<std::string(void)> getClipboardCallback = nullptr;
	std::function<void(std::string)> setClipboardCallback = nullptr;

public:
	void setWindow(ofAppBaseWindow* window) {
		ofWindow = window;
	}

	void setRenderer(ofBaseGLRenderer* renderer) {
		ofRenderer = renderer;
	}

	// Clipboard
	void setClipboardCallbacks(std::function<std::string(void)> getCallback, std::function<void(std::string)> setCallback) {
		getClipboardCallback = getCallback;
		setClipboardCallback = setCallback;
	}

	void setClipboardString(std::string clipboard) {
		if (setClipboardCallback) setClipboardCallback(clipboard);  
	} 

	std::string getClipboardString() {
		return getClipboardCallback ? getClipboardCallback() : "";
	}

	// This font loading function needs to be here because
    // downcasting from Renderer to MurkaAssets doesn't work for some reason.
    // TODO: figure a better way to make setFont() accessible like this.
    
    void setFont(std::string name, int size) {
        MurkaAssets::setFont(name, size * getScreenScale(), this);
    }

	void drawString(const string & s, float x, float y) {
		MurkaAssets::getCurrentFont()->drawString(s, x, y);
	} 

	// Object drawing
	void draw(const MurImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
		ofRenderer->draw(image.internal, x * getScreenScale(), y * getScreenScale(), z * getScreenScale(), w * getScreenScale(), h * getScreenScale(), sx * getScreenScale(), sy * getScreenScale(), sw * getScreenScale(), sh * getScreenScale());
	}

	void draw(const MurImage & image, float x, float y, float z, float w, float h) override {
		ofRenderer->draw(image.internal, x * getScreenScale(), y * getScreenScale(), z * getScreenScale(), w * getScreenScale(), h * getScreenScale(), 0, 0, image.internal.getWidth(), image.internal.getHeight());
	}

	void draw(const MurImage & image, float x, float y) override {
		ofRenderer->draw(image.internal, x * getScreenScale(), y * getScreenScale(), 0, image.internal.getWidth() * getScreenScale(), image.internal.getHeight() * getScreenScale(), 0, 0, image.internal.getWidth() * getScreenScale(), image.internal.getHeight() * getScreenScale());
	}

	void draw(const MurTexture & texture, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
        ofRenderer->draw(texture.internal, x * getScreenScale(), y * getScreenScale(), z * getScreenScale(), w * getScreenScale(), h * getScreenScale(), sx * getScreenScale(), sy * getScreenScale(), sw * getScreenScale(), sh * getScreenScale());
    }
    
    void draw(const MurVbo & vbo, GLuint drawMode, int first, int total) override{
        ofRenderer->draw(vbo.internal, drawMode, first, total);
    }
    
    // Textures binding
	void bind(const MurImage & img, int location = 0) override {
		ofRenderer->bind(img.internal.getTexture(), location);
	}

	void unbind(const MurImage & img, int location = 0) override {
		ofRenderer->unbind(img.internal.getTexture(), location);
	}

	void bind(const MurTexture & texture, int location = 0) override {
		ofRenderer->bind(texture.internal, location);
	}

	void unbind(const MurTexture & texture, int location = 0) override {
		ofRenderer->unbind(texture.internal, location);
	}

	// transformations
	void pushView() override {
		ofRenderer->pushView();
	}

	void popView() override {
		ofRenderer->popView();
	}

	void pushMatrix() override {
		ofRenderer->pushMatrix();
	}

	void popMatrix() override {
		ofRenderer->popMatrix();
	}

	void translate(float x, float y, float z)  override {
		ofRenderer->translate(x * getScreenScale(), y * getScreenScale(), z * getScreenScale());
	}
    
    void rotateZRad(float radians) override {
        ofRenderer->rotateZRad(radians);
    }

	void scale(float x, float y, float z) override {
		ofRenderer->scale(x * getScreenScale(), y * getScreenScale(), z * getScreenScale());
	}

    // setup matrices and viewport (upto you to push and pop view before and after)
    void viewport(MurkaShape viewport) override {
        ofRenderer->viewport(viewport * getScreenScale());
    }
    
    void viewport(float x = 0, float y = 0, float width = -1, float height = -1, bool vflip=true) override {
        ofRenderer->viewport(x * getScreenScale(), y * getScreenScale(), width * getScreenScale(), height * getScreenScale(), vflip);
    }

	MurkaShape getCurrentViewport() override {
		ofRectangle rect = ofRenderer->getCurrentViewport();
		return MurkaShape(rect.x, rect.y, rect.width, rect.height) / getScreenScale();
	}

	void setupScreen() override {
		ofRenderer->setupScreen();
	}

    // rendering setup
    void setCircleResolution(int resolution) override{
        ofRenderer->setCircleResolution(resolution);
    }
    
    void setLineWidth(float lineWidth) override{
        ofRenderer->setLineWidth(lineWidth);
    };
    
    void enableFill() override{
        ofRenderer->setFillMode(OF_FILLED);
    };
    
    void disableFill() override{
        ofRenderer->setFillMode(OF_OUTLINE);
    };
    
    void setLineSmoothing(bool smooth) override{
        ofRenderer->setLineSmoothing(smooth);
    };
    
    void enableAntiAliasing() override{
        ofRenderer->enableAntiAliasing();
    };
    
    void disableAntiAliasing() override{
        ofRenderer->disableAntiAliasing();
    };
    
    void enableAlphaBlending() override{
        ofRenderer->setBlendMode(OF_BLENDMODE_ALPHA);
    };
    
    void disableAlphaBlending() override{
        ofRenderer->setBlendMode(OF_BLENDMODE_DISABLED);
    };
    
    void pushStyle() override{
        ofRenderer->pushStyle();
    };
    
    void popStyle() override{
        ofRenderer->popStyle();
    };
    
    // color operations
    MurkaColor getColor() override {
        auto color = ofRenderer->getStyle().color;
        return {float(color.r / 255.),
                float(color.g / 255.),
                float(color.b / 255.),
                float(color.a / 255.)};
    }
    
    void setColor(int r, int g, int b) override{
        ofRenderer->setColor(r, g, b);
    }; // 0-255
    
    void setColor(int r, int g, int b, int a) override{
        ofRenderer->setColor(r, g, b, a);
    }; // 0-255
    
    void setColor(const MurkaColor & color) override{
        ofRenderer->setColor(color);
    };
    
    void setColor(const MurkaColor & color, int _a) override{
        ofRenderer->setColor(color, _a);
    };
    
    void setColor(int gray) override{
        ofRenderer->setColor(gray);
    }; // 0 - 255
    
    void setColor(int gray, int _a) override {
        ofRenderer->setColor(gray, _a);
    }; // 0-255


    void clear() override{
        ofRenderer->clear();
    };
    
	void clear(int r, int g, int b, int a = 0) override {
		ofRenderer->clear(r / 255., g / 255., b / 255., a / 255.);
	};

	void clear(int brightness, int a = 0) override {
		ofRenderer->clear(brightness / 255., a / 255.);
	};

	void drawRectangle(float x, float y, float w, float h) override {
		ofRenderer->drawRectangle(x * getScreenScale(), y * getScreenScale(), 0.0f, w * getScreenScale(), h * getScreenScale());
	}
    
    void drawRectangle(MurkaShape s) override {
        ofRenderer->drawRectangle(s.position.x * getScreenScale(), s.position.y * getScreenScale(), 0., s.size.x * getScreenScale(), s.size.y * getScreenScale());
    }
    

	void drawCircle(float x, float y, float radius) override {
		ofRenderer->drawCircle(x * getScreenScale(), y * getScreenScale(), 0.0f, radius * getScreenScale());
	}

	void drawLine(float x1, float y1, float x2, float y2) override {
		ofRenderer->drawLine(x1 * getScreenScale(), y1 * getScreenScale(), 0.0f, x2 * getScreenScale(), y2 * getScreenScale(), 0.0f);
	}

	void drawVbo(const MurVbo & vbo, GLuint drawMode, int first, int total) override {
        draw(vbo, drawMode, first, total);
    }

	void drawPath(const vector<MurkaPoint> & verts) override {
		ofPolyline polyline;
		polyline.addVertices(*(const vector<glm::vec3>*)&verts);
		ofRenderer->draw(polyline);
	}

	int getWindowWidth() override {
		return ofWindow->getWidth() / getScreenScale();
	}

	int getWindowHeight() override {
		return ofWindow->getHeight() / getScreenScale();
	}

	bool getUsingArbTex() {
		return ofGetUsingArbTex();
	}

	void disableArbTex() {
		ofDisableArbTex();
	}

	void enableArbTex() {
		ofEnableArbTex();
	}

	uint64_t getFrameNum() {
		return ofGetFrameNum();
	}

	float getElapsedTime() {
		return ofGetElapsedTimef();
	}

};


#elif defined(MURKA_JUCE)

class MurkaRenderer : public MurkaRendererBase {

	juce::OpenGLAppComponent* glAppComp = nullptr;
	juce::OpenGLContext* openGLContext = nullptr;

	std::function<std::string(void)> getClipboardCallback = nullptr;
	std::function<void(std::string)> setClipboardCallback = nullptr;

	vector<MurMatrix<float>> matrixStack;
	MurMatrix<float> currentMatrix;

	vector<MurkaShape> viewportStack;
	MurkaShape currentViewport;

	struct Style {
		MurkaColor color = MurkaColor(1.0f, 1.0f, 1.0f, 1.0f);
		bool fill = true;
	};
	vector<Style> styleStack;
	Style currentStyle;

	float lineWidth = 1;
	float circleResolution = 32;

	MurMatrix<float> stackedMatrix;

	MurMatrix<float> projMatrix;
	MurMatrix<float> viewMatrix;

	std::unique_ptr<juce::OpenGLShaderProgram> shaderMain;
	juce::OpenGLShaderProgram::Uniform* uniformMatrixModel = nullptr;
	juce::OpenGLShaderProgram::Uniform* uniformMatrixView = nullptr;
	juce::OpenGLShaderProgram::Uniform* uniformMatrixProj = nullptr;
	juce::OpenGLShaderProgram::Uniform* uniformColor = nullptr;
	juce::OpenGLShaderProgram::Uniform* uniformVFlip = nullptr;
	juce::OpenGLShaderProgram::Uniform* uniformUseTexture = nullptr;
	
	MurVbo vboRect;
	MurVbo vboLine;
	MurVbo vboLineOld;
	MurVbo vboCircle;

	void recreateCircleVbo() {
		vector<MurkaPoint> verts;
		for (int i = 0; i < circleResolution; i++)
		{
			float theta = 2.0f * juce::MathConstants<float>::pi * float(i) / float(circleResolution);
			float x = 1.0 * cosf(theta);
			float y = 1.0 * sinf(theta);
			verts.push_back(MurkaPoint(x, y));
		}

		vboCircle.setOpenGLContext(openGLContext);
		vboCircle.setup();
		vboCircle.setVertexData(verts.data(), verts.size());
		vboCircle.update(GL_STREAM_DRAW, attribLocationPosition, attribLocationUv);
	}

	void updateStackedMatrix() {
		stackedMatrix = matrixStack.size() == 0 ? MurMatrix<float>() : matrixStack[0];
		for (int i = 1; i < matrixStack.size(); i++) {
			stackedMatrix = matrixStack[1] * stackedMatrix;
		}
	}

	bool vflip = true;
	bool useTexture = false;
	
    int attribLocationPosition = 0;
    int attribLocationUv = 1;
    
	uint64_t frameNum = 0;
	std::chrono::steady_clock::time_point begin;

public:
	~MurkaRenderer() {
		delete uniformMatrixModel;
		delete uniformMatrixView;
		delete uniformMatrixProj;
		delete uniformColor;
		delete uniformVFlip;
		delete uniformUseTexture;

		uniformMatrixModel = nullptr;
		uniformMatrixView = nullptr;
		uniformMatrixProj = nullptr;
		uniformColor = nullptr;
		uniformVFlip = nullptr;
		uniformUseTexture = nullptr;
	}

	void setAppComponent(juce::OpenGLAppComponent* comp) {
		glAppComp = comp;
	}

	void setOpenGLContext(juce::OpenGLContext* ctx) {
		openGLContext = ctx;
	}
    
    int getAttribLocationPosition() {
        return attribLocationPosition;
    }
    
    int getAttribLocationUv() {
        return attribLocationUv;
    }

	juce::OpenGLContext* getOpenGLContext() {
		return openGLContext;
	}

	void setup() {
        // create main shader
        {
            string vertexShader =
                "varying vec2 vUv;\n"
                "uniform mat4 matrixModel;\n"
                "uniform mat4 matrixView;\n"
                "uniform mat4 matrixProj;\n"
                "uniform vec4 color;\n"
                "uniform bool vflip;\n"
                "attribute vec3 position;\n"
                "attribute vec2 uv;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    vUv = uv;"
                "    vec4 pos = matrixProj * matrixView * matrixModel * vec4(position, 1.0) ; \n"
                "    gl_Position = vflip ? vec4(pos.x, 1 - pos.y, pos.z, pos.w) : vec4(position, 1.0); \n"
                "}\n";

            string fragmentShader =
                "varying vec2 vUv;\n"
                "uniform sampler2D mainTexture;\n"
                "uniform vec4 color;\n"
                "uniform bool useTexture;\n"
                "\n"
                "void main()\n"
                "{\n"
                "    gl_FragColor = color * (useTexture ? texture(mainTexture, vUv) : vec4 (1.0, 1.0, 1.0, 1.0));\n"
                "}\n";

            shaderMain = std::make_unique<juce::OpenGLShaderProgram>(*openGLContext);
            if (shaderMain->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vertexShader)) &&
                shaderMain->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(fragmentShader)) &&
                shaderMain->link()) {
                //if (openGLContext->extensions.glGetUniformLocation(shaderProgram.getProgramID(), "name"))
                uniformMatrixModel = new juce::OpenGLShaderProgram::Uniform(*shaderMain, "matrixModel");
                uniformMatrixView = new juce::OpenGLShaderProgram::Uniform(*shaderMain, "matrixView");
                uniformMatrixProj = new juce::OpenGLShaderProgram::Uniform(*shaderMain, "matrixProj");
                uniformColor = new juce::OpenGLShaderProgram::Uniform(*shaderMain, "color");
                uniformVFlip = new juce::OpenGLShaderProgram::Uniform(*shaderMain, "vflip");
                uniformUseTexture = new juce::OpenGLShaderProgram::Uniform(*shaderMain, "useTexture");
            
                
                GLuint programID = shaderMain->getProgramID();
				attribLocationPosition = openGLContext->extensions.glGetAttribLocation(programID, "position");
				attribLocationUv = openGLContext->extensions.glGetAttribLocation(programID, "uv");
            }
            else {
                string err = shaderMain->getLastError().toStdString();
                err = err + "";
            }
        }

        // vbo for primitives
		{
			vector<MurkaPoint> verts;
			verts.push_back(MurkaPoint(0.0f, 0.0f));
			verts.push_back(MurkaPoint(0.0f, 1.0f));
			verts.push_back(MurkaPoint(1.0f, 1.0f));
			verts.push_back(MurkaPoint(1.0f, 0.0f));

			vector<MurkaPoint> texCoords;
			texCoords.push_back(MurkaPoint(0.0f, 0.0f));
			texCoords.push_back(MurkaPoint(0.0f, 1.0f));
            texCoords.push_back(MurkaPoint(1.0f, 1.0f));
			texCoords.push_back(MurkaPoint(1.0f, 0.0f));

			vboRect.setOpenGLContext(openGLContext);
			vboRect.setup();
			vboRect.setVertexData(verts.data(), verts.size());
			vboRect.setTexCoordData(texCoords.data(), texCoords.size());
			vboRect.update(GL_STATIC_DRAW, attribLocationPosition, attribLocationUv);
		}

		{
			vector<MurkaPoint> verts;
			verts.push_back(MurkaPoint(0.0f, -0.5f));
			verts.push_back(MurkaPoint(0.0f, 0.5f));
			verts.push_back(MurkaPoint(1.0f, 0.5f));
			verts.push_back(MurkaPoint(1.0f, -0.5f));

            vector<MurkaPoint> texCoords;
            texCoords.push_back(MurkaPoint(0.0f, 0.0f));
            texCoords.push_back(MurkaPoint(0.0f, 1.0f));
            texCoords.push_back(MurkaPoint(1.0f, 1.0f));
            texCoords.push_back(MurkaPoint(1.0f, 0.0f));

			vboLine.setOpenGLContext(openGLContext);
			vboLine.setup();
			vboLine.setVertexData(verts.data(), verts.size());
            vboLine.setTexCoordData(texCoords.data(), texCoords.size());
            vboLine.update(GL_STREAM_DRAW, attribLocationPosition, attribLocationUv);
		}

		{
			vector<MurkaPoint> verts;
			verts.push_back(MurkaPoint(0.0f, 0.0f));
			verts.push_back(MurkaPoint(0.0f, 0.0f));

            vector<MurkaPoint> texCoords;
            texCoords.push_back(MurkaPoint(0.0f, 0.0f));
            texCoords.push_back(MurkaPoint(1.0f, 1.0f));

			vboLineOld.setOpenGLContext(openGLContext);
			vboLineOld.setup();
			vboLineOld.setVertexData(verts.data(), verts.size());
            vboLineOld.setTexCoordData(texCoords.data(), texCoords.size());
            vboLineOld.update(GL_STREAM_DRAW, attribLocationPosition, attribLocationUv);
		}

		{
			recreateCircleVbo();
		}

        currentViewport = MurkaShape(0, 0, glAppComp->getWidth(), glAppComp->getHeight());

        begin = std::chrono::steady_clock::now();
	}

	void startFrame() {
		// clear matrix stack
		matrixStack.clear();
		currentMatrix = MurMatrix<float>();

		styleStack.clear();
		currentStyle = Style();

		viewportStack.clear();
		currentViewport = MurkaShape(0, 0, glAppComp->getWidth(), glAppComp->getHeight());

		frameNum++;

		//enableAlphaBlending();
	}
	
	// Clipboard
	void setClipboardCallbacks(std::function<std::string(void)> getCallback, std::function<void(std::string)> setCallback) {
		getClipboardCallback = getCallback;
		setClipboardCallback = setCallback;
	}

	void setClipboardString(std::string clipboard) {
		if (setClipboardCallback) setClipboardCallback(clipboard);
	}

	std::string getClipboardString() {
		return getClipboardCallback ? getClipboardCallback() : "";
	}

	// This font loading function needs to be here because
	// downcasting from Renderer to MurkaAssets doesn't work for some reason.
	// TODO: figure a better way to make setFont() accessible like this.

	void setFont(std::string name, int size) {
		MurkaAssets::setFont(name, size * getScreenScale(), this);
	}

	void drawString(const string & s, float x, float y) {
		MurkaAssets::getCurrentFont()->drawString(s, x, y);// *getScreenScale(), y * getScreenScale()); FIX THIS
	}

	// Object drawing
	void draw(const MurImage & image, float x, float y, float z, float w, float h) override {
		bind(image, 0);
		drawRectangle(x, y, w, h);
		unbind(image, 0);
	}
	
	void draw(const MurImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
		std::cout << "TODO" << std::endl;
	}

	void draw(const MurImage & image, float x, float y) override {
		bind(image, 0);
		drawRectangle(x, y, image.getWidth(), image.getHeight());
		unbind(image, 0);
	}

	void draw(const MurTexture & texture, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
		std::cout << "TODO" << std::endl;
	}
	
	void draw(const MurVbo & vbo, GLuint drawMode, int first, int total) override {
		MurMatrix<float> modelMatrix;
		modelMatrix = modelMatrix * stackedMatrix * currentMatrix;

		shaderMain->use();
		uniformMatrixModel->setMatrix4((GLfloat*)&(modelMatrix.mat[0]), 1, false);
		uniformMatrixView->setMatrix4((GLfloat*)&(viewMatrix.mat[0]), 1, false);
		uniformMatrixProj->setMatrix4((GLfloat*)&(projMatrix.mat[0]), 1, false);
		uniformUseTexture->set(useTexture);
		uniformVFlip->set(vflip);
		uniformColor->set(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
		vbo.internalDraw(drawMode, first, total);
	}

	// Textures binding
	void bind(const MurImage & img, int location = 0) override {
		useTexture = true;
		openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
		img.bind();
	}

	void unbind(const MurImage & img, int location = 0) override {
		useTexture = false;
		openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
		img.unbind();
	}
	
	void bind(const MurTexture & texture, int location = 0) override {
		useTexture = true;
		openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
		texture.bind();
	}

	void unbind(const MurTexture & texture, int location = 0) override {
		useTexture = false;
		openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
		texture.unbind();
	}

	void pushView() override {
		viewportStack.push_back(currentViewport);
	}

	void popView() override {
		MurkaShape v = viewportStack.back() / getScreenScale();
		viewport(v);
		setupScreen();
		viewportStack.pop_back();
	}

	void pushMatrix() override {
		matrixStack.push_back(currentMatrix);
		currentMatrix = MurMatrix<float>();
		updateStackedMatrix();
	}

	void popMatrix() override {
		currentMatrix = matrixStack.back();
		matrixStack.pop_back();
		updateStackedMatrix();
	}

	void translate(float x, float y, float z) override {
		currentMatrix = MurMatrix<float>::translation(juce::Vector3D<float>(x * getScreenScale(), y * getScreenScale(), z * getScreenScale())) * currentMatrix;
	}

	void rotateZRad(float radians) override {
		currentMatrix = MurMatrix<float>().rotated(juce::Vector3D<float>(0.0, 0.0, radians)) * currentMatrix;
	}

	void scale(float x, float y, float z) override {
		currentMatrix = MurMatrix<float>().scaled(juce::Vector3D<float>(x * getScreenScale(), y * getScreenScale(), z * getScreenScale())) * currentMatrix;
	}

	// setup matrices and viewport (upto you to push and pop view before and after)
	void viewport(MurkaShape viewport) override {
		this->viewport(viewport.x(), viewport.y(), viewport.width(), viewport.height(), vflip);
	}

	void viewport(float x, float y, float width, float height, bool vflip = true) override {
		currentViewport = MurkaShape(x * getScreenScale(), y * getScreenScale(), width * getScreenScale(), height * getScreenScale());
		glViewport(currentViewport.x(), vflip ? glAppComp->getHeight() - currentViewport.height() - currentViewport.y() : currentViewport.y(), currentViewport.width(), currentViewport.height());
	}

	MurkaShape getCurrentViewport() override {
		return currentViewport;
	}

	void setupScreen() override {
		float viewW = currentViewport.width();
		float viewH = currentViewport.height();

		float fov = 60;
		float eyeX = viewW / 2;
		float eyeY = viewH / 2;
		float halfFov = juce::MathConstants<float>::pi * fov / 360;
		float theTan = tanf(halfFov);
		float dist = eyeY / theTan;
		float aspect = (float)viewW / viewH;

		float nearDist = dist / 10.0f;
		float farDist = dist * 10.0f;

		projMatrix = MurMatrix<float>::fromPerspective(juce::degreesToRadians(fov), aspect, nearDist, farDist);
		viewMatrix = MurMatrix<float>::fromLookAt(juce::Vector3D<float>(eyeX, eyeY, dist), juce::Vector3D<float>(eyeX, eyeY, 0), juce::Vector3D<float>(0, 1, 0));
	}

	// rendering setup
	void setCircleResolution(int resolution) override {
		this->circleResolution = resolution;
		recreateCircleVbo();
	}

	void setLineWidth(float lineWidth) override {
		this->lineWidth = lineWidth;
	};

	void enableFill() override {
		currentStyle.fill = true;
		glEnable(GL_FILL);
	};

	void disableFill() override {
		currentStyle.fill = false;
		glDisable(GL_FILL);
	};

	void setLineSmoothing(bool smooth) override {
		glEnable(GL_LINE_SMOOTH);
	};

	void enableAntiAliasing() override {
		glEnable(GL_POLYGON_SMOOTH);
		glEnable(GL_POINT_SMOOTH);
	};

	void disableAntiAliasing() override {
		glDisable(GL_POLYGON_SMOOTH);
		glDisable(GL_POINT_SMOOTH);
	};

	void enableAlphaBlending() override {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	};

	void disableAlphaBlending() override {
		glDisable(GL_BLEND);
	};

	void pushStyle() override {
		styleStack.push_back(currentStyle);
	};

	void popStyle() override {
		currentStyle = styleStack.back();
		styleStack.pop_back();
	};

	// color operations
	MurkaColor getColor() override {
		return currentStyle.color;
	}
	 
	void setColor(int r, int g, int b) override {
		currentStyle.color = MurkaColor(1.0 * r / 255, 1.0 * g / 255, 1.0 * b / 255);
		glColor4f(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
	}; // 0-255

	void setColor(int r, int g, int b, int a) override {
		currentStyle.color = MurkaColor(1.0 * r / 255, 1.0 * g / 255, 1.0 * b / 255, 1.0 * a / 255);
		glColor4f(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
	}; // 0-255

	void setColor(const MurkaColor & color) override {
		currentStyle.color = MurkaColor(color.r, color.g, color.b, color.a);
		glColor4f(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
	};

	void setColor(const MurkaColor & color, int _a) override {
		currentStyle.color = MurkaColor(color.r, color.g, color.b, 1.0 * _a / 255);
		glColor4f(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
	};

	void setColor(int gray) override {
		currentStyle.color = MurkaColor(1.0 * gray / 255, 1.0 * gray / 255, 1.0 * gray / 255);
		glColor4f(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
	}; // 0 - 255

	void setColor(int gray, int _a) override {
		currentStyle.color = MurkaColor(1.0 * gray / 255, 1.0 * gray / 255, 1.0 * gray / 255, 1.0 * _a / 255);
		glClearColor(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
	}; // 0-255


	void clear() override {
		glClearColor(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
		glClear(GL_COLOR_BUFFER_BIT);
	};

	void clear(int r, int g, int b, int a = 0) override {
		MurkaColor col(1.0 * r / 255, 1.0 * g / 255, 1.0 * b / 255, 1.0 * a / 255);
		glClearColor(col.r, col.g, col.b, col.a);
		glClear(GL_COLOR_BUFFER_BIT);
	};

	void clear(int gray, int a = 0) override {
		MurkaColor col(1.0 * gray / 255, 1.0 * gray / 255, 1.0 * gray / 255, 1.0 * a / 255);
		glClearColor(col.r, col.g, col.b, col.a);
		glClear(GL_COLOR_BUFFER_BIT);
	};

	void drawRectangle(float x, float y, float w, float h) override {
		if (currentStyle.fill) {
			MurMatrix<float> modelMatrix;
			modelMatrix = modelMatrix * MurMatrix<float>::translation(juce::Vector3D<float>(x * getScreenScale(), y * getScreenScale(), 0.0));
			modelMatrix = modelMatrix.scaled(juce::Vector3D<float>(w * getScreenScale(), h * getScreenScale(), 1.0));

			modelMatrix = modelMatrix * stackedMatrix * currentMatrix;

			shaderMain->use();
			uniformMatrixModel->setMatrix4((GLfloat*)&(modelMatrix.mat[0]), 1, false);
			uniformMatrixView->setMatrix4((GLfloat*)&(viewMatrix.mat[0]), 1, false);
			uniformMatrixProj->setMatrix4((GLfloat*)&(projMatrix.mat[0]), 1, false);
			uniformUseTexture->set(useTexture);
			uniformVFlip->set(vflip);
			uniformColor->set(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
			vboRect.update(GL_STATIC_DRAW, attribLocationPosition, attribLocationUv);
			vboRect.internalDraw(GL_TRIANGLE_FAN, 0, 4);
		}
		else {
			pushMatrix();
			translate(x, y, 0.0);
			scale(w, h, 1.0);
			draw(vboRect, GL_LINE_LOOP, 0, 4);
			popMatrix();
		}
	}

	void drawRectangle(MurkaShape s) override {
		drawRectangle(s.position.x, s.position.y, s.size.x, s.size.y);
	}

	void drawCircle(float x, float y, float radius) override {
		if (currentStyle.fill) {
			MurMatrix<float> modelMatrix;
			modelMatrix = modelMatrix * MurMatrix<float>::translation(juce::Vector3D<float>(x * getScreenScale(), y * getScreenScale(), 0.0));
			modelMatrix = modelMatrix.scaled(juce::Vector3D<float>(radius * getScreenScale(), radius * getScreenScale(), 1.0));

			modelMatrix = modelMatrix * stackedMatrix * currentMatrix;

			shaderMain->use();
			uniformMatrixModel->setMatrix4((GLfloat*)&(modelMatrix.mat[0]), 1, false);
			uniformMatrixView->setMatrix4((GLfloat*)&(viewMatrix.mat[0]), 1, false);
			uniformMatrixProj->setMatrix4((GLfloat*)&(projMatrix.mat[0]), 1, false);
			uniformUseTexture->set(useTexture);
			uniformVFlip->set(vflip);
			uniformColor->set(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
			
			vboCircle.internalDraw(GL_TRIANGLE_FAN, 0, circleResolution);
		}
		else {
			pushMatrix();
			translate(x, y, 0.0);
			scale(radius, radius, 1);
			draw(vboCircle, GL_LINE_LOOP, 0, circleResolution);
			popMatrix();
		}
	}

	void drawLineNew(float x1, float y1, float x2, float y2) {
		x1 = x1 * getScreenScale();
		y1 = y1 * getScreenScale();
		x2 = x2 * getScreenScale();
		y2 = y2 * getScreenScale();

		float a = (atan2(y2 - y1, x2 - x1));

		float dist = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

		MurMatrix<float> modelMatrix;
		modelMatrix = modelMatrix.scaled(juce::Vector3D<float>(dist, lineWidth * getScreenScale(), 1.0));
		modelMatrix = modelMatrix * MurMatrix<float>().rotated(juce::Vector3D<float>(0.0, 0.0, a));
		modelMatrix = modelMatrix * MurMatrix<float>::translation(juce::Vector3D<float>(x1, y1, 0.0));

		shaderMain->use();
		uniformMatrixModel->setMatrix4((GLfloat*)&(modelMatrix.mat[0]), 1, false);
		uniformMatrixView->setMatrix4((GLfloat*)&(viewMatrix.mat[0]), 1, false);
		uniformMatrixProj->setMatrix4((GLfloat*)&(projMatrix.mat[0]), 1, false);
		uniformUseTexture->set(useTexture);
		uniformVFlip->set(vflip);
		uniformColor->set(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
 
		vboLine.internalDraw(GL_TRIANGLE_FAN, 0, 4);
	}

	void drawLine(float x1, float y1, float x2, float y2) override {
		x1 = x1 * getScreenScale();
		y1 = y1 * getScreenScale();
		x2 = x2 * getScreenScale();
		y2 = y2 * getScreenScale();

		vector<MurkaPoint> verts;
		verts.push_back(MurkaPoint(x1, y1));
		verts.push_back(MurkaPoint(x2, y2));
		vboLineOld.setVertexData(verts.data(), verts.size());
		vboLineOld.update(GL_STREAM_DRAW, attribLocationPosition, attribLocationUv);

		draw(vboLineOld, GL_LINES, 0, verts.size());
	}

	void drawVbo(const MurVbo & vbo, GLuint drawMode, int first, int total) override {
		draw(vbo, drawMode, first, total);
	}

	void drawPath(const vector<MurkaPoint> & verts) override {
		vboLineOld.setVertexData(verts.data(), verts.size());
		vboLineOld.update(GL_STATIC_DRAW, attribLocationPosition, attribLocationUv);

		draw(vboLineOld, GL_LINE_STRIP, 0, verts.size());
	}

	int getWindowWidth() override {
		return glAppComp->getWidth() / getScreenScale();
	}

	int getWindowHeight() override {
		return glAppComp->getHeight() / getScreenScale();
	}

	bool getUsingArbTex() override {
		return false;
	}

	void disableArbTex() override {
	}

	void enableArbTex() override {
	}

	uint64_t getFrameNum() override {
		return frameNum;
	}

	float getElapsedTime() override {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count() / 1000.0;
	}
};



#else // "Default" render version

class MurkaRender: public MurkaRenderBase {
    
};

#endif

}
