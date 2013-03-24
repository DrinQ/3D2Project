#ifndef MESHMATERIALDATA_H_
#define MESHMATERIALDATA_H_

#include "includes.h"

struct MeshMaterialData
{
	std::string Name;
	UINT32	Illum;
	vec3 Kd;
	vec3 Ka;
	vec3 Ks;
	vec3 Tf;
	string map_Kd;
	string map_Ks;
	string bump;
	string disp;
	string occlusion;
	float Ni;
	float Ns;

	MeshMaterialData()
	{
		Name = "none";
		map_Kd = "none";
		map_Ks = "none";
		bump = "none";
		//disp = "none";
		//occlusion = "none";
	}
};

#endif