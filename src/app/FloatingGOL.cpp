#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <thread>
#include <vector>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/glm.hpp" // Lots of glm types and functions (e.g. vectors, matrices, trig functions, etc)
#include "glm/gtx/matrix_operation.hpp" // For creating diagonal matrices from vectors
#include "glm/gtx/transform.hpp" // matrix transformations, e.g. persepctive, project, rotate, lookAt
#include "glm/gtc/type_ptr.hpp" // For value_ptr, make_vec, make_mat, and make_quat
#include "glm/gtx/string_cast.hpp" // **Need to check if this is even being used

#include "SETTINGS.hpp" // Project settings
#include "Utility.hpp" // printNodeInfo(aiNode...), printTab(cnt), aiMatToGLM4(aiMat, mat)
#include "MeshData.hpp" // Vertex and mesh structs
#include "MeshGLData.hpp" // Create, draw, and clear meshes
#include "GLSetup.hpp" // Setup GL
#include "Shader.hpp" // Compile and init shaders
#include "Buffer.hpp" // GBuffer / FBO
#include "Scene.hpp" // For importing scenes

using namespace std;


	///// END INCLUDES : START DEFINES /////

#define PI	3.14159265f

#define GL_CLEAR_COLOR	0.0f, 0.0f, 0.0f, 1.0f
#define VERTEX_COLOR	1.0f, 1.0f, 1.0f, 1.0f

#define GLOBAL_X	glm::vec3(1,0,0)
#define GLOBAL_Y	glm::vec3(0,1,0)
#define GLOBAL_Z	glm::vec3(0,0,1)

#define FOV			90.0f
#define NEAR_PLANE	0.001f
#define FAR_PLANE	100.0f

#define SHADER_DIR	std::string("./src/shaders/FloatingGOL/")

#define QUAD_SCALE	1.0f

	///// END DEFINES : START STRUCTS AND STUFF /////

namespace displayProg {
	MeshGL renderArea;
	float aspectRatio;
	GLuint ID = 0;
	vector<GLuint> gbLocs;

	
	void createScreenQuad(glm::vec2 c1 = glm::vec2(QUAD_SCALE), glm::vec2 c2 = glm::vec2(-QUAD_SCALE)) {
		c1 = maxAxes(c1, c2);
		c2 = minAxes(c1, c2);

		float width = c1.x - c2.x;
		float height = c1.y - c2.y;

		aspectRatio = 1.0f;
		if (width != 0 && height != 0) {
			aspectRatio = (float)width / (float)height;
		}
		
		Mesh quad;

		Vertex v0;
		v0.position = glm::vec3(c2.x, c2.y, 0.0f);
		v0.color = glm::vec4(0,1,0,1);
		v0.normal = glm::normalize(glm::vec3(-1,-1,1));
		v0.texcoord = glm::vec2(0,0);
		quad.vertices.push_back(v0);

		Vertex v1;
		v1.position = glm::vec3(c1.x, c2.y, 0.0f);
		v1.color = glm::vec4(0.5,0.5,0,1);
		v1.normal = glm::normalize(glm::vec3(1,-1,1));
		v1.texcoord = glm::vec2(1,0);
		quad.vertices.push_back(v1);

		Vertex v2;
		v2.position = glm::vec3(c2.x, c1.y, 0.0f);
		v2.color = glm::vec4(0,1,1,1);
		v2.normal = glm::normalize(glm::vec3(-1,1,1));
		v2.texcoord = glm::vec2(0,1);
		quad.vertices.push_back(v2);

		Vertex v3;
		v3.position = glm::vec3(c1.x, c1.y, 0.0f);
		v3.color = glm::vec4(0,0,1,1);
		v3.normal = glm::normalize(glm::vec3(1,1,1));
		v3.texcoord = glm::vec2(1,1);
		quad.vertices.push_back(v3);

		quad.indices = { 0, 1, 2, 1, 3, 2 };

		createMeshGL(quad, renderArea);
	}

	void getLocations() {
		// Does nothing for the time being
	}

	void use() {
		// Does nothing for the time being
	}

	void draw() {
		drawMesh(renderArea);
	}
}

namespace stepProg {
	MeshGL renderArea;
	float aspectRatio;
	GLuint ID = 0;
	vector<GLuint> gbLocs;

