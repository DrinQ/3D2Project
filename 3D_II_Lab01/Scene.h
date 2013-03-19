#ifndef SCENE_H_
#define SCENE_H_

#include "Object3D.h"
#include "Light.h"
#include "ShadowMap.h"
#include "ShaderHandler.h"
#include "Camera.h"

class Scene
{
private:
	Object3D mBthObject;
	Object3D mGroundQuad;
	Object3D mHouse;
	vector<Object3D> mTreeList;

	Light mPointLight;

	ShadowMap mShadowMap;
	int mShadowMapRes;

	GLuint recordDepthIndex;
	GLuint shadeWithShadowIndex;

	ShaderHandler mShaderHandler;
	GLuint shaderProgHandle; // integer to get hold of our shader programme
	GLuint billboardShaderProgHandle;

	mat4 mCameraProjectionMat;

	Camera* mCam;

	void SetShadowMatrices(mat4 model);
	void SetValues(mat4 model);
	void SetLightValues();

public:
	
	Scene(int windowWidth, int windowHeight);
	Scene();
	~Scene();

	void CreateObjects();
	void CreateLights();
	void CreateShadowMap(int res);
	void Update();

	void RenderShadowingObjects();
	void RenderObjects();
	void RenderLightSources();

	void CreateShaderPrograms();

	void SetStaticUniforms();
	void SetCameraViewMatrix(mat4 view);
	
	Camera* GetCam()
	{ return mCam; }
};

#endif