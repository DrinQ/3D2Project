#ifndef MESHPART_H_
#define MESHPART_H_
#include "includes.h"
#include "WorldData.h"

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

class MeshPart
{
private:
	//vector<VertexPoint> mVertices;
	int mVertexCount;
	MaterialInfo mMaterial;

public:
	GLuint mVAOHandle;
	GLuint mTextureHandle;

	MeshPart(GLuint VAOHandle, GLuint texHandle, int vertexCount, MaterialInfo material);
	~MeshPart(void);

	/*void SetVertexPoints(vector<VertexPoint>* vertices)
	{ mVertices = *vertices; }*/
	int GetVertexCount()
	{ return mVertexCount; }

	MaterialInfo GetMaterialInfo()
	{ return mMaterial; }
};

#endif