	GLuint xIncLoc = 0;
	GLuint yIncLoc = 0;

	
	void createScreenQuad(glm::vec2 c1 = glm::vec2(QUAD_SCALE), glm::vec2 c2 = glm::vec2(-QUAD_SCALE)) {
		c1 = maxAxes(c1, c2);
		c2 = minAxes(c1, c2);

		float width = c1.x - c2.x;
		float height = c1.y - c2.y;

		aspectRatio = 1.0f;
		if (width != 0 && height != 0) {
			aspectRatio = (float)width / (float)height;
		}
		
		Mesh quad;

		Vertex v0;
		v0.position = glm::vec3(c2.x, c2.y, 0.0f);
		v0.color = glm::vec4(0,1,0,1);
		v0.normal = glm::normalize(glm::vec3(-1,-1,1));
		v0.texcoord = glm::vec2(0,0);
		quad.vertices.push_back(v0);

		Vertex v1;
		v1.position = glm::vec3(c1.x, c2.y, 0.0f);
		v1.color = glm::vec4(0.5,0.5,0,1);
		v1.normal = glm::normalize(glm::vec3(1,-1,1));
		v1.texcoord = glm::vec2(1,0);
		quad.vertices.push_back(v1);

		Vertex v2;
		v2.position = glm::vec3(c2.x, c1.y, 0.0f);
		v2.color = glm::vec4(0,1,1,1);
		v2.normal = glm::normalize(glm::vec3(-1,1,1));
		v2.texcoord = glm::vec2(0,1);
		quad.vertices.push_back(v2);

		Vertex v3;
		v3.position = glm::vec3(c1.x, c1.y, 0.0f);
		v3.color = glm::vec4(0,0,1,1);
		v3.normal = glm::normalize(glm::vec3(1,1,1));
		v3.texcoord = glm::vec2(1,1);
		quad.vertices.push_back(v3);

		quad.indices = { 0, 1, 2, 1, 3, 2 };

		createMeshGL(quad, renderArea);
	}

	void getLocations() {
		xIncLoc	= glGetUniformLocation(ID, "inc_x");
		yIncLoc	= glGetUniformLocation(ID, "inc_y");
		if (DEBUG_MODE) {
			cout << "inc_x: " << xIncLoc << endl;
			cout << "inc_y: " << yIncLoc << endl;
		}
	}

	void use(int width, int height) {
		glUseProgram(ID);
		glUniform1f(xIncLoc, 1.0f / float(width));
		glUniform1f(yIncLoc, 1.0f / float(height));
	}

	void draw() {
		drawMesh(renderArea);
	}
}

namespace geoMeshProg {
	vector<Scene> allScenes;

	GLuint ID = 0;

	GLint modelMatLoc = 0;
	GLint viewProjMatLoc = 0;
	GLint normalMatLoc = 0;

	void getLocations() {
		modelMatLoc		= glGetUniformLocation(ID, "modelMat");
		viewProjMatLoc	= glGetUniformLocation(ID, "viewProjMat");
		normalMatLoc	= glGetUniformLocation(ID, "normalMat");
		if (DEBUG_MODE) {
			cout << "\tGEOMETRY MESH PROGRAM: " << endl;
			cout << "modelMatLoc: "		<< modelMatLoc << endl;
			cout << "viewProjMatLoc: "	<< viewProjMatLoc << endl;
			cout << "normalMatLoc: "	<< normalMatLoc << endl;
		}
	}
	
	void use(glm::mat4 &viewProjMat) {
		glUseProgram(geoMeshProg::ID);
		glUniformMatrix4fv(geoMeshProg::viewProjMatLoc, 1, false, glm::value_ptr(viewProjMat));
	}
	
	// Geometry pass for all meshes (must use geometry program before calling)
	void renderScene(vector<MeshGL> &allMeshes, aiNode *node, glm::mat4 &parentMat, int level) {
		// Create and pass in new model matrix
		glm::mat4 nodeMat;
		aiMatToGLM4(node->mTransformation, nodeMat);
		glm::mat4 modelMat = parentMat * nodeMat;
		glUniformMatrix4fv(geoMeshProg::modelMatLoc, 1, false, glm::value_ptr(modelMat));

		// Create and pass in normal matrix
		glm::mat3 normalMat = glm::transpose(glm::inverse(glm::mat3(modelMat)));
		glUniformMatrix3fv(geoMeshProg::normalMatLoc, 1, false, glm::value_ptr(normalMat));

		// For each mesh in the node
		for (unsigned int i = 0; i < node->mNumMeshes; i++) {
			int index = node->mMeshes[i];
			drawMesh(allMeshes.at(index));
		}

		// For all child nodes
		for (unsigned int i = 0; i < node->mNumChildren; i++) {
			// Recurse through all child nodes
			renderScene(allMeshes, node->mChildren[i], modelMat, level+1);
		}
	}
	// Renders a specific scene struct
	void renderScene(Scene &scene) {
		renderScene(scene.allMeshes, scene.ais->mRootNode, glm::mat4(1.0), 0);
	}

