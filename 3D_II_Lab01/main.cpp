#include "includes.h"
#include <time.h>
#include "TextureLoader.h"

#include "Camera.h"
#include "Object3D.h"
#include "Light.h"
#include "ShadowMap.h"

int windowWidth = 1280;
int windowHeight = 720;
int frameCount = 0;
char windowTitle[128] = "Project 3DII";
int shadowMapRes = 4096; 
Camera cam;

GLuint shaderProgHandle; // integer to get hold of our shader programme
GLuint billboardShaderProgHandle;

Object3D bthObject;
Object3D groundQuad;
Object3D mHouse;
vector<Object3D> mTreeList;

Light pointLight;

ShadowMap mShadowMap;

struct LightInfo{
	vec4 LightPosition; // Light position in eye coords.
	vec3 La;		    // Ambient light intensity
	vec3 Ld;			// Diffuse light intensity
	vec3 Ls;			// Specular light intensity
	};

void My_mouse_routine(int x, int y)
	{
		cam.SetMousePos(x, y); //place current mouse pos in mouseX, mouseY (in Camera class)
	}

//Texture units, light and material properties
void setStaticUniforms()
{
	glUseProgram(shaderProgHandle); //standard shader program
	
	//Texture units
	GLuint loc = glGetUniformLocation(shaderProgHandle, "Tex1");
		glUniform1i(loc, 0);
	loc = glGetUniformLocation(shaderProgHandle, "ShadowMap");
		glUniform1i(loc, 1);

	//Light properties
	float maxDist = pointLight.GetDistance();
	LightInfo m_Light;
	m_Light.LightPosition = vec4(pointLight.GetWorldPos(), 1.0f);	// Light position
	m_Light.La = vec3(0.3f, 0.3f, 0.3f);			// Ambient light intensity
	m_Light.Ld = pointLight.GetDiffuse();			// Diffuse light intensity
	m_Light.Ls = pointLight.GetSpecular();			// Specular light intensity

	//Material properties
	MaterialInfo m_Material;
	m_Material.Ka = groundQuad.GetMaterialInfo().Ka;			// Ambient reflectivity
	m_Material.Kd = groundQuad.GetMaterialInfo().Kd;			// Diffuse reflectivity
	m_Material.Ks = groundQuad.GetMaterialInfo().Ks;			// Specular reflectivity
	m_Material.Shininess = groundQuad.GetMaterialInfo().Shininess;						// Specular shininess factor
	
	
	//------
	GLuint location = glGetUniformLocation(shaderProgHandle, "maxDist");	//gets the UniformLocation 
	glUniform1fv(location, 1, &maxDist);

	location = glGetUniformLocation(shaderProgHandle, "lightPos");	//gets the UniformLocation 
	glUniform4fv(location, 1, &m_Light.LightPosition[0]);

	location = glGetUniformLocation(shaderProgHandle, "Light.La");	//gets the UniformLocation
	glUniform3fv(location, 1, &m_Light.La[0]);

	location = glGetUniformLocation(shaderProgHandle, "Light.Ld");	//gets the UniformLocation 
	glUniform3fv(location, 1, &m_Light.Ld[0]);

	location = glGetUniformLocation(shaderProgHandle, "Light.Ls");	//gets the UniformLocation 
	glUniform3fv(location, 1, &m_Light.Ls[0]);

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

void setShadowMatrices(mat4 model)
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

void setValues(mat4 model)
{
	//matrices
	mat4 Projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 500.f);
	mat4 viewMatrix = cam.GetCamViewMatrix();
	//mat4 Projection = mShadowMap.GetProjectionMatrix();
	//mat4 viewMatrix = mShadowMap.GetViewMatrix();
	mat4 ModelView = viewMatrix * model; 
	mat4 ShadowMatrix = mShadowMap.GetBiasMatrix() * mShadowMap.GetProjectionMatrix() * mShadowMap.GetViewMatrix() * model;
	mat3 normalMatrix = glm::transpose(glm::inverse(mat3(ModelView)));

	vec4 lightPos = vec4(pointLight.GetWorldPos(), 1.0f);

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

void setLightValues()
{
	//matrices
	//mat4 Projection = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 500.f);
	mat4 Model = glm::translate(pointLight.GetWorldPos());
	mat4 viewMatrix = cam.GetCamViewMatrix();
	mat4 ModelView = viewMatrix * Model; 

	vec4 lightColor = vec4(pointLight.GetSpecular(), 1.0f) *1.2f;
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

void renderCallback() 
{
	frameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffer using colour

	GLuint recordDepthIndex = glGetSubroutineIndex(shaderProgHandle, GL_FRAGMENT_SHADER, "recordDepth");
	GLuint shadeWithShadowIndex = glGetSubroutineIndex(shaderProgHandle, GL_FRAGMENT_SHADER, "shadeWithShadow");

	glViewport(0, 0, shadowMapRes, shadowMapRes);
	glUseProgram(shaderProgHandle);

	glBindFramebuffer(GL_FRAMEBUFFER, mShadowMap.GetShadowFBOHandle());
	glClear(GL_DEPTH_BUFFER_BIT);
	glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &recordDepthIndex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, 0); 
	glCullFace(GL_FRONT);
	
	//---1st time---- from light perspective----------------------------------------
	setShadowMatrices(groundQuad.GetModelMatrix());
	glBindVertexArray(groundQuad.mVAOHandle);
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4);

	setShadowMatrices(bthObject.GetModelMatrix());
	glBindVertexArray(bthObject.mVAOHandle); // bind VAO
	glDrawArrays( GL_TRIANGLES, 0, bthObject.GetVertexList()->size());

	setShadowMatrices(mHouse.GetModelMatrix());
	glBindVertexArray(mHouse.mVAOHandle); // bind VAO
	glDrawArrays( GL_TRIANGLES, 0, mHouse.GetVertexList()->size());

	glBindVertexArray(mTreeList[0].mVAOHandle); // bind VAO
	for(UINT i = 0; i < mTreeList.size(); i++)
	{
		setShadowMatrices(mTreeList[i].GetModelMatrix());
		glDrawArrays( GL_TRIANGLES, 0, mTreeList[0].GetVertexList()->size());
	}

	//----------------------------------------------------------------------------------------

	glViewport(0, 0, windowWidth, windowHeight);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUniformSubroutinesuiv( GL_FRAGMENT_SHADER, 1, &shadeWithShadowIndex);
	
	glBindTexture(GL_TEXTURE_2D, mShadowMap.GetDepthTexHandle()); 
	glActiveTexture(GL_TEXTURE0);
	glCullFace(GL_BACK);

	
	//flytta till objectHandler-klass sen-----
	mat4 projectionMatrix = glm::perspective(45.0f, (float)windowWidth / (float)windowHeight, 1.0f, 500.f);
	glUniformMatrix4fv(glGetUniformLocation(shaderProgHandle, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
	//----------------------------------------

	//---2nd time----- from camera perspective------------------------------------------------
	float tileSize = 0.2f;
	glUniform1fv(glGetUniformLocation(shaderProgHandle, "TileSize"), 1, &tileSize);

	setValues(groundQuad.GetModelMatrix());
	glBindTexture(GL_TEXTURE_2D, groundQuad.mTextureHandle);
	glBindVertexArray(groundQuad.mVAOHandle); 
	glDrawArrays( GL_TRIANGLE_STRIP, 0, 4); 

	tileSize = 1.0;
	glUniform1fv(glGetUniformLocation(shaderProgHandle, "TileSize"), 1, &tileSize);

	setValues(bthObject.GetModelMatrix());
	glBindTexture(GL_TEXTURE_2D, bthObject.mTextureHandle);
	glBindVertexArray(bthObject.mVAOHandle); // bind VAO
	glDrawArrays( GL_TRIANGLES, 0, bthObject.GetVertexList()->size());

	setValues(mHouse.GetModelMatrix());
	glBindTexture(GL_TEXTURE_2D, mHouse.mTextureHandle);
	glBindVertexArray(mHouse.mVAOHandle); // bind VAO
	glDrawArrays( GL_TRIANGLES, 0, mHouse.GetVertexList()->size());

	glBindTexture(GL_TEXTURE_2D, mTreeList[0].mTextureHandle);
	glBindVertexArray(mTreeList[0].mVAOHandle); // bind VAO
	for(UINT i = 0; i < mTreeList.size(); i++)
	{
		setValues(mTreeList[i].GetModelMatrix());
		glDrawArrays( GL_TRIANGLES, 0, mTreeList[0].GetVertexList()->size());
	}
	

//-----------------------switch shader program------------------------
	glUseProgram(billboardShaderProgHandle);
	//flytta till objectHandler-klass sen-----
	glUniformMatrix4fv(glGetUniformLocation(billboardShaderProgHandle, "ProjectionMatrix"), 1, GL_FALSE, &projectionMatrix[0][0]);
	setLightValues(); //select light in pointLights list

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBindTexture(GL_TEXTURE_2D, pointLight.mTextureHandle);
	glBindVertexArray(pointLight.mVAOHandle); // bind VAO
	glDrawArrays( GL_POINTS, 0, 1);

	glutSwapBuffers(); // swap drawing back-buffer to displayed front buffer
	glutPostRedisplay(); // flag for redraw
	
}

void resizeCallback(int width, int height) {
	windowWidth = width; // remember new size
	windowHeight = height;
	glViewport(0, 0, width, height); // change viewport size in gl

}

void timerCallback(int value) {
	if (value != 0) {
		char tmp[128];
		sprintf(tmp, "%s @ %iFPS", windowTitle, frameCount * 4); // update frame counter in window title
		char tmp2[128];
		sprintf(tmp2, "%s RES: %iPxls", tmp, shadowMapRes); // update frame counter in window title
		glutSetWindowTitle(tmp2); // set the window title using freeGLUT
	}
	frameCount = 0;
	glutTimerFunc(250, timerCallback, 1); // start timer again (every 1/4 second)
}

bool compileShader(const char* shaderFileName, const int& type, GLuint& shaderHandle) {
	// load file into string
	ifstream file;
	file.open(shaderFileName);
	if (!file.is_open()) {
		printf("ERROR creating opening shader file %s\n", shaderFileName);
		return false;
	}
	string shaderString; // string to load shader into
	char line[256];
  while (!file.eof()) {
    strcpy_s(line, "");
    file.getline(line, 256);
    shaderString += line;
    shaderString += '\n';
  }
  file.close();

	// create shader object
	if (type == 0) {
		shaderHandle = glCreateShader(GL_VERTEX_SHADER);
	} else if (type == 1) {
		shaderHandle = glCreateShader(GL_FRAGMENT_SHADER);
	} else if (type == 2) {
		shaderHandle = glCreateShader(GL_GEOMETRY_SHADER);
	}
	// validate creation
	if (0 == shaderHandle) {
		printf("ERROR creating shader type %i\n", type);
		return false;
	}

	// load source from a char array
	const char* ptr = shaderString.c_str(); // get character pointer from string
	glShaderSource(shaderHandle, 1, &ptr, NULL);

	// compile shader
	glCompileShader(shaderHandle);

	// Check for errors
	int result = 0;
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &result);
	if(GL_FALSE == result) {
		printf("ERROR compiling shader type %i\n", type);
		int length = 0;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &length);
		if(length > 0) {
			// create a log of error messages
			char* errorLog = new char[length];
			int written = 0;
			glGetShaderInfoLog(shaderHandle, length, &written, errorLog);
			printf("Shader error log;\n%s\n", errorLog);
			delete [] errorLog;
		}
		return false;
	}
	return true;
}

