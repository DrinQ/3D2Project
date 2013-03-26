#ifndef _OBJLOADER_H_
#define _OBJLOADER_H_
#include "includes.h"
#include "WorldData.h"
#include "MeshMaterialData.h"
#include <map>
using glm::vec2;



struct ObjGroupData
{
	struct Triangle
	{
		UINT32 V[3][3];
	};

	string Name;
	string Material;
	vector<VertexPoint> mVertices;
};

typedef std::map<std::string, MeshMaterialData*> MAP_MATERIAL;
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

	void SetTangents(int startIndex);

	HRESULT ParseMaterialFile(std::ifstream& f, std::string fileDir);

public:
	
	ObjLoader(void);
	~ObjLoader(void);

	void InitObject(char* filename);
	HRESULT Load(char* filename);

	ObjGroupData* GetGroup(uint index);
	MeshMaterialData* GetMaterial(string name);

	int GetGroupCount()
	{ return mGroups.size(); }
	
	vector<VertexPoint> GetVertexPoints()
		{ return vertexPoints; }

};

#endif