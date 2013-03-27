#ifndef TERRAIN_H_
#define TERRAIN_H_
#include "includes.h"
#include "WorldData.h"
#include "TextureLoader.h"
#include "ShaderHandler.h"

class Terrain
{
private:
	int mWidth, mHeight;
	vector<float> mHeightMap;
	uint* mIndices;
	ShaderHandler* mShader;

	uint mVAOHandle;
	uint mTexureHandle;

	float SampleHeight3x3(int i, int j);
	void Filter3x3();

public:
	
	Terrain(int w, int h, const string &hMapFile, float yScale, float yOffset, float xzScale, char* texFile);
	Terrain();
	~Terrain();

	void LoadHeightMap(int m, int n,const string &filename, float heightScale, float heightOffset);
	void CreateGrid(int width, int height, float scale);
	void RenderGeometry(uint shaderProg);
	void Render(uint shaderProg);
	mat4 GetModelMatrix()
	{ return mat4(1.0f) * glm::translate(vec3(150.0f, -100.0f, -100.0f)); }
};

#endif