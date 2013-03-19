#ifndef OBJECTHANDLER_H_
#define OBJECTHANDLER_H_
#include "Object3D.h"
#include "ShadowMap.h"

class ObjectHandler
{
private:
	Object3D mBthObject;
	Object3D mGroundQuad;
	Object3D mHouse;
	vector<Object3D> mTreeList;

public:
	
	ObjectHandler(void);
	~ObjectHandler(void);

	void CreateObjects();
	void UpdateObjects();

	void RenderShadowingObjects(uint shaderProg);
	void RenderObjects();

};

#endif