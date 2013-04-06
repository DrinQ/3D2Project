#ifndef SHADERHANDLER_H_
#define SHADERHANDLER_H_
#include "includes.h"
#include <fstream>

class ShaderHandler
{
private:
	uint CompileShaderFile(const char* shaderFile, const int& type); 
public:
	ShaderHandler(void);
	~ShaderHandler(void);

	uint CreateShaderProgram(const char* shaderAFile, const char* shaderBFile); 
	uint CreateShaderProgram(const char* shaderAFile, const char* shaderBFile, const char* shaderCFile);
	int UpdateUniform(const char* variable, uint shaderProgHandle, float value); 
	int UpdateUniform(const char* variable, uint shaderProgHandle, vec3 value); 
	int UpdateUniform(const char* variable, uint shaderProgHandle, vec4 value); 
	int UpdateUniform(const char* variable, uint shaderProgHandle, mat3 matrix);
	int UpdateUniform(const char* variable, uint shaderProgHandle, mat4 matrix);
	int UpdateUniform(const char* variable, uint shaderProgHandle, int value);
};

#endif