bool createShaderProgramme(const GLuint& shaderAHandle, const GLuint& shaderBHandle, GLuint& shaderProgrammeHandle) {
	// create shader programme
	shaderProgrammeHandle = glCreateProgram();
	if (0 == shaderProgrammeHandle) {
		printf("ERROR creating shader programme\n");
		return false;
	}
	
	// attach shaders
	glAttachShader(shaderProgrammeHandle, shaderAHandle);
	glAttachShader(shaderProgrammeHandle, shaderBHandle);
	
	// link programme
	glLinkProgram(shaderProgrammeHandle);
	
	// verify link status
	GLint status;
	glGetProgramiv(shaderProgrammeHandle, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		printf("ERROR: failed to link shader programme\n");
		return false;
	}
	
	return true;
}

bool createParticleShaderProgramme(const GLuint& shaderAHandle, const GLuint& shaderBHandle, const GLuint& shaderCHandle, GLuint& shaderProgrammeHandle) {
	// create shader programme
	shaderProgrammeHandle = glCreateProgram();
	if (0 == shaderProgrammeHandle) {
		printf("ERROR creating shader programme\n");
		return false;
	}
	
	// attach shaders
	glAttachShader(shaderProgrammeHandle, shaderAHandle);
	glAttachShader(shaderProgrammeHandle, shaderBHandle);
	glAttachShader(shaderProgrammeHandle, shaderCHandle);
	
	// link programme
	glLinkProgram(shaderProgrammeHandle);
	
	// verify link status
	GLint status;
	glGetProgramiv(shaderProgrammeHandle, GL_LINK_STATUS, &status);
	if (GL_FALSE == status) {
		printf("ERROR: failed to link shader programme\n");
		return false;
	}
	
	return true;
}

