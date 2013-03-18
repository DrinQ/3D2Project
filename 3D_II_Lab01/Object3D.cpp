#include "Object3D.h"


Object3D::Object3D()
{
	
}

Object3D::Object3D(vec3 pos, float scale, vec3 rot)
{
	mWorldPos = pos;
	mScale = scale;
	mRotationAngles = rot;

	mTranslation = glm::translate(pos);
	mRotation =	glm::rotate(mRotationAngles.x, vec3(1.0,0.0,0.0)) * glm::rotate(mRotationAngles.y, vec3(0.0,1.0,0.0)) * glm::rotate(mRotationAngles.z, vec3(0.0,0.0,1.0));
	
	mModelMatrix = mTranslation * mRotation * glm::scale(glm::mat4(1.0f), vec3(mScale));
	mMaterial = MaterialInfo(vec3(1.0), vec3(0.8), vec3(0.7), 3.0);
}

Object3D::~Object3D()
{
}

void Object3D::LoadTexture(char* file, char* format)
{
	if(format == "TGA") mTextureHandle = TextureLoader::LoadTexture_tga(file);
	else mTextureHandle = TextureLoader::LoadTexture(file);
}

void Object3D::CreateQuad()
{
	// create VBOs
	// create some 3D points
	float positionData[] = {
	-0.5, 0.0, -0.5,
	-0.5, 0.0, 0.5,
	0.5, 0.0, -0.5,
	0.5, 0.0, 0.5};

	float normalData[] = {
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
	};

	float texCoordData[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	1.0f, 0.0f
	};

	// remember how many
	int numberOfPoints = 12;

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
	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), texCoordData, GL_STATIC_DRAW);

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

void Object3D::CreateObjFromFile(char* filename)
{
	ObjLoader obj;
	obj.Load(filename);
	vertices = obj.GetVertexPoints();

	float* positionData = new float[vertices.size()*3];
	float* texCoordData = new float[vertices.size()*2];
	float* normalData = new float[vertices.size()*3];
	int count = 0;

	for(int i = 0; i < vertices.size(); i++)
	{
		positionData[3*count] = vertices[i].position.x;
		positionData[3*count+1] = vertices[i].position.y;
		positionData[3*count+2] = vertices[i].position.z;

		texCoordData[2*count] = vertices[i].texCoord.x;
		texCoordData[2*count+1] = vertices[i].texCoord.y;

		normalData[3*count] = vertices[i].normal.x;
		normalData[3*count+1] = vertices[i].normal.y;
		normalData[3*count+2] = vertices[i].normal.z;

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

glm::mat4 Object3D::GetModelMatrix()
{
	return mModelMatrix;
}

void Object3D::Update()
{
	mModelMatrix = mTranslation * mRotation * glm::scale(glm::mat4(1.0f), vec3(mScale));
	mRotation =	glm::rotate(mRotationAngles.x, vec3(1.0,0.0,0.0)) * glm::rotate(mRotationAngles.y, vec3(0.0,1.0,0.0)) * glm::rotate(mRotationAngles.z, vec3(0.0,0.0,1.0));
	if(GetAsyncKeyState(VK_UP) != 0)
	{
		mRotationAngles.x++;
	}
	if(GetAsyncKeyState(VK_DOWN) != 0)
	{
		mRotationAngles.x--;
	}
	if(GetAsyncKeyState(VK_LEFT) != 0)
	{
		mRotationAngles.y--;
	}
	if(GetAsyncKeyState(VK_RIGHT) != 0)
	{
		mRotationAngles.y++;
	}
}
