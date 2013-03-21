#include "MeshPart.h"


MeshPart::MeshPart(GLuint VAOHandle, GLuint texHandle, int vertexCount, MaterialInfo material)
{
	mVAOHandle = VAOHandle;
	mTextureHandle = texHandle;
	mVertexCount = vertexCount;
	mMaterial = material;
}


MeshPart::~MeshPart(void)
{
}
