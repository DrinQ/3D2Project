#include "SkyBox.h"

SkyBox::SkyBox()
{ }

SkyBox::~SkyBox()
{ }

SkyBox::SkyBox(string mapName, GLuint screenWidth, GLuint screenHeight)
{
	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glGenTextures(1, &mTextureHandle);
	glBindTexture(GL_TEXTURE_CUBE_MAP, mTextureHandle);

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	string suffixes[] = { "posx", "negx", "posy",
								"negy", "posz", "negz" };
	GLuint targets[] = {
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
	};
	
	int width, height, channels;
	for( int i = 0; i < 6; i++ ) {
		string texName = mapName + "_" + suffixes[i] + ".png";

	
		// Load texture file and convert to openGL format
		unsigned char* imgData = SOIL_load_image(texName.c_str(), &width, &height, &channels, 4 );
	
		glTexImage2D(targets[i], 0, GL_RGBA,
					width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

		delete imgData;
	}





	//mProjection = glm::perspective(45.0f, (float)screenWidth / (float)screenHeight, 0.1f, 100.0f); 
	mView       = glm::mat4(1.0f);
	mModel      = glm::scale(glm::mat4(1.0f),glm::vec3(50,50,50));
}

void SkyBox::BindBuffers()
{


	// cube vertices for vertex buffer object
	GLfloat cubeVertices[] = {
	  -1.0,  1.0,  1.0,
	  -1.0, -1.0,  1.0,
	   1.0, -1.0,  1.0,
	   1.0,  1.0,  1.0,
	  -1.0,  1.0, -1.0,
	  -1.0, -1.0, -1.0,
	   1.0, -1.0, -1.0,
	   1.0,  1.0, -1.0,
	};

	// cube indices for index buffer object
	GLuint cubeIndices[] = {
	  /*0, 1, 2, 3,
	  3, 2, 6, 7,
	  7, 6, 5, 4,
	  4, 5, 1, 0,
	  0, 3, 7, 4,
	  1, 2, 6, 5,*/



	  3, 2, 1, 0,
	  7, 6, 2, 3,
	  4, 5, 6, 7,
	  0, 1, 5, 4,
	  4, 7, 3, 0,
	 1, 2, 6, 5,
	};

	glGenVertexArrays(1, &mVAOHandle);
	glBindVertexArray(mVAOHandle);

	GLuint vboCubeVertices;
	glGenBuffers(1, &vboCubeVertices);
	glBindBuffer(GL_ARRAY_BUFFER, vboCubeVertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

	//glEnableVertexAttribArray(0);  // Vertex position.
 

	GLuint iboCubeIndices;
	glGenBuffers(1, &iboCubeIndices);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboCubeIndices);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void SkyBox::Draw(GLuint shaderProgHandle, mat4 projection, mat4 rotation)
{
    GLuint PVMLocation = glGetUniformLocation(shaderProgHandle, "PVM");

	mat4 PVM = projection * mView * mModel * rotation;

	glUniformMatrix4fv(PVMLocation, 1, GL_FALSE, &PVM[0][0]);
	
	glBindTexture(GL_TEXTURE_2D, mTextureHandle);
	glBindVertexArray(mVAOHandle);
	glDrawElements( GL_QUADS, 24, GL_UNSIGNED_INT, 0);
}