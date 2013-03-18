#ifndef SHADOWMAP_H_
#define SHADOWMAP_H_
#include "includes.h"

class ShadowMap
{
private:
	GLuint mShadowFBO; //handle to framebuffer
	GLsizei mShadowMapWidth, mShadowMapHeight;
	GLuint mDepthTex;

	vec3 mLightPosition;
	vec3 mTargetDirection;
	mat4 mViewMatrix;
	mat4 mProjectionMatrix;
	mat4 mBiasMatrix;

	//Calculate the ShadowMatrix in the main class! (Needs ModelMatrix)
public:
	ShadowMap(vec3 lightPos, vec3 target, int resW, int resH);
	ShadowMap();
	~ShadowMap();

	void CreateShadowMapTexture();
	void ChangeResolution(int size);
	
	GLuint GetShadowFBOHandle()
		{ return mShadowFBO; }
	GLuint GetDepthTexHandle()
		{ return mDepthTex; }
	mat4 GetViewMatrix()
		{ return mViewMatrix; }
	mat4 GetProjectionMatrix()
		{ return mProjectionMatrix; }
	mat4 GetBiasMatrix()
		{ return mBiasMatrix; }
	void SetLightPos(vec3 pos);
};
#endif 

