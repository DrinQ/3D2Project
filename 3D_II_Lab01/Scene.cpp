#include "Scene.h"



Scene::Scene(int windowWidth, int windowHeight)
{
	mCameraProjectionMat = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 2000.f);
	mCam = new Camera();

	mSkyBox = SkyBox("../Textures/CubeMaps/skybox", windowWidth, windowHeight);
	mSkyBox.BindBuffers();

	La = 0.8f; //Ambient light
}


Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::CreateObjects()
{
	mTerrain = new Terrain(257, 257, "../Textures/heightMap.raw", 30.0f, 0.0f, 3.0f, "../Textures/seamlessGrass.jpg");

	mGroundQuad = Object3D(vec3(0), 300.0, vec3(0.0));
	mGroundQuad.CreateQuad("../Textures/groundStone.jpg", "JPG");

	mBthObject = Object3D(vec3(70, 0, 0), 0.5f, vec3(0.0));
	mBthObject.CreateObjFromFile("../Objects/PepsiCan/Pepsi_Max_Can.obj");

	mHouse = Object3D(vec3(360, 2, 280), 0.9f, vec3(0.0, 100.0, 0.0));
	mHouse.CreateObjFromFile("../Objects/TestHouse/houseA_obj.obj");
	
	for(int i = 0; i < 1; i++)
	{
		for(int j = 0; j < 3; j++)
			mTreeList.push_back(new Object3D(vec3(230-i*35, 5, 340+i*2 + j*45), (rand() % 140 + 100)*0.08f, vec3(0.0)));
	}
	mTreeList[0]->CreateObjFromFile("../Objects/HQGran/tree1.obj");
	for(int i = 1; i < mTreeList.size(); i++)
	{
		mTreeList[i]->SetMeshList(mTreeList[0]->GetMeshList());
	}
}

void Scene::CreateLights(int shadowMapRes)
{
	mPointLights.push_back(new Light(mHouse.GetPosition()-vec3(37, -102, 26), vec3(0.0f, 1.0f, 0.9f), vec3(0.8f, 0.8f, 1.0f), 500.0f, 0.2f));
	mPointLights[0]->CreatePointlight("../Textures/pointLight01.png", "png");

	mPointLights.push_back(new Light(mHouse.GetPosition()-vec3(130, -112, -90), vec3(0.0f, 1.0f, 0.9f), vec3(1.0f, 0.6f, 0.6f), 500.0f, 0.2f));
	mPointLights.push_back(new Light(vec3(400.0f, 85.0f, 250.0f), vec3(0.0f, 1.0f, 0.9f), vec3(1.0f, 1.0f, 1.0f), 300.0f, 0.2f));

	for(int i = 0; i < mPointLights.size(); i++)
	{
		CreateShadowMaps(i, shadowMapRes);
	}
}

void Scene::CreateShadowMaps(int index, int res)
{
	mShadowMapList.push_back(new ShadowMap(mPointLights[index]->GetWorldPos(), mPointLights[index]->GetWorldPos()+vec3(0.0f, -1.0f, -0.01f), res));
	mShadowMapList[index]->CreateShadowMapTexture(index);
	//mShadowMapRes = res;
}

void Scene::SetStaticUniforms()
{
	glUseProgram(shaderProgHandle); //standard shader program
	
	//Texture units
	GLuint loc = glGetUniformLocation(shaderProgHandle, "Tex1");
		glUniform1i(loc, 0);
	loc = glGetUniformLocation(shaderProgHandle, "BumpMap");
		glUniform1i(loc, 2);
	/*loc = glGetUniformLocation(shaderProgHandle, "SpecMap");
		glUniform1i(loc, 3);*/
	for(int i = 0; i < mShadowMapList.size(); i++)
	{
		char indexStr[3];		itoa(i, indexStr, 10);
		char shadowString[20];
		strcpy(shadowString, "ShadowMaps[");	strcat(shadowString, indexStr);		strcat(shadowString, "]");

		loc = glGetUniformLocation(shaderProgHandle, shadowString);
			glUniform1i(loc, i+5);
	}

	mShaderHandler->UpdateUniform("BiasMatrix", shaderProgHandle, mShadowMapList[0]->GetBiasMatrix());
	mShaderHandler->UpdateUniform("nrOfLights", shaderProgHandle, (int)mPointLights.size());

	for(int i = 0; i < mPointLights.size(); i++)
	{
		float Ld = mPointLights[i]->GetIntensity().y;			// Diffuse light intensity
		float Ls = mPointLights[i]->GetIntensity().z;			// Specular light intensity
		vec3 Color = mPointLights[i]->GetColor();
		float Distance = mPointLights[i]->GetDistance();

		char indexStr[3];		itoa(i, indexStr, 10);
		char intensityStr[25], colorStr[25], distanceStr[25];
		strcpy(intensityStr, "Lights[");	strcat(intensityStr, indexStr);		strcat(intensityStr, "].Intensity");
		strcpy(colorStr, "Lights[");		strcat(colorStr, indexStr);			strcat(colorStr, "].Color");
		strcpy(distanceStr, "Lights[");		strcat(distanceStr, indexStr);		strcat(distanceStr, "].Distance");

		mShaderHandler->UpdateUniform(intensityStr, shaderProgHandle, vec3(La, Ld, Ls));
		mShaderHandler->UpdateUniform(colorStr, shaderProgHandle, Color);
		mShaderHandler->UpdateUniform(distanceStr, shaderProgHandle, Distance);
	}
}