void Initialize()
{
	// create Window on O/S using freeGLUT
	glutInitWindowSize(windowWidth, windowHeight); // specify size of window
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);

	// create window with freeGLUT
	int windowHandle = glutCreateWindow(windowTitle);
	if (windowHandle < 1) { printf("ERROR: creating window\n"); exit(1); }

	// get version info and print it to the console (have a look to see what version you can support)
	const GLubyte* renderer = glGetString(GL_RENDERER); // get renderer string
	const GLubyte* vendor = glGetString(GL_VENDOR); // vendor
	const GLubyte* version = glGetString(GL_VERSION); // version as a string
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION); // glsl version string
	GLint major, minor;
	glGetIntegerv(GL_MAJOR_VERSION, &major); // get integer (only if gl version > 3)
	glGetIntegerv(GL_MINOR_VERSION, &minor); // get dot integer (only if gl version > 3)
	printf("OpenGL on %s %s\n", vendor, renderer);
	printf("OpenGL version supported %s\n", version);
	printf("GLSL version supported %s\n", glslVersion);
	printf("Will now set GL to version %i.%i\n", major, minor);
	
	// force GL to use a specific version
	glutInitContextVersion(major, minor); // set version of gl to 4.1
	glutInitContextFlags(GLUT_FORWARD_COMPATIBLE); // do not allow deprecated
	glutInitContextProfile(GLUT_CORE_PROFILE); // force current version of spec
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE, GLUT_ACTION_GLUTMAINLOOP_RETURNS);

	// start GLEW (that's it! - we could also use GLEW to check for driver support of all GL extensions)
	GLenum glErr = glGetError();
	while (glErr != GL_NO_ERROR) {
		printf("glError: %s\n", gluErrorString(glErr));
	}

	// set-up callbacks. we can also do a keyboard and mouse input callback, and various others (see freeGLUT website)
	glutDisplayFunc(renderCallback); // register rendering callback
	glutReshapeFunc(resizeCallback); // register callback for reshape
	glutTimerFunc(0, timerCallback, 0); // register a timer callback

	// set colour to clear screen buffer to
	glClearColor(0.05f, 0.05f, 0.05f, 1.0f);

	// enable some useful GL behaviours
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // set depth-testing function type
	glEnable(GL_CULL_FACE); // enable culling of back-faces
	glCullFace(GL_BACK); // enable culling of back-faces
	glFrontFace(GL_CCW); // use clock-wise 'winding' to define order used to make fronts of polygons
	glEnable(GL_BLEND);
	
	// start GLEW (that's it! - we could also use GLEW to check for driver support of all GL extensions)
	GLenum err = glewInit();
	if (GLEW_OK != err) {
		printf("ERROR starting GLEW: %s\n", glewGetErrorString(err));
	}
}

