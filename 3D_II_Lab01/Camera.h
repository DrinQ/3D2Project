#ifndef CAMERA_H
#define CAMERA_H
#include <GL/freeglut.h>
#include <Windows.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // rotate functions
#include <glm/gtx/transform2.hpp> // lookAt function
#include <glm/gtc/type_ptr.hpp>

#define _USE_MATH_DEFINES
#include <cmath>
using glm::mat4;
using glm::vec3;

class Camera
{
private:

	int mouseX, mouseY;
	float camX, camY, camZ;
	float camYaw;
	float camPitch;
	mat4 mCamViewMat;
	mat4 mRotationMat;

public:
	
	Camera(void);
	~Camera(void);

	void lockCamera();
	void moveCamera(float, float);
	void moveCameraUp(float, float);
	void Control(float moveVel, float mouseSense, bool mi, float midX, float midY);
	void UpdateMatrices();
	void SetMousePos(int, int);
	float GetYaw();
	float GetPitch();
	glm::vec3 GetCamPos();
	void SetYPos(float y);
	mat4 GetCamViewMatrix();
	mat4 GetCamRotationMatrix();

};

#endif

