#include "ObjLoader.h"


ObjLoader::ObjLoader(void)
{
	mCurrentGroup = NULL;
	mNumGroupsWithNoName = 0;
	mMaterialReadAfterGroup = false;
}


ObjLoader::~ObjLoader(void)
{
}

void ObjLoader::InitObject(char* filename)
{
	Load(filename);
}

HRESULT ObjLoader::Load(char* filename)
{
	std::ifstream f;
	f.open(filename);

	if(!f)
	{
		printf("ERROR creating opening shader file %s\n", filename);
		return E_FAIL;
	}

	string str;
	while(!f.eof())
	{
		f >> str;

			 if(str == "#" || str=="s")	ParseComment(f);
 		else if(str == "v")				ParsePosition(f);
		else if(str == "vn")			ParseNormal(f);
		else if(str == "vt")			ParseTexCoord(f);
		else if(str == "g")				ParseGroup(f);
		else if(str == "f")				ParseFace(f);
		else if(str == "usemtl")		ParseMaterial(f); 
		
			
		else if(str == "mtllib")
		{
			std::string dir = filename;
			dir = dir.substr(0, dir.find_last_of("/") + 1);
			if(FAILED(ParseMaterialFile(f, dir)))
				return E_FAIL;
		}

		str = "";
	}

	SetTangents(0);

	//cout << "vertices object: " << vertexPoints.size() << endl;
	return S_OK;
}

void ObjLoader::ParseComment(std::ifstream& f)
{
	std::string str;

	getline(f, str);
}

void ObjLoader::ParsePosition(std::ifstream& f)
{
	vec3 p;

	f >> p.x;
	f >> p.y;
	f >> p.z;

	mPositions.push_back(p);
}

void ObjLoader::ParseTexCoord(std::ifstream& f)
{
	vec2 t;

	f >> t.x;
	f >> t.y;

	mTexCoords.push_back(t);
}

void ObjLoader::ParseNormal(std::ifstream& f)
{
	vec3 n;

	f >> n.x;
	f >> n.y;
	f >> n.z;
	//n.z *= -1.0f;

	mNormals.push_back(n);
}

void ObjLoader::SetTangents(int startIndex)
{
	vector<VertexPoint>* vertices = &(mCurrentGroup->mVertices);
	for(int i = 0 ; i < mCurrentGroup->mVertices.size(); i+=3)
	{
		vec3 v0 = vertices->at(i+0).position;
		vec3 v1 = vertices->at(i+1).position;
		vec3 v2 = vertices->at(i+2).position;

		vec2 uv0 = vertices->at(i+0).texCoord;
		vec2 uv1 = vertices->at(i+1).texCoord;
		vec2 uv2 = vertices->at(i+2).texCoord;

		vec3 deltaPos1 = v1 - v0;
		vec3 deltaPos2 = v2 - v0;

		vec2 deltaUV1 = uv1 - uv0;
		vec2 deltaUV2 = uv2 - uv0;

		float r = 1.0 / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);

		vec3 tangent = (deltaPos1 * deltaUV2.y   - deltaPos2 * deltaUV1.y)*r;
		//vec3 bitangent = (deltaPos2 * deltaUV1.x   - deltaPos1 * deltaUV2.x)*r;


		mCurrentGroup->mVertices[i+0].tangent,  
			mCurrentGroup->mVertices[i+1].tangent, 
			mCurrentGroup->mVertices[i+2].tangent = tangent;
	}

}

void ObjLoader::ParseFace(std::ifstream& f)
{
	std::string tri;
	std::string sub;
	int count = 0;
	for(int i = 0; i < 3; ++i)
	{
		f >> tri;
		int v[3];
		for(int j = 0; j < 3; ++j)
		{
			sub = tri.substr(0, tri.find("/"));
			int off = tri.find("/");
			tri = tri.substr(off + 1);
			v[j] = atoi(sub.c_str()) - 1;
			
			if ( off < 0 )
				break;
		}
		vec3 vp		= vec3(0,0,0);
		vec3 vn		= vec3(0,0,0);
		vec2 vt		= vec2(0,0);

		if ( v[0] >= 0 )
			vp = mPositions[v[0]];
		if ( v[1] >= 0 )
			vt = mTexCoords[v[1]];
		if ( v[2] >= 0 )
			vn = mNormals[v[2]];

		//vertexPoints.push_back(VertexPoint(vp,vt,vn));
		mCurrentGroup->mVertices.push_back(VertexPoint(vp,vt,vn));
		count++;
	}
	//SetTangents(mCurrentGroup->mVertices.size() - 3);


	if(f.peek() != '\n')
	{
		string str;
		f >> str;
		int v[3];
		for(int j = 0; j < 3; ++j)
		{
			sub = str.substr(0, str.find("/"));
			int off = str.find("/");
			str = str.substr(off + 1);
			v[j] = atoi(sub.c_str()) - 1;
			if ( off < 0 )
				break;
		}
		vec3 vp		= vec3(0,0,0);
		vec3 vn		= vec3(0,0,0);
		vec2 vt		= vec2(0,0);

		if ( v[0] >= 0 )
			vp = mPositions[v[0]];
		if ( v[1] >= 0 )
			vt = mTexCoords[v[1]];
		if ( v[2] >= 0 )
			vn = mNormals[v[2]];

		//vertexPoints.push_back(vertexPoints[vertexPoints.size()-3]);
		//vertexPoints.push_back(vertexPoints[vertexPoints.size()-2]);
		//vertexPoints.push_back(VertexPoint(vp,vt,vn));

		mCurrentGroup->mVertices.push_back(mCurrentGroup->mVertices[mCurrentGroup->mVertices.size()-3]);
		mCurrentGroup->mVertices.push_back(mCurrentGroup->mVertices[mCurrentGroup->mVertices.size()-2]);
		mCurrentGroup->mVertices.push_back(VertexPoint(vp,vt,vn));

		//SetTangents(mCurrentGroup->mVertices.size() - 3);
	}
}

