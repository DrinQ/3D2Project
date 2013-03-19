#include "Scene.h"



Scene::Scene(int windowWidth, int windowHeight)
{
	mCameraProjectionMat = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 500.f);
	mCam = new Camera();
}

void Scene::CreateShadowMap(int res)
{
	mShadowMap = ShadowMap(mPointLight.GetWorldPos(), mPointLight.GetWorldPos()+vec3(0.0f, -1.0f, -0.01f), res, res);
	mShadowMap.CreateShadowMapTexture();
	mShadowMapRes = res;
}

Scene::Scene()
{
}

Scene::~Scene()
{
}

void Scene::CreateObjects()
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

void Scene::CreateLights()
{
	mPointLight = Light(vec3(0.0f, 85.0f, 15.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.9f, 0.9f, 0.9f), 500.0f, 0.2f);
	mPointLight.CreatePointlight();
	mPointLight.LoadTexture("../Textures/pointLight01.png", "png");
}

void Scene::SetStaticUniforms()
{
	glUseProgram(shaderProgHandle); //standard shader program
	
	//Texture units
	GLuint loc = glGetUniformLocation(shaderProgHandle, "Tex1");
		glUniform1i(loc, 0);
	loc = glGetUniformLocation(shaderProgHandle, "ShadowMap");
		glUniform1i(loc, 1);

	//Light properties
	float maxDist = mPointLight.GetDistance();
	vec4 LightPosition = vec4(mPointLight.GetWorldPos(), 1.0f);	// Light position
	vec3 La = vec3(0.3f, 0.3f, 0.3f);			// Ambient light intensity
	vec3 Ld = mPointLight.GetDiffuse();			// Diffuse light intensity
	vec3 Ls = mPointLight.GetSpecular();			// Specular light intensity

	//Material properties
	MaterialInfo m_Material;
	m_Material.Ka = mGroundQuad.GetMaterialInfo().Ka;			// Ambient reflectivity
	m_Material.Kd = mGroundQuad.GetMaterialInfo().Kd;			// Diffuse reflectivity
	m_Material.Ks = mGroundQuad.GetMaterialInfo().Ks;			// Specular reflectivity
	m_Material.Shininess = mGroundQuad.GetMaterialInfo().Shininess;						// Specular shininess factor
	
	
	//------
	GLuint location = glGetUniformLocation(shaderProgHandle, "maxDist");	//gets the UniformLocation 
	glUniform1fv(location, 1, &maxDist);

	location = glGetUniformLocation(shaderProgHandle, "lightPos");	//gets the UniformLocation 
	glUniform4fv(location, 1, &LightPosition[0]);

	location = glGetUniformLocation(shaderProgHandle, "Light.La");	//gets the UniformLocation
	glUniform3fv(location, 1, &La[0]);

	location = glGetUniformLocation(shaderProgHandle, "Light.Ld");	//gets the UniformLocation 
	glUniform3fv(location, 1, &Ld[0]);

	location = glGetUniformLocation(shaderProgHandle, "Light.Ls");	//gets the UniformLocation 
	glUniform3fv(location, 1, &Ls[0]);

	//-------------------
	location = glGetUniformLocation(shaderProgHandle, "Material.Ka");	//gets the UniformLocation from shader.vertex
	glUniform3fv(location, 1, &m_Material.Ka[0]);

	location = glGetUniformLocation(shaderProgHandle, "Material.Kd");	//gets the UniformLocation from shader.vertex
	glUniform3fv(location, 1, &m_Material.Kd[0]);

	location = glGetUniformLocation(shaderProgHandle, "Material.Ks");	//gets the UniformLocation from shader.vertex
	glUniform3fv(location, 1, &m_Material.Ks[0]);

	location = glGetUniformLocation(shaderProgHandle, "Material.Shininess");	//gets the UniformLocation from shader.vertex
	glUniform1fv(location, 1, &m_Material.Shininess);

}

