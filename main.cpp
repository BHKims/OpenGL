#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <stdexcept>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"
#include "Camera.h"
#include "stb_image.h"

const char* vertexShaderPath = "./shaders/vertexShader.glsl";
const char* fragmentShaderPath = "./shaders/fragmentShader.glsl";
const char* lightVertexPath = "./shaders/lightVertex.glsl";
const char* lightFragmentPath = "./shaders/lightFragment.glsl";

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

std::string getFileContent(const std::string& path)
{
	std::ifstream openFile(path);
	std::string content(
		(std::istreambuf_iterator<char>(openFile)),
		std::istreambuf_iterator<char>()
	);

	return content;
}

class App {
public:
	void run() {
		initWindow();
		initGLAD();
		setViewport();

		setupShaderProgram();
		setupVertexArray();
		setupTextures();
		setupUserInput();
		
		renderLoop();

		glfwTerminate();
	}

	App(const char* vertexPath, const char* fragmentPath)
		: shaderProgram(Shader(vertexPath, fragmentPath)), 
		lightShader(Shader(lightVertexPath, lightFragmentPath))
	{
		camera = Camera();
		lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	}

	void mouseInput(double xpos, double ypos) {
		camera.mouseInput(xpos, ypos);
	}
	void scrollInput(double xoffset, double yoffset) {
		camera.scrollInput(xoffset, yoffset);
	}

private:
	GLFWwindow* window;
	Shader shaderProgram;
	Shader lightShader;
	Camera camera;
	unsigned int VAO, lightVAO;
	unsigned int TexBox;
	unsigned int TexBoxSpecular;
	glm::vec3 lightPos;

