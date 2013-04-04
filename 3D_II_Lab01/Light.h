#ifndef _LIGHT_H_
#define _LIGHT_H_
#include "includes.h"
#include "TextureLoader.h"

class Light
{
private:
	vec3 mWorldPos;
	vec3 mIntensity; //holds the La, Ld and Ls intensities
	vec3 mColor;
	float mDistance; //distance light reaches
	float mSpeed;

public:
	GLuint mVAOHandle;
	GLuint mTextureHandle;

	Light(vec3 pos, vec3 intens, vec3 col, float dist, float speed);
	Light();
	~Light();

	void CreatePointlight(char* file, char* format);
	void UpdateLights(vec3 cameraPos);

	vec3 GetWorldPos()
	{ return mWorldPos; }
	float GetDistance()
	{ return mDistance; }
	vec3 GetIntensity()
	{ return mIntensity; }
	vec3 GetColor()
	{ return mColor; }

	void LoadTexture(char* file, char* format)
{
	if(format == "TGA") mTextureHandle = TextureLoader::LoadTexture_tga(file, GL_TEXTURE0);
	else mTextureHandle = TextureLoader::LoadTexture(file, GL_TEXTURE0);
}
};
#endif
