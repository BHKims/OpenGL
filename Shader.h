#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	unsigned int _id; // Program ID;
	std::string _vertexSource;
	std::string _fragmentSource;

	Shader(const char* vertexPath, const char* fragmentPath);

	void build();
	void use(); // use/activate Shader Program

	/* Utility uniform functions */
	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMat4(const std::string& name, glm::mat4 mat) const;
	void setVec3(const std::string& name, glm::vec3 vec) const;
	void setVec3(const std::string& name, float x, float y, float z) const;
};

#endif