void Scene::SetShadowMatrices(mat4 model)
{
	mat4 Projection = mShadowMap.GetProjectionMatrix();
	mat4 viewMatrix = mShadowMap.GetViewMatrix(); 
	mat4 ModelView = viewMatrix * model; 

	GLuint location = glGetUniformLocation(shaderProgHandle, "ModelViewMatrix");	//gets the UniformLocation from shader.vertex
	if( location >= 0 )
	{ glUniformMatrix4fv(location, 1, GL_FALSE, &ModelView[0][0]); }

	location = glGetUniformLocation(shaderProgHandle, "ProjectionMatrix");	//gets the UniformLocation from shader.vertex
	if( location > 0 )
	{ glUniformMatrix4fv(location, 1, GL_FALSE, &Projection[0][0]); }
}

void Scene::SetValues(mat4 model)
{
	//matrices
//	mat4 Projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 500.f);
	mat4 viewMatrix = mCam->GetCamViewMatrix();
	//mat4 Projection = mShadowMap.GetProjectionMatrix();
	//mat4 viewMatrix = mShadowMap.GetViewMatrix();
	mat4 ModelView = viewMatrix * model; 
	mat4 ShadowMatrix = mShadowMap.GetBiasMatrix() * mShadowMap.GetProjectionMatrix() * mShadowMap.GetViewMatrix() * model;
	mat3 normalMatrix = glm::transpose(glm::inverse(mat3(ModelView)));

	vec4 lightPos = vec4(mPointLight.GetWorldPos(), 1.0f);

	//Update uniforms
	GLuint location = glGetUniformLocation(shaderProgHandle, "lightPos");	//gets the UniformLocation 
	glUniform4fv(location, 1, &lightPos[0]);

	location = glGetUniformLocation(shaderProgHandle, "ShadowMatrix");	//gets the UniformLocation from shader.vertex
	if( location >= 0 )
	{ glUniformMatrix4fv(location, 1, GL_FALSE, &ShadowMatrix[0][0]); }

	location = glGetUniformLocation(shaderProgHandle, "ViewMatrix");	//gets the UniformLocation from shader.vertex
	if( location >= 0 )
	{ glUniformMatrix4fv(location, 1, GL_FALSE, &viewMatrix[0][0]); }

	location = glGetUniformLocation(shaderProgHandle, "ModelViewMatrix");	//gets the UniformLocation from shader.vertex
	if( location >= 0 )
	{ glUniformMatrix4fv(location, 1, GL_FALSE, &ModelView[0][0]); }

	location = glGetUniformLocation(shaderProgHandle, "NormalMatrix");	//gets the UniformLocation from shader.vertex
	if( location >= 0 )
	{ glUniformMatrix3fv(location, 1, GL_FALSE, &normalMatrix[0][0]); }
}

void Scene::SetLightValues()
{
	//matrices
	//mat4 Projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 500.f);
	mat4 Model = glm::translate(mPointLight.GetWorldPos());
	mat4 viewMatrix = mCam->GetCamViewMatrix();
	mat4 ModelView = viewMatrix * Model; 

	vec4 lightColor = vec4(mPointLight.GetSpecular(), 1.0f) *1.1f;
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
	mPointLight.UpdateLights();
	mShadowMap.SetLightPos(mPointLight.GetWorldPos());
	mBthObject.Update();
}

