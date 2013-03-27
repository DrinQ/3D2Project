#ifndef OBJECT3D_H_
#define OBJECT3D_H_
#include "ObjLoader.h"
#include "TextureLoader.h"
#include "MeshPart.h"
#include "ShaderHandler.h"

class Object3D
{
private:
	vector<MeshPart*>* mMeshList;
	float mScale;
	vec3 mRotationAngles;
	glm::mat4 mModelMatrix;
	glm::mat4 mTranslation;
	glm::mat4 mRotation;
	vec3 mWorldPos;
	ShaderHandler* mShader;

public:
	Object3D();
	Object3D(vec3 pos, float scale, vec3 rot);
	~Object3D();

	uint LoadTexture(char* file, char* format, GLenum textureSlot);
	void CreateQuad(char* texFile, char* fileFormat);
	void CreateObjFromFile(char* filename);
	glm::mat4 GetModelMatrix();
	void Update();
	vector<MeshPart*>* GetMeshList()
	{ return mMeshList; }
	
	void RenderGeometry(uint shaderProg);
	void Render(uint shaderProg);
	void SetMeshList(vector<MeshPart*>* meshList)
	{ mMeshList = meshList; }
};

#endif