#pragma once

#include "MurkaTypes.h"
#include "MurMatrix.h"

namespace murka {

#if defined(MURKA_OF)

class MurCamera {

	public:
		ofCamera camera;

		MurCamera() {
			camera.setNearClip(0.001);
			camera.setFarClip(1000);
		}

		void setPosition(MurkaPoint3D p) {
			camera.setPosition(p.x, p.y, p.z);
		}

		void lookAt(MurkaPoint3D p) {
			camera.lookAt(ofVec3f(p.x, p.y, p.z));
		}

		ofMatrix4x4 getViewMatrix() {
			return camera.getModelViewMatrix();
		}

		ofMatrix4x4 getProjectionMatrix(float aspect) {
			return camera.getProjectionMatrix();
		}

};

#elif defined(MURKA_JUCE)

class MurCamera {

	MurMatrix<float> transformMatrix;
	MurMatrix<float> projectionMatrix;

	MurkaPoint3D position;
	MurkaPoint3D target;

	void updateTransformMatrix() {
		MurMatrix<float> m;

		MurkaPoint3D relPosition = (position - target);
		float radius = relPosition.length();
		if (radius > 0) {
			float latitude = acos(relPosition.y / radius) - (float)( M_PI / 2);
			float longitude = atan2(relPosition.x, relPosition.z);

			juce::Quaternion<float> q = juce::Quaternion<float>::fromAngle(0.f, juce::Vector3D<float>(0, 0, 1));
			q *= juce::Quaternion<float>::fromAngle(longitude, juce::Vector3D<float>(0, 1, 0));
			q *= juce::Quaternion<float>::fromAngle(latitude, juce::Vector3D<float>(1, 0, 0));

			m = MurMatrix<float>::fromQuaternion(q);
		}

		transformMatrix = MurMatrix<float>();
		transformMatrix = transformMatrix.scaled(juce::Vector3D<float>(1, 1, 1));
		transformMatrix = transformMatrix * m;
		transformMatrix = transformMatrix * MurMatrix<float>::translation(juce::Vector3D<float>(position.x, position.y, position.z));
	}

public:
	MurCamera(){
	}

	void setPosition(MurkaPoint3D p) {
		position = p;
		updateTransformMatrix();
	}
	
	void lookAt(MurkaPoint3D p) {
		target = p;
		updateTransformMatrix();
	}

	MurMatrix<float> getViewMatrix() {
		MurMatrix<float> m_inv = transformMatrix.inverted();
		return transformMatrix.inverted();
	}

	MurMatrix<float> getProjectionMatrix(float aspect) { 
		float fov = 60;

		float zFar = 1000;
		float zNear = 0.001;
		 
		float tanHalfFovy = tan((fov * (float)(M_PI) / 180) / 2);

		projectionMatrix = MurMatrix<float>();

		projectionMatrix.mat[0] = 1 / (aspect * tanHalfFovy); // [0][0]
		projectionMatrix.mat[5] = 1 / (tanHalfFovy); // [1][1]
		projectionMatrix.mat[11] = -1; // [2][3] 14

		projectionMatrix.mat[10] = -(zFar + zNear) / (zFar - zNear); // [2][2] 
		projectionMatrix.mat[14] = -(2 * zFar * zNear) / (zFar - zNear); // [3][2] 11
		projectionMatrix.mat[15] = 0;

		return projectionMatrix;
	}

};

#else // Default version
#endif

}