void Scene::SetShadowMatrices(int i, mat4 model)
{
	mat4 Projection = mShadowMapList[i]->GetProjectionMatrix();
	mat4 ModelView = mShadowMapList[i]->GetViewMatrix() * model; 

	mShaderHandler->UpdateUniform("ModelViewMatrix", shadowShaderProgHandle, ModelView);
	mShaderHandler->UpdateUniform("ProjectionMatrix", shadowShaderProgHandle, Projection);
}

void Scene::SetValues(mat4 model)
{
	//matrices
//	mat4 Projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 500.f);
	mat4 viewMatrix = mCam->GetCamViewMatrix();
	//mat4 Projection = mShadowMapList[0]->GetProjectionMatrix();
	//mat4 viewMatrix = mShadowMap.GetViewMatrix();
	//mat4 ModelView = viewMatrix * model; 
	mat3 normalMatrix = glm::transpose(glm::inverse(mat3(viewMatrix * model)));

	vec4 cameraPos = vec4(mCam->GetCamPos(), 1.0f);

	//Update uniforms
	
	mShaderHandler->UpdateUniform("ViewMatrix", shaderProgHandle, viewMatrix);
	mShaderHandler->UpdateUniform("ModelMatrix", shaderProgHandle, model);
	mShaderHandler->UpdateUniform("NormalMatrix", shaderProgHandle, normalMatrix);

	for(int i = 0; i < mPointLights.size(); i++)
	{
		//Light properties
		vec4 LightPosition = vec4(mPointLights[i]->GetWorldPos(), 1.0f);	// Light position
		
		//-----Send all the lights values------
		char indexStr[3];		itoa(i, indexStr, 10);
		char positionStr[25];
		strcpy(positionStr, "Lights[");	 	strcat(positionStr, indexStr);		strcat(positionStr, "].Position");
		
		mShaderHandler->UpdateUniform(positionStr, shaderProgHandle, LightPosition);
		
		//-----for the shadow matrices(same amount as lights for now)-------
		mat4 ShadowViewProj = mShadowMapList[i]->GetProjectionMatrix() * mShadowMapList[i]->GetViewMatrix();

		char shadowStr[20];
		strcpy(shadowStr, "ShadowViewProj[");	 	strcat(shadowStr, indexStr);		strcat(shadowStr, "]");

		mShaderHandler->UpdateUniform(shadowStr, shaderProgHandle, ShadowViewProj);
	}

}

void Scene::SetLightValues(int i)
{
	//matrices
	//mat4 Projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 500.f);
	mat4 Model = glm::translate(mPointLights[i]->GetWorldPos());
	mat4 viewMatrix = mCam->GetCamViewMatrix();
	mat4 ModelView = viewMatrix * Model; 

	vec4 lightColor = vec4(mPointLights[i]->GetColor(), 1.0f) *1.1f;
	float particleSize = 2.0;

	//------------------Variables in particleShader.geometry-------------------
	GLuint location = glGetUniformLocation(billboardShaderProgHandle, "Size2");	//gets the UniformLocation from shader.vertex
	glUniform1fv(location, 1, &particleSize);

	location = glGetUniformLocation(billboardShaderProgHandle, "lightColor");	//gets the UniformLocation from shader.vertex
	glUniform4fv(location, 1, &lightColor[0]);


	//------------------Matrices in particleShader.vertex-------------------------------------
	location = glGetUniformLocation(billboardShaderProgHandle, "ModelViewMatrix");	//gets the UniformLocation from particleShader.vertex
	if( location >= 0 )
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, &ModelView[0][0]);
	}
}

void Scene::Update()
{
	if(GetAsyncKeyState('Q') != 0)
		mPointLights[1]->UpdateLights(mCam->GetCamPos());
	else
		mPointLights[0]->UpdateLights(mCam->GetCamPos());

	for(int i = 0; i < mShadowMapList.size(); i++)
	{
		mShadowMapList[i]->SetLightPos(mPointLights[i]->GetWorldPos());
	}

	mHouse.Update();
}

