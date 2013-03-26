#ifndef _WORLDDATA_H_
#define _WORLDDATA_H_
#include "includes.h"
using glm::vec2;

struct VertexPoint
{
	vec3 position;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	
	VertexPoint()
	{
	}

	VertexPoint(vec3 pos, vec2 tCoord, vec3 norm)
	{
		position = pos;
		texCoord.x = tCoord.x;
		texCoord.y = -tCoord.y;
		normal = norm;
		tangent = vec3(0,0,0);
	}
};





#endif