#include "MeshPart.h"


MeshPart::MeshPart(GLuint VAOHandle, GLuint texHandle, GLuint bumpHandle, GLuint specHandle, int vertexCount, MeshMaterialData* material)
{
	mVAOHandle = VAOHandle;
	mTextureHandle = texHandle;
	mBumpMapHandle = bumpHandle;
	mSpecMapHandle = specHandle;
	mVertexCount = vertexCount;
	mMaterial = material;
}


MeshPart::~MeshPart(void)
{
	delete mMaterial;
}