void Scene::RenderSkyBox()
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glUseProgram(skyboxShaderProgHandle);
	mSkyBox.Draw(skyboxShaderProgHandle, mCameraProjectionMat, mCam->GetCamRotationMatrix());
	glUseProgram(shaderProgHandle);
	glClear(GL_DEPTH_BUFFER_BIT);
}

void Scene::RenderShadowingObjects()
{
	glUseProgram(shadowShaderProgHandle);

	for(int i = 0; i < mShadowMapList.size(); i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, mShadowMapList[i]->GetShadowFBOHandle());
		glClear(GL_DEPTH_BUFFER_BIT);

		glCullFace(GL_FRONT);
		glActiveTexture(GL_TEXTURE5+i);
		glBindTexture(GL_TEXTURE_2D, 0); 
		

		//----Render-----------
		//SetShadowMatrices(i, mTerrain->GetModelMatrix());
		//mTerrain->RenderGeometry();

		SetShadowMatrices(i, mBthObject.GetModelMatrix());
		mBthObject.RenderGeometry();

		SetShadowMatrices(i, mHouse.GetModelMatrix());
		mHouse.RenderGeometry();

		for(UINT j = 0; j < mTreeList.size(); j++)
		{
			SetShadowMatrices(i, mTreeList[j]->GetModelMatrix());
			mTreeList[j]->RenderGeometry();
		}

		SetShadowMatrices(i, mGroundQuad.GetModelMatrix());
		mGroundQuad.RenderGeometry();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Scene::RenderObjects()
{
	glUseProgram(shaderProgHandle);
	
	//glActiveTexture(GL_TEXTURE0);
	glCullFace(GL_BACK);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgHandle, "ProjectionMatrix"), 1, GL_FALSE, &mCameraProjectionMat[0][0]);

	for(int i = 0; i < mShadowMapList.size(); i++)
	{
		glActiveTexture(GL_TEXTURE5+i);
		glBindTexture(GL_TEXTURE_2D, mShadowMapList[i]->GetDepthTexHandle()); 
	}

	//-------Render------
	float tileSize = 1.0;
	glUniform1fv(glGetUniformLocation(shaderProgHandle, "TileSize"), 1, &tileSize);

	
	SetValues(mBthObject.GetModelMatrix());
	mBthObject.Render(shaderProgHandle);

	SetValues(mHouse.GetModelMatrix());
	mHouse.Render(shaderProgHandle);

	
	tileSize = 0.1f;
	glUniform1fv(glGetUniformLocation(shaderProgHandle, "TileSize"), 1, &tileSize);

	SetValues(mGroundQuad.GetModelMatrix());
	mGroundQuad.Render(shaderProgHandle);

	tileSize = 0.05f;
	glUniform1fv(glGetUniformLocation(shaderProgHandle, "TileSize"), 1, &tileSize);

	SetValues(mTerrain->GetModelMatrix());
	mTerrain->Render(shaderProgHandle);

	tileSize = 1.0f;
	glUniform1fv(glGetUniformLocation(shaderProgHandle, "TileSize"), 1, &tileSize);

	for(UINT j = 0; j < mTreeList.size(); j++)
	{
		SetValues(mTreeList[j]->GetModelMatrix());
		mTreeList[j]->Render(shaderProgHandle);
	}

	//glBindTexture(GL_TEXTURE_2D, 0); 
}

void Scene::RenderLightSources()
{
	glUseProgram(billboardShaderProgHandle);

	glUniformMatrix4fv(glGetUniformLocation(billboardShaderProgHandle, "ProjectionMatrix"), 1, GL_FALSE, &mCameraProjectionMat[0][0]);
	
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, mPointLights[0]->mTextureHandle);
	glBindVertexArray(mPointLights[0]->mVAOHandle); // bind VAO
	for(int i = 0; i < mPointLights.size(); i++)
	{
		SetLightValues(i);
		glDrawArrays( GL_POINTS, 0, 1);
	}
}

void Scene::CreateShaderPrograms()
{
	shaderProgHandle = mShaderHandler->CreateShaderProgram("../Shaders/shader.vertex", "../Shaders/shader.fragment");
	shadowShaderProgHandle = mShaderHandler->CreateShaderProgram("../Shaders/shadowShader.vertex", "../Shaders/shadowShader.fragment");
	billboardShaderProgHandle = mShaderHandler->CreateShaderProgram("../Shaders/particleShader.vertex", "../Shaders/particleShader.fragment", "../Shaders/particleShader.geometry");
	skyboxShaderProgHandle = mShaderHandler->CreateShaderProgram("../Shaders/skyboxShader.vertex", "../Shaders/skyboxShader.fragment");
}



