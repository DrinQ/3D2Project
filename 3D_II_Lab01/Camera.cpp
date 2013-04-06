#include "Camera.h"


#ifndef M_PI
#define M_PI 3.14159265
#endif


bool mousein = false;

Camera::Camera(void)
{
	camX = 440.0, camY = 140.0, camZ = 450.0;
	camYaw = 18.0;
	camPitch = -42.0;
}

Camera::~Camera(void)
{
}

void Camera::lockCamera()
{
	if(camPitch > 90)
		camPitch = 90;
	if(camPitch < -90)
		camPitch = -90;
	if(camYaw < 0.0)
		camYaw += 360;
	if(camYaw > 360)
		camYaw -= 360;
}

void Camera::moveCamera(float dist, float dir)
{
	float rad = (camYaw + dir)*M_PI/180.0;
	camX -= sin(rad)*dist;
	camZ -= cos(rad)*dist;
}

void Camera::moveCameraUp(float dist, float dir)
{
	float rad = (camPitch+dir)*M_PI/180.0;
	camY += sin(rad)*dist;
}

void Camera::SetMousePos(int x, int y)
{
	mouseX = x;
	mouseY = y;
}

void Camera::Control(float moveVel, float mouseSens, bool mi, float midX, float midY)
{
	if(mi)
	{
		camYaw += mouseSens*(midX-mouseX);
		camPitch += mouseSens*(midY-mouseY);
		lockCamera();
		glutWarpPointer(midX, midY);
		if(GetAsyncKeyState('W') != 0)
		{
			if(camPitch!=90 && camPitch != -90)
				moveCamera(moveVel, 0.0);
				moveCameraUp(moveVel, 0.0);
		}
		else if(GetAsyncKeyState('S') != 0)
		{
			if(camPitch!=90 && camPitch != -90)
				moveCamera(moveVel, 180.0);
				moveCameraUp(moveVel, 180.0);
		}

		if(GetAsyncKeyState('A') != 0)
		{
			moveCamera(moveVel, 90.0);
		}
		else if(GetAsyncKeyState('D') != 0)
		{
			moveCamera(moveVel, 270.0);
		}
	}

}

void Camera::UpdateMatrices()
{
	vec3 eye(vec3(camX, camY, camZ)); // camera position in world coordinates
	vec3 centre(camX, camY, camZ-1); // orient camera to point towards a target position
	vec3 up(0.0f, 1.0f, 0.0f); // vector pointing up from camera's head  (describes roll of camera)
	mat4 lookAtMat = glm::lookAt(eye, centre, up);

	mRotationMat = glm::rotate(-camPitch, 1.0f, 0.0f, 0.0f) * glm::rotate(-camYaw, 0.0f, 1.0f, 0.0f);
	mCamViewMat = mRotationMat * lookAtMat;
}

float Camera::GetPitch()
{
	return camPitch;
}

float Camera::GetYaw()
{
	return camYaw;
}

glm::vec3 Camera::GetCamPos()
{
	return glm::vec3(camX, camY, camZ);
}

void Camera::SetYPos(float y)
{
	camY = y;
}

mat4 Camera::GetCamViewMatrix()
{
	return mCamViewMat;
}

mat4 Camera::GetCamRotationMatrix()
{
	return mRotationMat;
}
