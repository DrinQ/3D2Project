#ifndef SKYBOX_H_
#define SKYBOX_H_
#include "includes.h"
#include "SOIL.h"
class SkyBox
{
private:
	GLuint mTextureHandle;
	GLuint mVAOHandle;

	//mat4 mProjection; 
	mat4 mView;
	mat4 mModel;

public:
	SkyBox(string mapName, GLuint screenWidth, GLuint screenHeight);
	SkyBox();
	~SkyBox();

	void BindBuffers();
	void Draw(GLuint shaderProgHandle, mat4 projection, mat4 rotation);
};

#endif