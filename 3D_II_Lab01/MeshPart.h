#ifndef MESHPART_H_
#define MESHPART_H_
#include "includes.h"
#include "WorldData.h"
#include "MeshMaterialData.h"

/*struct MaterialInfo
{
	vec3 Ka;			// Ambient reflectivity
	vec3 Kd;		    // Diffuse reflectivity
	vec3 Ks;			// Specular reflectivity
	float Shininess;	// Specular shininess factor
	GLuint map_Kd;
	GLuint map_Ks;
	GLuint bump;

	MaterialInfo(){}
	MaterialInfo(vec3 ambRef, vec3 diffRef, vec3 specRef, float s)
	{
		Ka = ambRef;
		Kd = diffRef;
		Ks = specRef;
		Shininess = s;

	}
};*/

class MeshPart
{
private:
	//vector<VertexPoint> mVertices;
	int mVertexCount;
	MeshMaterialData* mMaterial;

public:
	GLuint mVAOHandle;
	GLuint mTextureHandle;
	GLuint mBumpMapHandle;
	GLuint mSpecMapHandle;

	MeshPart(GLuint VAOHandle, GLuint texHandle, GLuint bumpHandle, GLuint specHandle, int vertexCount, MeshMaterialData* material);
	~MeshPart(void);

	/*void SetVertexPoints(vector<VertexPoint>* vertices)
	{ mVertices = *vertices; }*/
	int GetVertexCount()
	{ return mVertexCount; }

	MeshMaterialData* GetMaterialInfo()
	{ return mMaterial; }
};

#endif