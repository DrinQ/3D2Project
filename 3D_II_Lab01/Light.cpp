#include "Light.h"



Light::Light(vec3 pos, vec3 intens, vec3 col, float dist, float speed)
	{
		mWorldPos = pos;
		mIntensity = intens;
		mColor = col;
		mDistance = dist;
		mSpeed = speed;
	}

Light::Light()
{
}

Light::~Light()
{
}

void Light::CreatePointlight(char* file, char* format)
{
	LoadTexture(file, format);

	float positionData[] = {
	0.0f, 0.0f, 0.0f
	};

	// Make 2 new VBO handles
	GLuint VBOHandles[1];
	glGenBuffers(1, VBOHandles);

	// "Bind" (switch focus to) first buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOHandles[0]); 

	// Put data in currently bound buffer
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float), positionData, GL_STATIC_DRAW);

	// create 1 VAO
		glGenVertexArrays(1, &mVAOHandle);
	glBindVertexArray(mVAOHandle);

	// enable "vertex attribute arrays"
	glEnableVertexAttribArray(0); // position

	// map index 0 to position buffer
	glBindBuffer(GL_ARRAY_BUFFER, VBOHandles[0]);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (GLubyte *)NULL);
	
	glBindVertexArray(0); // disable VAO
	glUseProgram(0); // disable shader programme
}

void Light::UpdateLights(vec3 cameraPos)
{
	
	if(GetAsyncKeyState('I') != 0)
		mWorldPos.z -= mSpeed*2;

	if(GetAsyncKeyState('J') != 0)
		mWorldPos.x -= mSpeed*2;

	if(GetAsyncKeyState('L') != 0)
		mWorldPos.x += mSpeed*2;

	if(GetAsyncKeyState('K') != 0)
		mWorldPos.z += mSpeed*2;
	

	if(GetAsyncKeyState(VK_CONTROL))
		mWorldPos = cameraPos;
}