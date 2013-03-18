#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_
#include "includes.h"
#include "WorldData.h"
#include <map>
using glm::vec2;

struct ObjMaterialData
{
	std::string Name;
	UINT32	Illum;
	float Kd[3];
	float Ka[3];
	float Tf[3];
	std::string	map_Kd;
	std::string bump;
	std::string disp;
	std::string occlusion;
	float Ni;

	ObjMaterialData()
	{
		Name = "none";
		map_Kd = "none";
		//bump = "none";
		//disp = "none";
		//occlusion = "none";
	}
};

struct ObjGroupData
{
	struct Triangle
	{
		UINT32 V[3][3];
	};

	std::string Name;
	std::string Material;
	std::vector<Triangle> Triangles;
};

typedef std::map<std::string, ObjMaterialData*> MAP_MATERIAL;
typedef std::map<std::string, ObjGroupData*> MAP_GROUPS;

class ObjLoader
{
private:
	MAP_GROUPS		mGroups;
	MAP_MATERIAL	mMaterials;

	vector<vec3> mPositions;
	vector<vec2> mTexCoords;
	vector<vec3> mNormals;

	ObjGroupData*	mCurrentGroup;

	UINT32			mNumGroupsWithNoName;
	bool			mMaterialReadAfterGroup;

	vector<VertexPoint> vertexPoints;

	void ltrim(std::string& str, const std::locale& loc = std::locale());
	void rtrim(std::string& str, const std::locale& loc = std::locale());
	void btrim(std::string& str, const std::locale& loc = std::locale());

	void ParseComment(std::ifstream& f);
	void ParsePosition(std::ifstream& f);
	void ParseTexCoord(std::ifstream& f);
	void ParseNormal(std::ifstream& f);
	void ParseMaterial(std::ifstream& f);
	void ParseGroup(std::ifstream& f);

	void ParseFace(std::ifstream& f);
	void ParseFace2(std::ifstream& f);
	void ParseFace3(std::ifstream& f);

	HRESULT ParseMaterialFile(std::ifstream& f, std::string fileDir);

public:
	
	ObjLoader(void);
	~ObjLoader(void);

	void InitObject(char* filename);
	HRESULT Load(char* filename);

	

	vector<VertexPoint> GetVertexPoints();
};

#endif