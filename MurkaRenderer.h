#pragma once

#include "MurkaRendererBase.h"
#include "MurMatrix.h"
#include "MurShader.h"

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
	void drawImage(const MurImage & image, float x, float y, float w, float h) override {
		ofRenderer->draw(image.internal, x * getScreenScale(), y * getScreenScale(), 0, w * getScreenScale(), h * getScreenScale(), 0, 0, image.internal.getWidth(), image.internal.getHeight());
	}

	void drawImage(const MurImage & image, float x, float y) override {
		ofRenderer->draw(image.internal, x * getScreenScale(), y * getScreenScale(), 0, image.internal.getWidth() * getScreenScale(), image.internal.getHeight() * getScreenScale(), 0, 0, image.internal.getWidth() * getScreenScale(), image.internal.getHeight() * getScreenScale());
	}

	void drawImage(const MurTexture & texture, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
        ofRenderer->draw(texture.internal, x * getScreenScale(), y * getScreenScale(), z * getScreenScale(), w * getScreenScale(), h * getScreenScale(), sx * getScreenScale(), sy * getScreenScale(), sw * getScreenScale(), sh * getScreenScale());
    }
    
    void drawVbo(const MurVbo & vbo, GLuint drawMode, int first, int total) override{
		pushMatrix();
		scale(1, 1, 1);
		ofRenderer->draw(vbo.internal, drawMode, first, total);
		popMatrix();
	}
    
    // Textures binding
	void bind(const MurImage & img, int location = 0) override {
		ofRenderer->bind(img.internal.getTexture(), location);
	}

	void unbind(const MurImage & img, int location = 0) override {
		ofRenderer->unbind(img.internal.getTexture(), location);
	}

	void bind(const MurTexture& texture, int location = 0) override {
		ofRenderer->bind(texture.internal, location);
	}

	void unbind(const MurTexture& texture, int location = 0) override {
		ofRenderer->unbind(texture.internal, location);
	}

	void bind(const MurFbo& fbo, int location = 0) override {
		ofRenderer->bind(fbo.internal, location);
	}

	void unbind(const MurFbo& texture, int location = 0) override {
		ofRenderer->unbind(fbo.internal, location);
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
    
	void setBlendMode(MurBlendMode blendMode) override {
		ofRenderer->setBlendMode((ofBlendMode)blendMode);
	}

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

	void drawPath(const vector<MurkaPoint3D> & verts) override {
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

	void beginCamera(MurCamera cam) {
		cam.camera.begin();
		ofScale(1 / getScreenScale()); // fix for scale ui
	}

	void endCamera(MurCamera cam) {
		cam.camera.end();
	}
};


#elif defined(MURKA_JUCE)

#define GL_FUNC_ADD 0x8006
#define GL_FUNC_ADD 0x8006

class MurkaRenderer : public MurkaRendererBase {

	juce::OpenGLAppComponent* glAppComp = nullptr;
	juce::OpenGLContext* openGLContext = nullptr;

	std::function<std::string(void)> getClipboardCallback = nullptr;
	std::function<void(std::string)> setClipboardCallback = nullptr;

	std::vector<MurMatrix<float>> matrixStack;
	MurMatrix<float> currentMatrix;

	std::vector<MurkaShape> viewportStack;
	MurkaShape currentViewport;
	MurMatrix<float> stackedMatrix;
	
	// camera
	std::vector<MurMatrix<float>> modelStack;
	MurMatrix<float> currentModelMatrix;

	std::vector<MurMatrix<float>> viewStack;
	MurMatrix<float> currentViewMatrix;

	std::vector<MurMatrix<float>> projectionStack;
	MurMatrix<float> currentProjectionMatrix;

	struct Style {
		MurkaColor color = MurkaColor(1.0f, 1.0f, 1.0f, 1.0f);
		bool fill = true;
	};
	std::vector<Style> styleStack;
	Style currentStyle;

	float lineWidth = 1;
	float circleResolution = 32;


	MurShader shaderMain;
	MurShader* currentShader = nullptr;

	MurVbo vboRect;
	MurVbo vboLine;
	MurVbo vboLineOld;
	MurVbo vboCircle;

	void recreateCircleVbo() {
		std::vector<MurkaPoint3D> verts(circleResolution);
		for (int i = 0; i < circleResolution; i++)
		{
			float theta = 2.0f * juce::MathConstants<float>::pi * float(i) / float(circleResolution);
			float x = 1.0 * cosf(theta);
			float y = 1.0 * sinf(theta);
			verts[i] = (MurkaPoint3D(x, y, 0));
		}

		vboCircle.setOpenGLContext(openGLContext);
		vboCircle.setup();
		vboCircle.setVertexData(verts.data(), verts.size());
		vboCircle.update(GL_STREAM_DRAW, currentShader->attributePosition, currentShader->attributeUv, currentShader->attributeCol);
	}

	void updateStackedMatrix() {
		stackedMatrix = matrixStack.size() == 0 ? MurMatrix<float>() : matrixStack[0];
		for (int i = 1; i < matrixStack.size(); i++) {
			stackedMatrix = matrixStack[1] * stackedMatrix;
		}
	}

	bool vflip = true;
	bool useFbo = false;
	bool useTexture = false;
	bool useCamera = false;

	uint64_t frameNum = 0;
	std::chrono::steady_clock::time_point begin;

public:

	std::string vertexShaderBase = R"(
		varying vec2 vUv;
		varying vec4 vCol;
		uniform mat4 matrixModel;
		uniform mat4 matrixView;
		uniform mat4 matrixProj;
		uniform vec4 color;
		attribute vec3 position;
		attribute vec2 uv;
		attribute vec4 col;
		
		void main()
		{
			vUv = uv;
			vCol = col;
			vec4 pos = matrixProj * matrixView * matrixModel * vec4(position, 1.0); 
			gl_Position =  pos; 
		}
	)";


	std::string fragmentShaderBase = R"(
		varying vec2 vUv;
		varying vec4 vCol;
		uniform sampler2D mainTexture;
		uniform vec4 color;
		uniform bool vflip;
		uniform bool useTexture;

		void main()
		{
			vec2 uv = vUv;
			if (vflip) uv.y = 1 - uv.y;
			gl_FragColor = color * vCol * (useTexture ? texture(mainTexture, uv) : vec4(1.0, 1.0, 1.0, 1.0));
		}
	)";

	MurkaRenderer() {
	}

	~MurkaRenderer() {
	}

	void setAppComponent(juce::OpenGLAppComponent* comp) {
		glAppComp = comp;
	}

	void setOpenGLContext(juce::OpenGLContext* ctx) {
		openGLContext = ctx;
	}

	MurShader* getMainShader() {
		return &shaderMain;
	}

	void closeOpenGLContext() {
		if (shaderMain.isLoaded()) {
			shaderMain.unload();
		}
	}
   
	void bindShader(MurShader* shader) {
		shader->use();

		currentShader = shader;
	}

	void unbindShader() {
		shaderMain.use();

		currentShader = &shaderMain;
	}

	juce::OpenGLContext* getOpenGLContext() {
		return openGLContext;
	}

	void setup() {
        // create main shader
        {
			shaderMain.setOpenGLContext(openGLContext);
			shaderMain.load(vertexShaderBase, fragmentShaderBase);
			bindShader(&shaderMain);
        }

        // vbo for primitives
		{
			std::vector<MurkaPoint3D> verts;
			verts.push_back(MurkaPoint3D(0.0f, 0.0f, 0.0f));
			verts.push_back(MurkaPoint3D(0.0f, 1.0f, 0.0f));
			verts.push_back(MurkaPoint3D(1.0f, 1.0f, 0.0f));
			verts.push_back(MurkaPoint3D(1.0f, 0.0f, 0.0f));

			std::vector<MurkaPoint> texCoords;
			texCoords.push_back(MurkaPoint(0.0f, 0.0f));
			texCoords.push_back(MurkaPoint(0.0f, 1.0f));
            texCoords.push_back(MurkaPoint(1.0f, 1.0f));
			texCoords.push_back(MurkaPoint(1.0f, 0.0f));

			vboRect.setOpenGLContext(openGLContext);
			vboRect.setup();
			vboRect.setVertexData(verts.data(), verts.size());
			vboRect.setTexCoordData(texCoords.data(), texCoords.size());
			updateVbo(vboRect);
		}

		{
			std::vector<MurkaPoint3D> verts;
			verts.push_back(MurkaPoint3D(0.0f, -0.5f, 0.0f));
			verts.push_back(MurkaPoint3D(0.0f, 0.5f, 0.0f));
			verts.push_back(MurkaPoint3D(1.0f, 0.5f, 0.0f));
			verts.push_back(MurkaPoint3D(1.0f, -0.5f, 0.0f));

			std::vector<MurkaPoint> texCoords;
            texCoords.push_back(MurkaPoint(0.0f, 0.0f));
            texCoords.push_back(MurkaPoint(0.0f, 1.0f));
            texCoords.push_back(MurkaPoint(1.0f, 1.0f));
            texCoords.push_back(MurkaPoint(1.0f, 0.0f));

			vboLine.setOpenGLContext(openGLContext);
			vboLine.setup();
			vboLine.setVertexData(verts.data(), verts.size());
            vboLine.setTexCoordData(texCoords.data(), texCoords.size());
			updateVbo(vboLine);
		}

		{
			std::vector<MurkaPoint3D> verts;
			verts.push_back(MurkaPoint3D(0.0f, 0.0f, 0.0f));
			verts.push_back(MurkaPoint3D(0.0f, 0.0f, 0.0f));

			std::vector<MurkaPoint> texCoords;
            texCoords.push_back(MurkaPoint(0.0f, 0.0f));
            texCoords.push_back(MurkaPoint(1.0f, 1.0f));

			vboLineOld.setOpenGLContext(openGLContext);
			vboLineOld.setup();
			vboLineOld.setVertexData(verts.data(), verts.size());
            vboLineOld.setTexCoordData(texCoords.data(), texCoords.size());
			updateVbo(vboLineOld);
		}

		{
			recreateCircleVbo();
		}

        float w  = openGLContext->getRenderingScale();
        currentViewport = MurkaShape(0, 0, glAppComp->getWidth() * openGLContext->getRenderingScale(), glAppComp->getHeight() * openGLContext->getRenderingScale());

        begin = std::chrono::steady_clock::now();
	}

	void startFrame() {
		// clear matrix stack
		matrixStack.clear();
		currentMatrix = MurMatrix<float>();

		styleStack.clear();
		currentStyle = Style();

		viewportStack.clear();
        currentViewport = MurkaShape(0, 0, glAppComp->getWidth() * openGLContext->getRenderingScale(), glAppComp->getHeight() *  openGLContext->getRenderingScale());

		// camera
		modelStack.clear();
		currentModelMatrix = MurMatrix<float>();

		viewStack.clear();
		currentViewMatrix = MurMatrix<float>();
	
		projectionStack.clear();
		currentProjectionMatrix = MurMatrix<float>();

		frameNum++;
		enableAlphaBlending();
        
		unbindShader();

        setupScreen();
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

	void setFont(std::string filename, int size) {
		MurkaAssets::setFont(filename, size, this);
	}

	void setFontFromRawData(std::string name, const char* data, int dataSize, int fontSize) {
		MurkaAssets::setFont(name, data, dataSize, fontSize, this);
	}

	void drawString(const std::string & s, float x, float y) {
		MurkaAssets::getCurrentFont()->drawString(s, x, y);
	}

	// Object drawing
	void drawImage(const MurImage& image, float x, float y) override {
		if (image.isAllocated()) {
			bind(image, 0);
			drawRectangle(x, y, image.getWidth(), image.getHeight());
			unbind(image, 0);
		}
	}

	void drawImage(const MurImage& image, float x, float y, float w, float h) override {
		if (image.isAllocated()) {
			bind(image, 0);
			drawRectangle(x, y, w, h);
			unbind(image, 0);
		}
	}

	void drawImage(const MurImage& image, float x, float y, float z, float w, float h) override {
		if (image.isAllocated()) {
			bind(image, 0);
			drawRectangle(x, y, w, h);
			unbind(image, 0);
		}
	}

	void drawImage(const MurImage& image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
#ifdef MURKA_DEBUG
		std::cout << "TODO" << std::endl;
#endif
	}

	void drawTexture(const MurTexture & texture, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) override {
#ifdef MURKA_DEBUG
		std::cout << "TODO" << std::endl;
#endif
	}
	
	void drawVbo(const MurVbo & vbo, GLuint drawMode, int first, int total) override {
		MurMatrix<float> modelMatrix;
		modelMatrix = modelMatrix.scaled(juce::Vector3D<float>(getScreenScale(), getScreenScale(), 1.0));
		modelMatrix = modelMatrix * stackedMatrix * currentMatrix;

		MurMatrix<float> vflipMatrix;
		if (vflip && !useFbo) {
			vflipMatrix.scale(juce::Vector3D<float>(1, -1, 1));
		}

		currentShader->setUniformMatrix4f(currentShader->uniformMatrixModel, modelMatrix * currentModelMatrix);
		currentShader->setUniformMatrix4f(currentShader->uniformMatrixView, currentViewMatrix);
		currentShader->setUniformMatrix4f(currentShader->uniformMatrixProj, vflipMatrix * currentProjectionMatrix);
		
		currentShader->setUniform1i(currentShader->uniformUseTexture, useTexture);
		currentShader->setUniform4f(currentShader->uniformColor, currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
		vbo.internalDraw(drawMode, first, total);
	}

	// Textures binding
	void bind(const MurImage & img, int location = 0) override {
		if (img.isAllocated()) {
			if(location==0) useTexture = true;
			openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
			img.bind();
		}
	}

	void unbind(const MurImage & img, int location = 0) override {
		if (img.isAllocated()) {
			if (location == 0) useTexture = false;
			openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
			img.unbind();
		}
	}
	
	void bind(const MurTexture& texture, int location = 0) override {
		if (texture.isAllocated()) {
			if (location == 0) useTexture = true;
			openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
			texture.bind();
		}
	}

	void unbind(const MurTexture& texture, int location = 0) override {
		if (texture.isAllocated()) {
			if (location == 0) useTexture = false;
			openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
			texture.unbind();
		}
	}

	void bind(const MurFbo& fbo, int location = 0) override {
		if (fbo.isAllocated()) {
			if (location == 0) useTexture = true;
			openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
			fbo.bind();
		}
	}

	void unbind(const MurFbo& fbo, int location = 0) override {
		if (fbo.isAllocated()) {
			if (location == 0) useTexture = false;
			openGLContext->extensions.glActiveTexture(GL_TEXTURE0 + location);
			fbo.unbind();
		}
	}

	void beginFbo(MurFbo& fbo) {
		if (fbo.isAllocated()) {
			useFbo = true;
			fbo.begin();
		}
	}

	void endFbo(MurFbo& fbo) {
		if (fbo.isAllocated()) {
			useFbo = false;
			fbo.end();
		}
	}

	void pushView() override {
		viewportStack.push_back(currentViewport);

		// camera
		modelStack.push_back(currentModelMatrix);
		viewStack.push_back(currentViewMatrix);
		projectionStack.push_back(currentProjectionMatrix);
	}

	void popView() override {
		if (viewportStack.size() > 0) {
			MurkaShape v = viewportStack.back() / getScreenScale();
			viewport(v);
			//setupScreen();
			viewportStack.pop_back();

			// camera
			currentModelMatrix = modelStack.back();
			modelStack.pop_back();

			currentViewMatrix = viewStack.back();
			viewStack.pop_back();

			currentProjectionMatrix = projectionStack.back();
			projectionStack.pop_back();
		}
		else {
			throw std::runtime_error("Check the count of push/popView calls");
		}
	}

	void pushMatrix() override {
		matrixStack.push_back(currentMatrix);
		currentMatrix = MurMatrix<float>();
		updateStackedMatrix();
	}

	void popMatrix() override {
		if (matrixStack.size() > 0) {
			currentMatrix = matrixStack.back();
			matrixStack.pop_back();
			updateStackedMatrix();
		}
		else {
			throw std::runtime_error("Check the count of push/popMatrix calls");
		}
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
		glViewport(currentViewport.x(), vflip ? glAppComp->getHeight() * openGLContext->getRenderingScale() - currentViewport.height() - currentViewport.y() : currentViewport.y(), currentViewport.width(), currentViewport.height());
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

		currentProjectionMatrix = MurMatrix<float>::fromPerspective(juce::degreesToRadians(fov), aspect, nearDist, farDist);
		currentViewMatrix = MurMatrix<float>::fromLookAt(juce::Vector3D<float>(eyeX, eyeY, dist), juce::Vector3D<float>(eyeX, eyeY, 0), juce::Vector3D<float>(0, 1, 0));
		currentModelMatrix = MurMatrix<float>();

		useCamera = false;
	}

	// rendering setup
	void setCircleResolution(int resolution) override {
		if (this->circleResolution != resolution) {
			this->circleResolution = resolution;
			recreateCircleVbo();
		}
	}

	void setLineWidth(float lineWidth) override {
		this->lineWidth = lineWidth;
	};

	void enableFill() override {
		currentStyle.fill = true;
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	};

	void disableFill() override {
		currentStyle.fill = false;
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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

	void setBlendMode(MurBlendMode blendMode) override {
		if (blendMode == MUR_BLENDMODE_DISABLED) {
			glDisable(GL_BLEND);
		}
		else if (blendMode == MUR_BLENDMODE_ALPHA) {
			glEnable(GL_BLEND);

			typedef void(*glBlendEquation_type)(GLenum p1);
			glBlendEquation_type pglBlendEquation;
			pglBlendEquation = (glBlendEquation_type)juce::OpenGLHelpers::getExtensionFunction("glBlendEquation");

			if (pglBlendEquation) {
				pglBlendEquation(GL_FUNC_ADD);
			}

			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}
		else if (blendMode == MUR_BLENDMODE_ADD) {
			glEnable(GL_BLEND);

			typedef void(*glBlendEquation_type)(GLenum p1);
			glBlendEquation_type pglBlendEquation;
			pglBlendEquation = (glBlendEquation_type)juce::OpenGLHelpers::getExtensionFunction("glBlendEquation");

			if (pglBlendEquation) {
				pglBlendEquation(GL_FUNC_ADD);
			}

			glBlendFunc(GL_SRC_ALPHA, GL_ONE);
		}
	}

	void updateVbo(MurVbo& vbo) override {
		vbo.update(GL_STATIC_DRAW, currentShader->attributePosition, currentShader->attributeUv, currentShader->attributeCol);
	}

	void disableAlphaBlending() override {
		glDisable(GL_BLEND);
	};

	void pushStyle() override {
		styleStack.push_back(currentStyle);
	};

	void popStyle() override {
		if (styleStack.size() > 0) {
			currentStyle = styleStack.back();
			styleStack.pop_back();
		}
		else {
			throw std::runtime_error("Check the count of push/popStyle calls");
		}
	};

	// color operations
	MurkaColor getColor() override {
		return currentStyle.color;
	}
	 
	void setColor(int r, int g, int b) override {
		currentStyle.color = MurkaColor(1.0 * r / 255, 1.0 * g / 255, 1.0 * b / 255);
	}; // 0-255

	void setColor(int r, int g, int b, int a) override {
		currentStyle.color = MurkaColor(1.0 * r / 255, 1.0 * g / 255, 1.0 * b / 255, 1.0 * a / 255);
	}; // 0-255

	void setColor(const MurkaColor & color) override {
		currentStyle.color = MurkaColor(color.r, color.g, color.b, color.a);
	};

	void setColor(const MurkaColor & color, int _a) override {
		currentStyle.color = MurkaColor(color.r, color.g, color.b, 1.0 * _a / 255);
	};

	void setColor(int gray) override {
		currentStyle.color = MurkaColor(1.0 * gray / 255, 1.0 * gray / 255, 1.0 * gray / 255);
	}; // 0 - 255

	void setColor(int gray, int _a) override {
		currentStyle.color = MurkaColor(1.0 * gray / 255, 1.0 * gray / 255, 1.0 * gray / 255, 1.0 * _a / 255);
	}; // 0-255

	void clear() override {
		glClearColor(currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	};

	void clear(int r, int g, int b, int a = 0) override {
		MurkaColor col(1.0 * r / 255, 1.0 * g / 255, 1.0 * b / 255, 1.0 * a / 255);
		glClearColor(col.r, col.g, col.b, col.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	};

	void clear(int gray, int a = 0) override {
		MurkaColor col(1.0 * gray / 255, 1.0 * gray / 255, 1.0 * gray / 255, 1.0 * a / 255);
		glClearColor(col.r, col.g, col.b, col.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	};

	void drawRectangle(float x, float y, float w, float h) override {
		MurMatrix<float> modelMatrix;
		modelMatrix = modelMatrix * MurMatrix<float>::translation(juce::Vector3D<float>(x * getScreenScale(), y * getScreenScale(), 0.0));
		modelMatrix = modelMatrix.scaled(juce::Vector3D<float>(w * getScreenScale(), h * getScreenScale(), 1.0));


		modelMatrix = modelMatrix * stackedMatrix * currentMatrix;

		MurMatrix<float> vflipMatrix;
		if (vflip && !useFbo) {
			vflipMatrix.scale(juce::Vector3D<float>(1, -1, 1));
		}

		currentShader->setUniformMatrix4f(currentShader->uniformMatrixModel, modelMatrix * currentModelMatrix);
		currentShader->setUniformMatrix4f(currentShader->uniformMatrixView, currentViewMatrix);
		currentShader->setUniformMatrix4f(currentShader->uniformMatrixProj, vflipMatrix * currentProjectionMatrix);

		currentShader->setUniform1i(currentShader->uniformUseTexture, useTexture);
		currentShader->setUniform1i(currentShader->uniformVflip, useCamera);
		currentShader->setUniform4f(currentShader->uniformColor, currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);
		updateVbo(vboRect);
		vboRect.internalDraw(currentStyle.fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, 4);
	}

	void drawRectangle(MurkaShape s) override {
		drawRectangle(s.position.x, s.position.y, s.size.x, s.size.y);
	}

	void drawCircle(float x, float y, float radius) override {
		MurMatrix<float> modelMatrix;
		modelMatrix = modelMatrix * MurMatrix<float>::translation(juce::Vector3D<float>(x * getScreenScale(), y * getScreenScale(), 0.0));
		modelMatrix = modelMatrix.scaled(juce::Vector3D<float>(radius * getScreenScale(), radius * getScreenScale(), 1.0));

		modelMatrix = modelMatrix * stackedMatrix * currentMatrix;

		MurMatrix<float> vflipMatrix;
		if (vflip && !useFbo) {
			vflipMatrix.scale(juce::Vector3D<float>(1, -1, 1));
		}

		currentShader->setUniformMatrix4f(currentShader->uniformMatrixModel, modelMatrix * currentModelMatrix);
		currentShader->setUniformMatrix4f(currentShader->uniformMatrixView, currentViewMatrix);
		currentShader->setUniformMatrix4f(currentShader->uniformMatrixProj, vflipMatrix * currentProjectionMatrix);

		currentShader->setUniform1i(currentShader->uniformUseTexture, useTexture);
		currentShader->setUniform1i(currentShader->uniformVflip, useCamera);
		currentShader->setUniform4f(currentShader->uniformColor, currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);

		vboCircle.internalDraw(currentStyle.fill ? GL_TRIANGLE_FAN : GL_LINE_LOOP, 0, circleResolution);
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

		MurMatrix<float> vflipMatrix;
		if (vflip && !useFbo) {
			vflipMatrix.scale(juce::Vector3D<float>(1, -1, 1));
		}

		currentShader->setUniformMatrix4f(currentShader->uniformMatrixModel, modelMatrix * currentModelMatrix);
		currentShader->setUniformMatrix4f(currentShader->uniformMatrixView, currentViewMatrix);
		currentShader->setUniformMatrix4f(currentShader->uniformMatrixProj, vflipMatrix * currentProjectionMatrix);

		currentShader->setUniform1i(currentShader->uniformUseTexture, useTexture);
		currentShader->setUniform1i(currentShader->uniformVflip, useCamera);
		currentShader->setUniform4f(currentShader->uniformColor, currentStyle.color.r, currentStyle.color.g, currentStyle.color.b, currentStyle.color.a);

		vboLine.internalDraw(GL_TRIANGLE_FAN, 0, 4);
	}

	int getMainShaderAttribLocation(std::string name) {
		return shaderMain.getAttributeLocation(name);
	}

	void drawLine(float x1, float y1, float x2, float y2) override {
		std::vector<MurkaPoint3D> verts(2);
		verts[0] = (MurkaPoint3D(x1, y1, 0));
		verts[1] = (MurkaPoint3D(x2, y2, 0));
		vboLineOld.setVertexData(verts.data(), verts.size());

		updateVbo(vboLineOld);
		drawVbo(vboLineOld, GL_LINES, 0, verts.size());
	}

	void drawPath(const std::vector<MurkaPoint3D> & verts) override {
		vboLineOld.setVertexData(verts.data(), verts.size());

		updateVbo(vboLineOld);
		drawVbo(vboLineOld, GL_LINE_STRIP, 0, verts.size());
	}

	int getWindowWidth() override {
		return glAppComp->getWidth() * openGLContext->getRenderingScale() / getScreenScale();
	}

	int getWindowHeight() override {
		return glAppComp->getHeight() * openGLContext->getRenderingScale() / getScreenScale();
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

	void beginCamera(MurCamera cam) {
		pushView();

		MurMatrix<float> vflipMatrix;
		if (vflip && !useFbo) {
			vflipMatrix.scale(juce::Vector3D<float>(1, -1, 1));
		}

		MurkaShape view = getCurrentViewport();
		currentProjectionMatrix = vflipMatrix * cam.getProjectionMatrix(view.size.x / view.size.y);
		currentViewMatrix = cam.getViewMatrix();
		currentModelMatrix = MurMatrix<float>().scaled(juce::Vector3D<float>(1/ getScreenScale(), 1/ getScreenScale(), 1 / getScreenScale()));

		useCamera = true;
	}

	void endCamera(MurCamera cam) {
		useCamera = false;

		popView();
	}
};



#else // "Default" render version

class MurkaRender: public MurkaRenderBase {
    
};

#endif

}
