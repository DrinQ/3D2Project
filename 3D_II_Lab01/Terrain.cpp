#include "Terrain.h"


Terrain::Terrain(int w, int h, const string &hMapFile, float yScale, float yOffset, float xzScale, char* texFile)
{
	mWidth = w;
	mHeight = h;

	LoadHeightMap(w, h, hMapFile, yScale, yOffset);
	CreateGrid(w, h, xzScale);
	mTexureHandle = TextureLoader::LoadTexture(texFile, GL_TEXTURE0);
}

Terrain::Terrain()
{
}

Terrain::~Terrain()
{
}

void Terrain::LoadHeightMap(int m, int n,const string &filename, float heightScale, float heightOffset)
{

	//A height for each vertex
	vector<unsigned char> vertexHeights(m*n);

	//Open file of reading
	ifstream fin;
	fin.open(filename.c_str(), ios_base::binary);
	if (!fin) { 
		cout << "Error opening .raw-file" << endl;
	}

	//Read all the RAW bytes in once
	fin.read((char*)&vertexHeights[0], (streamsize)vertexHeights.size());
	fin.close();

	//Copy array data into a float table(matrix) format
	//Scale and offset the heights
	mHeightMap.resize(m*n, 0);
	
	for(int i = 0; i < m*n; i++)
	{
		mHeightMap[i] = (vertexHeights[i]/255.0f) * heightScale + heightOffset;
	}

	Filter3x3();
}

float Terrain::SampleHeight3x3(int i, int j)
{
	float average = 0.0f;
	float sample = 0.0f;

	for(int m = i-1; m <= i+1; m++)
	{
		for(int n = j-1; n <= j+1; n++)
		{
			//if in bounds?
			if(m < 0){ m=0; }
			if(m > mHeight){ m=mHeight; }
			if(n < 0){ n=0; }
			if(n > mWidth){ n=mWidth; }
			average += mHeightMap[m*mWidth + n];
			sample += 1.0f;
		}
	}
	return average/sample;
}

void Terrain::Filter3x3()
{
	vector<float> FilteredhMap(mHeight * mWidth);

	for(int i = 0; i < mHeight-1; i++)
	{
		for(int j = 0; j < mWidth-1; j++)
		{
			FilteredhMap[i*mWidth + j] = SampleHeight3x3(j, i);
		}
	}
	
	mHeightMap = FilteredhMap;
}

void Terrain::CreateGrid(int width, int height, float scale)
{
	int triangleCount = (width-1)*(height-1) * 2;

	mIndices = new uint[triangleCount * 3];
	vec2* normals = new vec2[width*height];

	UINT k = 0;
    for (UINT i = 0; i < height-2; i++)
    {
            for (UINT j = 0; j < width-2; j++)
            {
                    mIndices[k]     = i * (width-1) + j;
                    mIndices[k + 1] = (i+1) * (width-1) + j;
                    mIndices[k + 2] = (i+1) * (width-1) + j+1;


                    mIndices[k + 3] = i * (width-1) + j;
                    mIndices[k + 4] = (i+1) * (width-1) + j+1;
                    mIndices[k + 5] = i * (width-1) + j+1;
 
                    k += 6;
            }
    }

	VertexPoint* mGridValues = new VertexPoint[width*height];

	int dataIndex = 0;

	for(int j = 0; j<height-1; j++)
		for(int i= 0; i <width-1; i++)
		{
			mGridValues[dataIndex] = VertexPoint(vec3(i*scale, mHeightMap[i + j*height], j*scale), vec2((float)i/(width-1),  1.0-(float)j/(height-1)), vec3(0.0, 1.0, 0.0));
			
			dataIndex++;
		}

	for(uint i = 0; i < (height-2)*(width-2); i++)
	{
		vec3 v0 = mGridValues[i].position;
		vec3 v1 = mGridValues[i+width].position;
		vec3 v2 = mGridValues[i+width+1].position;
		mGridValues[i].normal = glm::cross(v1-v0,v2-v1);
	}
		
	float* positionData = new float[width*height*3];
	float* texCoordData = new float[width*height*2];
	float* normalData = new float[width*height*3];
	int count = 0;

	for(int i = 0; i < width*height; i++)
	{
		positionData[3*count] = mGridValues[i].position.x;
		positionData[3*count+1] = mGridValues[i].position.y;
		positionData[3*count+2] = mGridValues[i].position.z;

		texCoordData[2*count] = mGridValues[i].texCoord.x;
		texCoordData[2*count+1] = mGridValues[i].texCoord.y;

		normalData[3*count] = mGridValues[i].normal.x;
		normalData[3*count+1] = mGridValues[i].normal.y;
		normalData[3*count+2] = mGridValues[i].normal.z;

		count++;
	}
	
	// remember how many
	int numberOfPoints = count*3;

	// Make 2 new VBO handles
	GLuint VBOHandles[3];
	glGenBuffers(3, VBOHandles);

	// "Bind" (switch focus to) first buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOHandles[0]); 

	// Put data in currently bound buffer
	glBufferData(GL_ARRAY_BUFFER, numberOfPoints * sizeof(float), positionData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHandles[1]);
	glBufferData(GL_ARRAY_BUFFER, numberOfPoints * sizeof(float), normalData, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHandles[2]);
	glBufferData(GL_ARRAY_BUFFER, count*2 * sizeof(float), texCoordData, GL_STATIC_DRAW);

	// create 1 VAO
	glGenVertexArrays(1, &mVAOHandle);
	glBindVertexArray(mVAOHandle);

	// enable "vertex attribute arrays"
	glEnableVertexAttribArray(0); // position
	glEnableVertexAttribArray(1); // normal
	glEnableVertexAttribArray(2); // texCoord

	// map index 0 to position buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOHandles[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHandles[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	glBindBuffer(GL_ARRAY_BUFFER, VBOHandles[2]);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);

	glBindVertexArray(0); // disable VAO
	glUseProgram(0); // disable shader programme

}

void Terrain::RenderGeometry(uint shaderProg)
{
	glBindVertexArray(mVAOHandle);
	glDrawElements( GL_TRIANGLES, (mWidth-1)*(mHeight-1)*6, GL_UNSIGNED_INT, this->mIndices);
}

void Terrain::Render(uint shaderProg)
{
	mShader->UpdateUniform("Material.Ka", shaderProg, vec3(0.3, 0.3, 0.3));
	mShader->UpdateUniform("Material.Kd", shaderProg, vec3(0.8, 0.8, 0.8));
	mShader->UpdateUniform("Material.Ks", shaderProg, vec3(0.0, 0.0, 0.0));
	mShader->UpdateUniform("Material.Shininess", shaderProg, 0.8f);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mTexureHandle);

	glBindVertexArray(mVAOHandle);
	glDrawElements( GL_TRIANGLES, (mWidth-1)*(mHeight-1)*6, GL_UNSIGNED_INT, this->mIndices);
}