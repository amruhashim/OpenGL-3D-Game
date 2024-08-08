#ifndef CAMERA_H
#define CAMERA_H

#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

// simple up-right camera (no roll)
class Camera {
public:
	Camera();
	~Camera();

	void update(float moveForward, float moveRight, float moveUp = 0.0f);
	void setViewMatrix(glm::vec3 position, glm::vec3 lookAt, glm::vec3 up = glm::vec3(0.0, 1.0, 0.0));
	void setProjMatrix(glm::mat4 projMatrix);
	glm::mat4 getViewMatrix();
	const glm::mat4 getProjMatrix();
	glm::vec3 getPosition();

	float mYaw = 0.0f;
	float mPitch = 0.0f;

private:
	
	glm::vec3 mPosition;
	glm::vec3 mLookAt;
	glm::vec3 mUp;
	glm::mat4 mViewMatrix;
	glm::mat4 mProjMatrix;
};

#endif