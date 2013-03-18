#ifndef OBJECT3D_H_
#define OBJECT3D_H_
#include "includes.h"
#include "WorldData.h"
#include "ObjLoader.h"
#include "TextureLoader.h"

struct MaterialInfo
{
	vec3 Ka;			// Ambient reflectivity
	vec3 Kd;		    // Diffuse reflectivity
	vec3 Ks;			// Specular reflectivity
	float Shininess;	// Specular shininess factor

	MaterialInfo(){}
	MaterialInfo(vec3 ambRef, vec3 diffRef, vec3 specRef, float s)
	{
		Ka = ambRef;
		Kd = diffRef;
		Ks = specRef;
		Shininess = s;
	}
};

class Object3D
{
private:
	vector<VertexPoint> vertices;
	float mScale;
	vec3 mRotationAngles;
	glm::mat4 mModelMatrix;
	glm::mat4 mTranslation;
	glm::mat4 mRotation;
	vec3 mWorldPos;

	MaterialInfo mMaterial;
	
public:
	
	GLuint mVAOHandle;
	GLuint mTextureHandle;
	
	Object3D();
	Object3D(vec3 pos, float scale, vec3 rot);
	~Object3D();

	void LoadTexture(char* file, char* format);
	void CreateQuad();
	void CreateObjFromFile(char* filename);
	glm::mat4 GetModelMatrix();
	void Update();
	vector<VertexPoint>* GetVertexList()
	{ return &vertices; }
	MaterialInfo GetMaterialInfo()
	{ return mMaterial; }
};

#endif