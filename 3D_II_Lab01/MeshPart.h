#ifndef MESHPART_H_
#define MESHPART_H_
#include "includes.h"
#include "WorldData.h"
#include "MeshMaterialData.h"



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