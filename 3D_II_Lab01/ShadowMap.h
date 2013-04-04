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

public:
	ShadowMap(vec3 lightPos, vec3 target, int res);
	ShadowMap();
	~ShadowMap();

	void CreateShadowMapTexture(int i);
	void ChangeResolution(int size);
	
	GLuint GetShadowFBOHandle()
		{ return mShadowFBO; }
	GLuint GetDepthTexHandle()
		{ return mDepthTex; }
	mat4 GetViewMatrix()
		{ return mViewMatrix; }
	mat4 GetProjectionMatrix()
		{ return mProjectionMatrix; }
	mat4 GetBiasMatrix();
	void SetLightPos(vec3 pos);
};
#endif 