int CreateAndLinkShaderPrograms()
{
	//-------------------------------------------------------------------------------------------------
	// load and compile shaders from text files - if there an error starting it is most likely a path here
	GLuint vertexHandle, fragmentHandle;
	if (!compileShader("../Shaders/shader.vertex", 0, vertexHandle)) { // this should be the path when running from inside visual studio
		if (!compileShader("shader.vertex", 0, vertexHandle)) { return 1; } // but if that didn't work try the working directory
	}
	if (!compileShader("../Shaders/shader.fragment", 1, fragmentHandle)) {
		if (!compileShader("shader.fragment", 1, fragmentHandle)) { return 1; }
	}
	// link shader programme
	if (!createShaderProgramme(vertexHandle, fragmentHandle, shaderProgHandle)) { return 1; }

//--------------------------Create light shader programme-------------------------------------------
	GLuint partVertexHandle, partGeometryHandle, partFragmentHandle;
	if (!compileShader("../Shaders/particleShader.vertex", 0, partVertexHandle)) { // this should be the path when running from inside visual studio
		 return 1;
	}
	if (!compileShader("../Shaders/particleShader.fragment", 1, partFragmentHandle)) {
		 return 1;
	}
	if (!compileShader("../Shaders/particleShader.geometry", 2, partGeometryHandle)) {
		 return 1;
	}
	// link shader programme
	if (!createParticleShaderProgramme(partVertexHandle, partGeometryHandle, partFragmentHandle, billboardShaderProgHandle)) { return 1; }
//--------------------------------------------------------------------------------------------------
	return 0;
}