void ObjLoader::ParseMaterial(std::ifstream& f)
{
	std::string str;

	if(mMaterialReadAfterGroup == true)
	{
		MAP_GROUPS::iterator mIter;
		do
		{
			char tmp[20];
			sprintf_s(tmp, sizeof(tmp), "%d", mNumGroupsWithNoName++);
			std::string groupName = tmp;
			mIter = mGroups.find(groupName);
			
			//check if material is already there
			if(mIter == mGroups.end())
			{
				mCurrentGroup = new ObjGroupData;
				mCurrentGroup->Name = groupName;
				mGroups[groupName] = mCurrentGroup;
			}
		} while(mIter != mGroups.end());
		
		mMaterialReadAfterGroup = false;
	}
	else
	{
		mMaterialReadAfterGroup = true;
	}

	getline(f, str);
	btrim(str);

	mCurrentGroup->Material = str;
}

void ObjLoader::ParseGroup(std::ifstream& f)
{
	std::string str;

	getline(f, str);
	btrim(str);

	MAP_GROUPS::const_iterator mIter = mGroups.find(str);

	//check if material is already there
	if(mIter == mGroups.end())
	{
		mCurrentGroup = new ObjGroupData;
		mCurrentGroup->Name = str;
		mGroups[str] = mCurrentGroup;
	}
	else
	{
		mCurrentGroup = mIter->second;
	}

	mMaterialReadAfterGroup = false;
}

void ObjLoader::ltrim(std::string& str, const std::locale& loc)
{
	string::size_type pos = 0;
	
	while (pos < str.size() && isspace(str[pos]))
		pos++;
	
	str.erase(0, pos);
} 

void ObjLoader::rtrim(std::string& str, const std::locale& loc)
{
	string::size_type pos = str.size(); 
	
	while (pos > 0 && isspace(str[pos - 1]))
		pos--; 
	
	str.erase(pos); 
} 

void ObjLoader::btrim(std::string& str, const std::locale& loc)
{
	ltrim(str, loc);
	rtrim(str, loc);
} 

HRESULT ObjLoader::ParseMaterialFile(std::ifstream& f, std::string fileDir)
{
	MeshMaterialData* md = NULL;
	std::string str;

	//get material filename
	getline(f, str);
	btrim(str);

	//append directory in front of filename
	str = fileDir + str;
	std::ifstream mf;
	mf.open(str.c_str());

	if(!mf)
	{
		MessageBoxA(0, "Can't open material file", str.c_str(), MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	while(!mf.eof())
	{
		mf >> str;
		if(str == "illum")
		{
			mf >> md->Illum;
		}
		else if(str == "Kd")
		{
			for(UINT32 i = 0; i < 3; ++i)
				mf >> md->Kd[i];
		}
		else if(str == "Ka")
		{
			for(UINT32 i = 0; i < 3; ++i)
				mf >> md->Ka[i];
		}
		else if(str == "Ks")
		{
			for(UINT32 i = 0; i < 3; ++i)
				mf >> md->Ks[i];
		}
		else if(str == "Tf")
		{
			for(UINT32 i = 0; i < 3; ++i)
				mf >> md->Tf[i];
		}
		else if(str == "map_Kd")
		{
			getline(mf, str);
			btrim(str);

			md->map_Kd = fileDir + str;
		}
		else if(str == "map_Ks")
		{
			getline(mf, str);
			btrim(str);

			md->map_Ks = fileDir + str;
		}
		else if(str == "bump")
		{
			getline(mf, str);
			btrim(str);
			//cut last crap :-)
			md->bump = fileDir + str;
		}
		else if(str == "disp")
		{
			getline(mf, str);
			btrim(str);
			//cut last crap :-)
			md->disp = fileDir + str;
		}
		else if(str == "occlusion")
		{
			getline(mf, str);
			btrim(str);
			//cut last crap :-)
			md->occlusion = fileDir + str;
		}
		else if(str == "Ni")
		{
			mf >> md->Ni;
		}
		else if(str == "Ns")
		{
			mf >> md->Ns;
		}
		else if(str == "newmtl")
		{
			//mf >> str;
			getline(mf, str);
			btrim(str);
			

			MAP_MATERIAL::const_iterator mIter = mMaterials.find(str);

			//check if material is already there
			if(mIter == mMaterials.end())
			{
				md = new MeshMaterialData;
				md->Name = str;
				mMaterials[str] = md;
			}
			else
			{
				md = mIter->second;
			}
		}
	}

	return S_OK;
}

ObjGroupData* ObjLoader::GetGroup(uint index)
{
	if(index > mGroups.size() - 1)
		return NULL;

	MAP_GROUPS::iterator mIter = mGroups.begin();

	while(index-- > 0)
		mIter++;

	return mIter->second;
}

MeshMaterialData* ObjLoader::GetMaterial(string name)
{
	MAP_MATERIAL::const_iterator mIter = mMaterials.find(name);

	//check if material is already there
	if(mIter == mMaterials.end())
		return NULL;
	else
		return mIter->second;
}

