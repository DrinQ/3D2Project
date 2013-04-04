#ifndef SCENE_H_
#define SCENE_H_

#include "Object3D.h"
#include "Light.h"
#include "ShadowMap.h"
#include "ShaderHandler.h"
#include "Camera.h"
#include "SkyBox.h"
#include "Terrain.h"

class Scene
{
private:
	float La;		// Ambient light intensity

	Object3D mBthObject;
	Object3D mGroundQuad;
	Object3D mHouse;
	vector<Object3D*> mTreeList;
	Terrain* mTerrain;

	Object3D quad1;
	Object3D quad2;

	vector<Light*> mPointLights;

	vector<ShadowMap*> mShadowMapList;
	SkyBox mSkyBox;
	//int mShadowMapRes;

	ShaderHandler* mShaderHandler;
	GLuint shaderProgHandle; // integer to get hold of our shader programme
	GLuint shadowShaderProgHandle;
	GLuint billboardShaderProgHandle;
	GLuint skyboxShaderProgHandle;

	mat4 mCameraProjectionMat;

	Camera* mCam;

	void SetShadowMatrices(int i, mat4 model);
	void SetValues(mat4 model);
	void SetLightValues(int i);

public:
	
	Scene(int windowWidth, int windowHeight);
	Scene();
	~Scene();

	void CreateObjects();
	void CreateLights(int shadowMapRes);
	void CreateShadowMaps(int index, int res);
	void Update();

	void RenderSkyBox();
	void RenderShadowingObjects();
	void RenderObjects();
	void RenderLightSources();

	void CreateShaderPrograms();

	void SetStaticUniforms();
	
	Camera* GetCam()
	{ return mCam; }
};

#endif