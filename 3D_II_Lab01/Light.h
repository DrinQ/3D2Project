#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "includes.h"
#include "TextureLoader.h"

class Light
{
private:
	vec3 mWorldPos;
	vec3 mDiffuse;
	vec3 mSpecular;
	float mDistance; //distance light reaches

	float mSpeed;

public:
	GLuint mVAOHandle;
	GLuint mTextureHandle;

	Light(vec3 pos, vec3 diff, vec3 spec, float dist, float speed);
	Light(void);
	~Light(void);

	void CreatePointlight();
	void UpdateLights();

	vec3 GetWorldPos()
	{ return mWorldPos; }
	float GetDistance()
	{ return mDistance; }
	vec3 GetDiffuse()
	{ return mDiffuse; }
	vec3 GetSpecular()
	{ return mSpecular; }

	void LoadTexture(char* file, char* format)
{
	if(format == "TGA") mTextureHandle = TextureLoader::LoadTexture_tga(file, GL_TEXTURE0);
	else mTextureHandle = TextureLoader::LoadTexture(file, GL_TEXTURE0);
}
};
#endif