	// Renders all scenes
	void renderAll() {
		for (Scene &scene : allScenes) {
			renderScene(scene);
		}
	}
	
	void importScene(char *file) {
		allScenes.push_back(importSceneFromFile(file));
	}

	// Cleanup all meshes
	void cleanup() {
		for (Scene &scene : allScenes) {
			scene.cleanup();
		}
	}
}

	///// END STRUCTS : START GLOBALS /////

glm::vec3 camera_eye = glm::vec3(0,0,1); // Location of the camera
glm::vec3 camera_lookat = camera_eye + glm::normalize(-camera_eye); // Point that the camera is looking at (default = origin)

glm::vec2 last_mouse_pos = glm::vec2(0,0);

// Corners of the viewport relative to the size of the window (0.0 ~ 1.0)
glm::vec2 bottomleft = glm::vec2(1.0f);
glm::vec3 vpsize = glm::vec3(0.0f, 0.0f, 1.0f); // Width, height, aspect ratio

bool is_stepping = false;

int FRAMETIME = int(1000.0 / FPS);


	///// END GLOBALS : START FUNCTIONS /////

// Set the corers of the viewport
void setViewport(glm::vec2 c1, glm::vec2 c2) {
	boundBetween(c1, 0.0f, 1.0f);
	boundBetween(c2, 0.0f, 1.0f);

	bottomleft = minAxes(c1, c2);
	glm::vec2 topright = maxAxes(c1, c2);

	vpsize.x = topright.x - bottomleft.x;
	vpsize.y = topright.y - bottomleft.y;

	// Get aspect ratio
	if (vpsize.y != 0.0f && vpsize.z != 0.0f) {
		vpsize.z = vpsize.x / vpsize.y;
	}
	else { vpsize.z = 1.0f; }
}


	///// END FUNCTIONS : START CALLBACKS /////

bool shift_pressed = false;
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Update shift_pressed
	if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
		shift_pressed = (action == GLFW_PRESS);
	}
	
	// If key was pressed or "repeat-pressed"
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		// Check keypresses
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_SPACE:
			is_stepping = !is_stepping;
			break;
		case GLFW_KEY_UP:
			FRAMETIME = std::max(1, std::min(1000, FRAMETIME + int(float(1 + FRAMETIME / 20.0f))));
			cout << "Frametime = " << FRAMETIME << endl;
			break;
		case GLFW_KEY_DOWN:
			FRAMETIME = std::max(1, std::min(1000, FRAMETIME - int(float(1 + FRAMETIME / 20.0f))));
			cout << "Frametime = " << FRAMETIME << endl;
			break;
		}
	}
}

static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
	// Unused for now
}


	///// END CALLBACKS : START MAIN /////

