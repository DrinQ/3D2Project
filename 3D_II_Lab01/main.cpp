#include "includes.h"
#include <time.h>
//#include "TextureLoader.h"
//#include "ShaderHandler.h"
#include "Scene.h"

//#include "Camera.h"
//#include "Object3D.h"
//#include "Light.h"
//#include "ShadowMap.h"

int windowWidth = 1280;
int windowHeight = 720;
int frameCount = 0;
char windowTitle[128] = "Project 3DII";
int shadowMapRes = 4096; 

Scene* mScene;

void My_mouse_routine(int x, int y)
	{
		mScene->GetCam()->SetMousePos(x, y); //place current mouse pos in mouseX, mouseY (in Camera class)
	}

void renderCallback()
{
	frameCount++;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // clear buffer using colour

	//---1st time---- from light perspective------------------------------------
	//viewPort set in the function
	mScene->RenderShadowingObjects();
	//--------------------------------------------------------------------------

	glViewport(0, 0, windowWidth, windowHeight);
	//---2nd time----- from camera perspective----------------------------------
	
	mScene->RenderObjects();
//-----------------------switch shader program-----------------------
	mScene->RenderLightSources();

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
//
//void CreateShaderPrograms()
//{
//	shaderProgHandle = mShaderHandler.CreateShaderProgram("../Shaders/shader.vertex", "../Shaders/shader.fragment");
//	billboardShaderProgHandle = mShaderHandler.CreateShaderProgram("../Shaders/particleShader.vertex", "../Shaders/particleShader.fragment", "../Shaders/particleShader.geometry");
//}

//void CreateLights()
//{
//	mPointLight = Light(vec3(0.0f, 85.0f, 15.0f), vec3(1.0f, 1.0f, 1.0f), vec3(0.9f, 0.9f, 0.9f), 500.0f, 0.2f);
//	mPointLight.CreatemPointLight();
//	mPointLight.LoadTexture("../Textures/mPointLight01.png", "png");
//}
//
//void CreateObjects()
//{
//	groundQuad = Object3D(vec3(0), 300.0, vec3(0.0));
//	groundQuad.CreateQuad();
//	groundQuad.LoadTexture("../Textures/groundStone.jpg", "JPG");
//
//	bthObject = Object3D(vec3(70, 30, 0), 0.5f, vec3(0.0));
//	bthObject.CreateObjFromFile("../Objects/bth.obj");
//	bthObject.LoadTexture("../Textures/bthcolor.dds", "DDS");
//
//	mHouse = Object3D(vec3(-30, 0, -45), 0.08f, vec3(0.0, 25.0, 0.0));
//	mHouse.CreateObjFromFile("../Objects/house_obj.obj");
//	mHouse.LoadTexture("../Textures/house_diffuse.tga", "TGA");
//	
//	for(int i = 0; i < 5; i++)
//	{
//		mTreeList.push_back(Object3D(vec3(20-i*20, 0, 50+i*i), (rand() % 140 + 100)*0.01f, vec3(0.0)));
//	}
//	mTreeList[0].CreateObjFromFile("../Objects/gran.obj");
//	mTreeList[0].LoadTexture("../Textures/gran.png", "PNG");
//}

int main(int argc, char** argv){

	// initialise freeGLUT
	glutInit(&argc, argv); // init freeGLUT with command line param list
	Initialize();
	
	glutSetCursor(GLUT_CURSOR_NONE); //hide mouse cursor
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glutWarpPointer(windowWidth*0.5, windowHeight*0.5);

	mScene = new Scene(windowWidth, windowHeight);
	mScene->CreateShaderPrograms();
	mScene->CreateLights();
	mScene->CreateObjects();
	mScene->CreateShadowMap(shadowMapRes);
	
	mScene->SetStaticUniforms();
	
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
			glutPassiveMotionFunc(My_mouse_routine); //gets the mousePointers x,y coordinates in glut window
			glutMainLoopEvent();
			
			mScene->GetCam()->UpdateMatrices();
			if(GetAsyncKeyState(VK_SPACE) == 0)
				mScene->GetCam()->Control(0.8f, 0.08f, true, windowWidth*0.5f, windowHeight*0.5f);
			
			mScene->Update();

			if(GetAsyncKeyState(VK_ESCAPE) != 0)
			{
				return 0;
			}
		}
	}


	return 0;
}

