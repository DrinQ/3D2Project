#include "Light.h"



Light::Light(vec3 pos, vec3 diff, vec3 spec, float dist, float speed)
	{
		mWorldPos = pos;
		mDiffuse = diff;
		mSpecular = spec;
		mDistance = dist;
		mSpeed = speed;
	}

Light::Light(void)
{
}

Light::~Light(void)
{
}

void Light::CreatePointlight()
{
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

void Light::UpdateLights()
{
	mWorldPos.x += mSpeed;

	if(mWorldPos.x < -60.0 || mWorldPos.x > 60.0) 
		mSpeed *= -1;

}