int main(int argc, char **argv) {
	// Get object file to be loaded
	char* file;
	if (argc >= 2) {
		file = argv[1];
	}
	else {
		cout << "No argument, using default model..." << endl;
		file = "./assets/models/sphere.obj";
	}

	// Create window & create OpenGL context
	GLFWwindow* window = setupGLFW("Press Space to Pause/Play", WINDOW_WIDTH, WINDOW_HEIGHT, DEBUG_MODE);
	setupGLEW(window);

	// Get current mouse pos
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	last_mouse_pos = glm::vec2(mx, my);

	// // Hide cursor
	// glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_position_callback);

	checkOpenGLVersion();

	// Setup debugging if requested
	if (DEBUG_MODE) checkAndSetupOpenGLDebugging();

	glClearColor(GL_CLEAR_COLOR);

	// Create a load shaders
	try {
		geoMeshProg::ID = loadAndCreateShaderProgram(SHADER_DIR + "GeoMesh.vs",	SHADER_DIR + "GeoMesh.fs");
		displayProg::ID = loadAndCreateShaderProgram(SHADER_DIR + "Display.vs",	SHADER_DIR + "Display.fs");
		stepProg::ID	= loadAndCreateShaderProgram(SHADER_DIR + "Step.vs",	SHADER_DIR + "Step.fs");
	}
	catch (exception e) {
		// Close program
		cleanupGLFW(window);
		exit(EXIT_FAILURE);
	}


	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);





	geoMeshProg::getLocations();

	// Set viewport size (relative to window size?)
	setViewport(glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
	print(bottomleft);
	print(vpsize);

	// Get framebuffer size
	int frameWidth, frameHeight;
	glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

	// Create FBO
	FBO fbo;
	fbo.pushFloatAttachment("life");
	fbo.init(GAME_WIDTH, GAME_HEIGHT);

	// If using integer framebuffers
	glDisable(GL_DITHER);

	// Initialize GDBuffer
	GDBuffer gbuff = GDBuffer(fbo);

	// Get buffer uniform locations per programs
	gbuff.getLocs(stepProg::ID, stepProg::gbLocs);
	gbuff.getLocs(displayProg::ID, displayProg::gbLocs);

	// Get other uniforms for step program
	glUseProgram(stepProg::ID);
	stepProg::getLocations();


		/// CREATE MESHES ///
	geoMeshProg::importScene(file); // Can call this as many times as you like with whatever models instead of "file"
	displayProg::createScreenQuad();
	stepProg::createScreenQuad();


		/// INITIAL GEOMETRY PASS | SETS INITAL CONDITIONS ///
	if (!glfwWindowShouldClose(window)) {
		// Set viewport size
		int fwidth, fheight;
		glfwGetFramebufferSize(window, &fwidth, &fheight);
		glViewport(
			0, 0, GAME_WIDTH, GAME_HEIGHT
		);

		// Get aspect ratio of window
		float aspect_ratio = 1.0f;
		if (fwidth != 0 && fheight != 0) {
			aspect_ratio = (float)fwidth / (float)fheight;
			aspect_ratio *= vpsize.z; // vpsize.z = aspect ratio of viewport
		}

		// Bind and clear gbuffer
		gbuff.bind(); // Write to gbuff
		glClearColor(GL_CLEAR_COLOR);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			/// DRAW THE INITAL CONDITIONS
		// Create view and projection matrices
		glm::mat4 viewMat = glm::lookAt(camera_eye, camera_lookat, GLOBAL_Y);
		glm::mat4 projMat = glm::perspective(glm::radians(FOV), aspect_ratio, NEAR_PLANE, FAR_PLANE);
		glm::mat4 viewProjMat = projMat * viewMat;

		// Setup mesh program
		geoMeshProg::use(viewProjMat);

		// Render meshes
		geoMeshProg::renderAll();
			/// END DRAWING ///


		gbuff.unbind(); // Stop writing to gbuff
		gbuff.swap(); // Swap internal buffers
	}

	while(!glfwWindowShouldClose(window)) {
		

			/// STEP ///
		if (is_stepping) {
			// Setup program stuff
			glUseProgram(stepProg::ID);
			glViewport(0, 0, gbuff.width, gbuff.height);

			// Use/bind gbuff
			gbuff.use(stepProg::gbLocs); // Read from gbuff
			gbuff.bind(); // Write to gbuff

			// Clear framebuffer
			glClearColor(GL_CLEAR_COLOR);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Perform step
			stepProg::use(gbuff.width, gbuff.height);
			stepProg::draw();
			glUseProgram(0);

			// Unbind/unuse and swap
			gbuff.unuse(stepProg::gbLocs); // Stop reading from gbuff
			gbuff.unbind(); // Stop writing to gbuff
			gbuff.swap(); // Swap internal buffers
		}


			/// DISPLAY ///
		
		// Set viewport size
		int fwidth, fheight;
		glfwGetFramebufferSize(window, &fwidth, &fheight);
		glViewport(
			(int)((float)fwidth*bottomleft.x), 
			(int)((float)fheight*bottomleft.y),
			(int)((float)fwidth*vpsize.x), 
			(int)((float)fheight*vpsize.y)
		);

		// Get aspect ratio of window
		float aspect_ratio = 1.0f;
		if (fwidth != 0 && fheight != 0) {
			aspect_ratio = (float)fwidth / (float)fheight;
			aspect_ratio *= vpsize.z; // vpsize.z = aspect ratio of viewport
		}

		
		glUseProgram(displayProg::ID); // Use display program
		glViewport(0, 0, fwidth, fheight); // Set viewport

		gbuff.use(displayProg::gbLocs); // Read from gbuff


		// Clear framebuffer
		glClearColor(GL_CLEAR_COLOR);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Display game status
		displayProg::use();
		displayProg::draw();

		gbuff.unuse(displayProg::gbLocs); // Stop reading gbuff
		glUseProgram(0);

		// Swap framebuffers and poll for window events
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Sleep for a bit
		this_thread::sleep_for(chrono::milliseconds(FRAMETIME));
	}

	geoMeshProg::cleanup();

	// Cleanup shader program
	glUseProgram(0);
	glDeleteProgram(geoMeshProg::ID);
	glDeleteProgram(displayProg::ID);

	// Destroy window and stop GLFW
	cleanupGLFW(window);

	return 0;
}