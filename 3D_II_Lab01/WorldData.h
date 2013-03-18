#ifndef _WORLDDATA_H_
#define _WORLDDATA_H_
#include "includes.h"
using glm::vec2;

struct VertexPoint
{
	vec3 position;
	vec2 texCoord;
	vec3 normal;
	
	VertexPoint(vec3 pos, vec2 tCoord, vec3 norm)
	{
		position = pos;
		texCoord = tCoord;
		normal = norm;
	}

	VertexPoint(vec3 pos, vec3 norm)
	{
		position = pos;
		normal = norm;
	}

	VertexPoint(vec3 pos)
	{
		position = pos;
	}
};



#endif