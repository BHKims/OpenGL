#include "Shader.h"

Shader::Shader(const char* vertexPath, const char* fragmentPath) {
	std::ifstream vertexFile;
	std::ifstream fragmentFile;

	vertexFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fragmentFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try {
		vertexFile.open(vertexPath);
		fragmentFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;

		vShaderStream << vertexFile.rdbuf();
		fShaderStream << fragmentFile.rdbuf();

		vertexFile.close();
		fragmentFile.close();

		_vertexSource = vShaderStream.str();
		_fragmentSource = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR: SHADER: File Not Successfully Read" << std::endl;
	}
}

void Shader::build() {
	int success;
	char infoLog[512];
	const char* pSource;

	/* Vertex Shader Compile ========================================================= */
	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
	pSource = _vertexSource.c_str();
	glShaderSource(vertexShader, 1, &pSource, NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success); // check for shader compile errors
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/* Fragment Shader Compile ========================================================= */
	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	pSource = _fragmentSource.c_str();
	glShaderSource(fragmentShader, 1, &pSource, NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success); // check for shader compile errors
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	/* Link Shaders ==================================================================== */
	_id = glCreateProgram();
	glAttachShader(_id, vertexShader);
	glAttachShader(_id, fragmentShader);
	glLinkProgram(_id);
	// check for linking errors
	glGetProgramiv(_id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(_id, 512, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	/* Release Shaders ================================================================== */
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

void Shader::use() {
	glUseProgram(_id);
}

/* Utility uniform functions */
void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(_id, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(_id, name.c_str()), value);
}
void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(_id, name.c_str()), value);
}
void Shader::setMat4(const std::string& name, glm::mat4 mat) const {
	glUniformMatrix4fv(glGetUniformLocation(_id, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
}
void Shader::setVec3(const std::string& name, glm::vec3 vec) const {
	glUniform3fv(glGetUniformLocation(_id, name.c_str()), 1, &vec[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(_id, name.c_str()), x, y, z);
}