void Scene::RenderShadowingObjects()
{
	recordDepthIndex = glGetSubroutineIndex(shaderProgHandle, GL_FRAGMENT_SHADER, "recordDepth");

	glViewport(0, 0, mShadowMapRes, mShadowMapRes);
	glUseProgram(shaderProgHandle);

	glBindFramebuffer(GL_FRAMEBUFFER, mShadowMap.GetShadowFBOHandle());
	glClear(GL_DEPTH_BUFFER_BIT);
	glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &recordDepthIndex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0); 
	glCullFace(GL_FRONT);

	//----Render-----------
	SetShadowMatrices(mGroundQuad.GetModelMatrix());
	glBindVertexArray(mGroundQuad.mVAOHandle);
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);

	SetShadowMatrices(mBthObject.GetModelMatrix());
	glBindVertexArray(mBthObject.mVAOHandle); // bind VAO
	glDrawArrays( GL_TRIANGLES, 0, mBthObject.GetVertexList()->size());

	SetShadowMatrices(mHouse.GetModelMatrix());
	glBindVertexArray(mHouse.mVAOHandle); // bind VAO
	glDrawArrays( GL_TRIANGLES, 0, mHouse.GetVertexList()->size());

	glBindVertexArray(mTreeList[0].mVAOHandle); // bind VAO
	for(UINT i = 0; i < mTreeList.size(); i++)
	{
		SetShadowMatrices(mTreeList[i].GetModelMatrix());
		glDrawArrays( GL_TRIANGLES, 0, mTreeList[0].GetVertexList()->size());
	}
}

void Scene::RenderObjects()
{
	shadeWithShadowIndex = glGetSubroutineIndex(shaderProgHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

	//glUseProgram(shaderProgHandle);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &shadeWithShadowIndex);
	
	glBindTexture(GL_TEXTURE_2D, mShadowMap.GetDepthTexHandle()); 
	glActiveTexture(GL_TEXTURE0);
	glCullFace(GL_BACK);

	glUniformMatrix4fv(glGetUniformLocation(shaderProgHandle, "ProjectionMatrix"), 1, GL_FALSE, &mCameraProjectionMat[0][0]);

	
	//-------Render------
	float tileSize = 0.15f;
	glUniform1fv(glGetUniformLocation(shaderProgHandle, "TileSize"), 1, &tileSize);

	SetValues(mGroundQuad.GetModelMatrix());
	glBindTexture(GL_TEXTURE_2D, mGroundQuad.mTextureHandle);
	glBindVertexArray(mGroundQuad.mVAOHandle); 
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4); 

	tileSize = 1.0;
	glUniform1fv(glGetUniformLocation(shaderProgHandle, "TileSize"), 1, &tileSize);

	SetValues(mBthObject.GetModelMatrix());
	glBindTexture(GL_TEXTURE_2D, mBthObject.mTextureHandle);
	glBindVertexArray(mBthObject.mVAOHandle); // bind VAO
	glDrawArrays( GL_TRIANGLES, 0, mBthObject.GetVertexList()->size());

	SetValues(mHouse.GetModelMatrix());
	glBindTexture(GL_TEXTURE_2D, mHouse.mTextureHandle);
	glBindVertexArray(mHouse.mVAOHandle); // bind VAO
	glDrawArrays( GL_TRIANGLES, 0, mHouse.GetVertexList()->size());

	glBindTexture(GL_TEXTURE_2D, mTreeList[0].mTextureHandle);
	glBindVertexArray(mTreeList[0].mVAOHandle); // bind VAO
	for(UINT i = 0; i < mTreeList.size(); i++)
	{
		SetValues(mTreeList[i].GetModelMatrix());
		glDrawArrays( GL_TRIANGLES, 0, mTreeList[0].GetVertexList()->size());
	}
}

void Scene::RenderLightSources()
{
	glUseProgram(billboardShaderProgHandle);

	glUniformMatrix4fv(glGetUniformLocation(billboardShaderProgHandle, "ProjectionMatrix"), 1, GL_FALSE, &mCameraProjectionMat[0][0]);
	
	SetLightValues();

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, mPointLight.mTextureHandle);
	glBindVertexArray(mPointLight.mVAOHandle); // bind VAO
	glDrawArrays( GL_POINTS, 0, 1);
}

void Scene::CreateShaderPrograms()
{
	shaderProgHandle = mShaderHandler.CreateShaderProgram("../Shaders/shader.vertex", "../Shaders/shader.fragment");
	billboardShaderProgHandle = mShaderHandler.CreateShaderProgram("../Shaders/particleShader.vertex", "../Shaders/particleShader.fragment", "../Shaders/particleShader.geometry");
}



