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
	//mMaterial = MaterialInfo(vec3(1.0), vec3(0.8), vec3(0.7), 3.0);
}

Object3D::~Object3D()
{
}

uint Object3D::LoadTexture(char* file, char* format, GLenum textureSlot)
{
	if(format == "TGA") return TextureLoader::LoadTexture_tga(file, textureSlot);
	else return TextureLoader::LoadTexture(file, textureSlot);
}

void Object3D::CreateQuad(char* texFile, char* fileFormat)
{
	GLuint mVAOHandle;
	GLuint mTexHandle = LoadTexture(texFile, fileFormat, GL_TEXTURE0);

	// create VBOs
	// create some 3D points
	float positionData[] = {
	-0.5, 0.0, -0.5,
	-0.5, 0.0, 0.5,
	0.5, 0.0, -0.5,
	-0.5, 0.0, 0.5,
	0.5, 0.0, 0.5,
	0.5, 0.0, -0.5
	};

	float normalData[] = {
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f
	};

	float texCoordData[] = {
	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	1.0f, 1.0f
	
	};

	// remember how many
	int numberOfPoints = 18;

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
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(float), texCoordData, GL_STATIC_DRAW);

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

	MeshMaterialData* material = new MeshMaterialData();

	material->Ka = vec3(1.0);
	material->Kd = vec3(0.8);
	material->Ks = vec3(0.7);
	material->Ns = 3.0;

	mMeshList.push_back(new MeshPart(mVAOHandle, mTexHandle, 0, 0, 6, material));
}

void Object3D::CreateObjFromFile(char* filename)
{
	ObjLoader* objData = new ObjLoader();
	objData->Load(filename);
	ObjGroupData* group = NULL;
	MeshMaterialData* material = NULL;

	for(uint i = 0; i < objData->GetGroupCount(); i++)
	{
		GLuint mVAOHandle;
		GLuint mTexHandle;
		GLuint mBumpHandle;
		GLuint mSpecHandle;

		group = objData->GetGroup(i);
		material = objData->GetMaterial(group->Material);

		//ignore groups that miss material
		if(!material) continue;

		if(material->map_Kd != "none")
		{
			if(material->map_Kd.find(".tga") > 100)
				mTexHandle = TextureLoader::LoadTexture((char*)material->map_Kd.c_str(), GL_TEXTURE0);
			else
				mTexHandle = TextureLoader::LoadTexture_tga((char*)material->map_Kd.c_str(), GL_TEXTURE0);
		}
		else
			mTexHandle = 0;

		if(material->bump != "none")
		{
			if(material->bump.find(".tga") > 100)
				mBumpHandle = TextureLoader::LoadTexture((char*)material->bump.c_str(), GL_TEXTURE2);
			else
				mBumpHandle = TextureLoader::LoadTexture_tga((char*)material->bump.c_str(), GL_TEXTURE2);
		}
		else
			mBumpHandle = 0;

		if(material->map_Ks != "none")
		{
			if(material->map_Ks.find(".tga") > 100)
				mSpecHandle = TextureLoader::LoadTexture((char*)material->map_Ks.c_str(), GL_TEXTURE3);
			else
				mSpecHandle = TextureLoader::LoadTexture_tga((char*)material->map_Ks.c_str(), GL_TEXTURE3);
		}
		else
			mSpecHandle = 0;

		vector<VertexPoint> vertices = group->mVertices;

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

		mMeshList->push_back(new MeshPart(mVAOHandle, mTexHandle, vertices.size(), MaterialInfo(vec3(1.0), vec3(0.8), vec3(0.7), 3.0)));
	}
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

void Object3D::Render(uint shaderProg)
{
	bool hasBump;
	bool hasSpec;
	for(uint i = 0; i < mMeshList.size(); i++)
	{
		mShader->UpdateUniform("Material.Ka", shaderProg, mMeshList[i]->GetMaterialInfo()->Ka);
		mShader->UpdateUniform("Material.Kd", shaderProg, mMeshList[i]->GetMaterialInfo()->Kd);
		mShader->UpdateUniform("Material.Ks", shaderProg, mMeshList[i]->GetMaterialInfo()->Ks);
		mShader->UpdateUniform("Material.Shininess", shaderProg, mMeshList[i]->GetMaterialInfo()->Ns);



		//Check if there's a bump map
		if(mMeshList[i]->GetMaterialInfo()->bump != "none")
		{

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, mMeshList[i]->mBumpMapHandle);
		}
		else
		{
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, 0);

		}

		//Check if there's a specular map
		if(mMeshList[i]->GetMaterialInfo()->map_Ks != "none")
		{

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mMeshList[i]->mSpecMapHandle);
		}
		else
		{
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, 0);

		}

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mMeshList[i]->mTextureHandle);

		glBindVertexArray(mMeshList[i]->mVAOHandle);
		glDrawArrays(GL_TRIANGLES, 0, mMeshList[i]->GetVertexCount());
	}
}