	void initWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		/* core-profile means we'll get access to a smaller subset of OpenGL features
		without backwards-compatible features */
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
		if (window == NULL)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			throw std::runtime_error("Failed to create GLFW window");
		}
		/* make the context of our window the main context on the current thread. */
		glfwMakeContextCurrent(window);
	}

	void initGLAD() {
		/* GL 함수들을 호출해보기 전에 GLAD 초기화하여 모든 함수 address load */
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			throw std::runtime_error("Failed to initialize GLAD");
		}
	}

	void setViewport() {
		/* tell OpenGL the size of the rendering window */
		glViewport(0, 0, 800, 600);

		/* register a callback function on the window that gets called
		each time the window is resized */
		/* register the callback functions after we've created the window
		and before the render loop is initiated. */
		auto resize = [](GLFWwindow* window, int width, int height) {
			glViewport(0, 0, width, height);
		};
		glfwSetFramebufferSizeCallback(window, resize);
	}

	void setupShaderProgram() {
		shaderProgram.build();
		lightShader.build();
	}

	void setupVertexArray() {
		/* Establish VBO, attrib pointer, and VAO */
		//float vertices[] = {
		//	// positions          // colors           // texture coords
		//	 0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
		//	 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
		//	-0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
		//	-0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left 
		//};
		/* A Cube in Model Coordinate. 6 vertices for one rectangle. */
		// pos, normal vector, texture coord
		int TIMES_OF_STRIDE = 8;
		float vertices[] = {
			// positions          // normals           // texture coords
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
		};

		unsigned int indices[] = {  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};

		unsigned int VBO, EBO;
		/* Generate VBO, VAO, EBO, Texture ================================================ */
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);
		glGenVertexArrays(1, &VAO);

		/* Bind VBO to the VAO ============================================== */
		glBindVertexArray(VAO);

		/* Bind and Init VBO --> VAO ====================================================== */
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

		/* Bind and Init EBO --> VAO */
		/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

		/* Setup Vertex Attributes ==================================================== */
		/* 1. Position Attribute : layout (location = 0) in vec3 aPos; */
		glVertexAttribPointer(
			/* Index of vertex attribute */ 0, 
			/* Number of components per attribute */ 3, 
			/* Data type of each component */ GL_FLOAT, 
			/* Normalized */ GL_FALSE, 
			/* Stride in byte sense */ TIMES_OF_STRIDE * sizeof(float),
			/* Byte offset of first component */ (void*)0
		);
		glEnableVertexAttribArray(0);

		/* 2. Normal Vector Attribute : layout (location = 1) in vec3 aNormal; */
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 
			TIMES_OF_STRIDE * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE,
			TIMES_OF_STRIDE * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		/* ============================================================================ */

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		/* Light VAO setting */
		glGenVertexArrays(1, &lightVAO);
		glBindVertexArray(lightVAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 
			TIMES_OF_STRIDE * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glBindVertexArray(0);
	}

	void registerTexture(unsigned int* id, const char* path, int format) {
		int width, height, nrChannels;
		unsigned char* texData;

		stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis

		glGenTextures(1, id);
		glBindTexture(GL_TEXTURE_2D, *id);

		texData = stbi_load(path, &width, &height, &nrChannels, 0);
		if (texData) {
			glTexImage2D(GL_TEXTURE_2D,
				/* level = */ 0, // set each mipmap level manually, but we'll leave it at the base level
				/* internalformat = */GL_RGB, // format we want to store the texture.
				width, height,
				/* border = */0, // should always be 0 (some legacy stuff)
				/* format = */format, GL_UNSIGNED_BYTE, // format and datatype of the source image
				texData);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "Failed to load texture : " << path << std::endl;
		}
		stbi_image_free(texData);
	}

	void setupTextures() {
		registerTexture(&TexBox, "./images/container2.png", GL_RGBA);
		registerTexture(&TexBoxSpecular, "./images/container2_specular.png", GL_RGBA);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, TexBox);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TexBoxSpecular);
	}

	void setupUserInput() {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
	}

	void setLightsUniform(glm::vec3 lights[]) {
		// directional light
		shaderProgram.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
		shaderProgram.setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
		shaderProgram.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
		shaderProgram.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
		// point light 1
		shaderProgram.setVec3("pointLights[0].position", lights[0]);
		shaderProgram.setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
		shaderProgram.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
		shaderProgram.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
		shaderProgram.setFloat("pointLights[0].constant", 1.0f);
		shaderProgram.setFloat("pointLights[0].linear", 0.09f);
		shaderProgram.setFloat("pointLights[0].quadratic", 0.032f);
		// point light 2
		shaderProgram.setVec3("pointLights[1].position", lights[1]);
		shaderProgram.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
		shaderProgram.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
		shaderProgram.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
		shaderProgram.setFloat("pointLights[1].constant", 1.0f);
		shaderProgram.setFloat("pointLights[1].linear", 0.09f);
		shaderProgram.setFloat("pointLights[1].quadratic", 0.032f);
		// point light 3
		shaderProgram.setVec3("pointLights[2].position", lights[2]);
		shaderProgram.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
		shaderProgram.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
		shaderProgram.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
		shaderProgram.setFloat("pointLights[2].constant", 1.0f);
		shaderProgram.setFloat("pointLights[2].linear", 0.09f);
		shaderProgram.setFloat("pointLights[2].quadratic", 0.032f);
		// point light 4
		shaderProgram.setVec3("pointLights[3].position", lights[3]);
		shaderProgram.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
		shaderProgram.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
		shaderProgram.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
		shaderProgram.setFloat("pointLights[3].constant", 1.0f);
		shaderProgram.setFloat("pointLights[3].linear", 0.09f);
		shaderProgram.setFloat("pointLights[3].quadratic", 0.032f);
		// spotLight
		shaderProgram.setVec3("spotLight.position", camera.getPos());
		shaderProgram.setVec3("spotLight.direction", camera.Front);
		shaderProgram.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		shaderProgram.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		shaderProgram.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		shaderProgram.setFloat("spotLight.constant", 1.0f);
		shaderProgram.setFloat("spotLight.linear", 0.09f);
		shaderProgram.setFloat("spotLight.quadratic", 0.032f);
		shaderProgram.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		shaderProgram.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
	}

	void setUniforms(glm::vec3 pos[], glm::vec3 lights[], unsigned int i) {
		glm::mat4 model, view, projection;

		shaderProgram.use();
		
		model = glm::translate(glm::mat4(1.0f), pos[i]);
		model = glm::rotate(model, glm::radians(20.0f * i), glm::vec3(1.0f, 0.3f, 0.5f));
		//model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

		// note that we're translating the scene in the reverse direction of where we want to move
		/*const float radius = 10.0f;
		float camX = sin(glfwGetTime()) * radius;
		float camZ = cos(glfwGetTime()) * radius;*/
		//view = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
		glm::vec3 Pos = camera.getPos();
		view = glm::lookAt(Pos, Pos + camera.getFront(), camera.getUp());

		projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.0f);

		shaderProgram.setMat4("model", model);
		shaderProgram.setMat4("view", view);
		shaderProgram.setMat4("projection", projection);

		shaderProgram.setVec3("viewPos", camera.getPos());

		shaderProgram.setVec3("material.ambient", glm::vec3(1.0f, 0.5f, 0.31f));
		shaderProgram.setInt("material.diffuse", 0);
		shaderProgram.setInt("material.specular", 1);
		shaderProgram.setFloat("material.shininess", 32.0f);

		setLightsUniform(lights);
	}

	void setUniformsLightSource(glm::vec3 light) {
		glm::mat4 model, view, projection;

		lightShader.use();

		/* MVP for Light Source */
		model = glm::mat4(1.0f);
		model = glm::translate(model, light);
		model = glm::scale(model, glm::vec3(0.2f));

		glm::vec3 Pos = camera.getPos();
		view = glm::lookAt(Pos, Pos + camera.getFront(), camera.getUp());

		projection = glm::perspective(glm::radians(camera.getFOV()), 800.0f / 600.0f, 0.1f, 100.0f);

		lightShader.setMat4("model", model);
		lightShader.setMat4("view", view);
		lightShader.setMat4("projection", projection);
	}

	void renderLoop() {
		glm::vec3 cubePositions[] = {
			glm::vec3(0.0f,  0.0f,  0.0f),
			glm::vec3(2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3(2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3(1.3f, -2.0f, -2.5f),
			glm::vec3(1.5f,  2.0f, -2.5f),
			glm::vec3(1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};
		glm::vec3 pointLightPositions[] = {
			glm::vec3(0.7f,  0.2f,  2.0f),
			glm::vec3(2.3f, -3.3f, -4.0f),
			glm::vec3(-4.0f,  2.0f, -12.0f),
			glm::vec3(0.0f,  0.0f, -3.0f)
		};
		/* Find Location of Uniform Variable */
		// int colorLocation = glGetUniformLocation(shaderProgram, "ourColor");

		glEnable(GL_DEPTH_TEST); // Enable Depth Testing via Z-Buffer.

		/* Render Loop */
		float deltaTime = 0.0f;
		float lastFrame = glfwGetTime();
		float currentFrame;
		while (!glfwWindowShouldClose(window)) {
			currentFrame = glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
			camera.keyboardInput(window, deltaTime);

			/* the entire color buffer will be filled with the color
			as configured by glClearColor. */
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glBindVertexArray(VAO);
			/* Shader Program & VAO setting Completed, Now, let's DRAW!! */
			for (unsigned int i = 0; i < 10; ++i) {
				setUniforms(cubePositions, pointLightPositions, i);
				glDrawArrays(GL_TRIANGLES, 0, 36); // Triangle	
			}

			glBindVertexArray(lightVAO);
			for (unsigned int i = 0; i < 4; ++i) {
				setUniformsLightSource(pointLightPositions[i]);
				glDrawArrays(GL_TRIANGLES, 0, 36);
			}
			// glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}
};

App app(vertexShaderPath, fragmentShaderPath);
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	app.mouseInput(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	app.scrollInput(xoffset, yoffset);
}

int main() {
	app.run();

	return 0;
}