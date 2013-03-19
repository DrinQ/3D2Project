#include "ObjectHandler.h"


ObjectHandler::ObjectHandler(void)
{
}


ObjectHandler::~ObjectHandler(void)
{
}

void ObjectHandler::CreateObjects()
{
	mGroundQuad = Object3D(vec3(0), 300.0, vec3(0.0));
	mGroundQuad.CreateQuad();
	mGroundQuad.LoadTexture("../Textures/blueStone.jpg", "JPG");

	mBthObject = Object3D(vec3(70, 30, 0), 0.5f, vec3(0.0));
	mBthObject.CreateObjFromFile("../Objects/bth.obj");
	mBthObject.LoadTexture("../Textures/bthcolor.dds", "DDS");

	mHouse = Object3D(vec3(-30, 0, -45), 0.08f, vec3(0.0, 25.0, 0.0));
	mHouse.CreateObjFromFile("../Objects/house_obj.obj");
	mHouse.LoadTexture("../Textures/house_diffuse.tga", "TGA");
	
	for(int i = 0; i < 5; i++)
	{
		mTreeList.push_back(Object3D(vec3(20-i*20, 0, 50+i*i), (rand() % 140 + 100)*0.01f, vec3(0.0)));
	}
	mTreeList[0].CreateObjFromFile("../Objects/gran.obj");
	mTreeList[0].LoadTexture("../Textures/gran.png", "PNG");
}

void ObjectHandler::UpdateObjects()
{
}

void ObjectHandler::RenderShadowingObjects(uint shaderProg)
{
}