int main(int argc, char** argv){

	// initialise freeGLUT
	glutInit(&argc, argv); // init freeGLUT with command line param list

	Initialize();
	
	glutSetCursor(GLUT_CURSOR_NONE); //hide mouse cursor
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	if(CreateAndLinkShaderPrograms() == 1) return 1;

	pointLight = Light(vec3(0.0f, 85.0f, 15.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.9f, 0.9f, 0.9f), 500.0f, 0.2f);
	pointLight.CreatePointlight();
	pointLight.LoadTexture("../Textures/pointlight01.png", "png");

	groundQuad = Object3D(vec3(0), 300.0, vec3(0.0));
	groundQuad.CreateQuad();
	groundQuad.LoadTexture("../Textures/blueStone.jpg", "JPG");

	bthObject = Object3D(vec3(70, 30, 0), 0.5f, vec3(0.0));
	bthObject.CreateObjFromFile("../Objects/bth.obj");
	bthObject.LoadTexture("../Textures/bthcolor.dds", "DDS");

	mHouse = Object3D(vec3(-30, 0, -45), 0.08f, vec3(0.0, 25.0, 0.0));
	mHouse.CreateObjFromFile("../Objects/house_obj.obj");
	mHouse.LoadTexture("../Textures/house_diffuse.tga", "TGA");
	
	for(int i = 0; i < 5; i++)
	{
		mTreeList.push_back(Object3D(vec3(20-i*20, 0, 50+i*i), (rand() % 140 + 100)*0.01f, vec3(0.0)));
	}
	mTreeList[0].CreateObjFromFile("../Objects/gran.obj");
	mTreeList[0].LoadTexture("../Textures/gran.png", "PNG");

	mShadowMap = ShadowMap(pointLight.GetWorldPos(), pointLight.GetWorldPos()+vec3(0.0f, -1.0f, -0.01f), shadowMapRes, shadowMapRes);
	mShadowMap.CreateShadowMapTexture();

	setStaticUniforms();
	
	//glutFullScreen();
	int oldTimeSinceStart = 0;
	int timeCounter = 0;
	while(true) {

		int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
		int deltaTime = timeSinceStart - oldTimeSinceStart;
		oldTimeSinceStart = timeSinceStart;
		timeCounter += deltaTime;
		

		if(timeCounter > 16)
		{
			timeCounter = 0;
			glutMainLoopEvent();
			glutPassiveMotionFunc(My_mouse_routine); //gets the mousePointers x,y coordinates in glut window

			cam.UpdateMatrices();
			if(GetAsyncKeyState(VK_SPACE) == 0)
				cam.Control(0.8f, 0.08f, true, windowWidth*0.5f, windowHeight*0.5f);

			pointLight.UpdateLights();
			mShadowMap.SetLightPos(pointLight.GetWorldPos());
			bthObject.Update();

			if(GetAsyncKeyState(VK_ESCAPE) != 0)
			{
				return 0;
			}
		}
			
	}


	return 0;
}

