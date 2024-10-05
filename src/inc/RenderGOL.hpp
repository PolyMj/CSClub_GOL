#pragma once
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

#define CLEAR_COLOR	0.0f, 0.0f, 0.0f, 1.0f

#define VERT_SHADER "./src/shaders/General/RasterizeTexture.vs"
#define GEO_SHAD_VS	"./src/shaders/General/GeoMesh1.vs"
#define GEO_SHAD_FS	"./src/shaders/General/GeoMesh1.fs"

#define MIN_FPS		10
#define MIN_SPF		1

float ASPECT_RATIO;
MeshGL SCREEN_QUAD;
GDBuffer gbuff;

GLFWwindow *window;

size_t STEPTIME = 1'000'000 / (STEP_PER_FRAME*TARGET_FPS);
bool is_stepping = false;

bool shift_pressed = false;

int steps_per_frame = STEP_PER_FRAME;
int frames_per_second = TARGET_FPS;
size_t disp_step = 0;

Mesh brush_mesh;
size_t brush_size = 5;
glm::vec4 brush_color = glm::vec4(1.0f);

inline void recompute_frametime() {
	STEPTIME = 1'000'000 / (steps_per_frame*frames_per_second);
	cout << "FPS = " << frames_per_second 
		<< " | Steps per Frame = " << steps_per_frame 
		<< " | Steptime (ns) = " << STEPTIME << endl;
}

inline void spf(int inc) {
	steps_per_frame = std::max(steps_per_frame+inc, MIN_SPF);
	recompute_frametime();
}

inline void fps(int inc) {
	frames_per_second = std::max(frames_per_second+inc, MIN_FPS);
	recompute_frametime();
}


Mesh createQuad(glm::vec2 c1 = glm::vec2(1.0f), glm::vec2 c2 = glm::vec2(-1.0f)) {
	c1 = maxAxes(c1, c2);
	c2 = minAxes(c1, c2);

	float width = c1.x - c2.x;
	float height = c1.y - c2.y;

	ASPECT_RATIO = 1.0f;
	if (width != 0 && height != 0) {
		ASPECT_RATIO = (float)width / (float)height;
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

	return quad;
}

namespace displayProg {
	GLuint ID;
	vector<GLuint> gbLocs;

	inline void getLocations() {

	}

	inline void use() {
		glUseProgram(ID);
	}

	inline void draw() {
		drawMesh(SCREEN_QUAD);
	}
}

namespace stepProg {
	GLuint ID;
	vector<GLuint> gbLocs;

	GLuint xIncLoc = 0;
	GLuint yIncLoc = 0;

	inline void getLocations() {
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

	inline void draw() {
		drawMesh(SCREEN_QUAD);
	}
}

namespace geoMeshProg {
	GLuint ID = 0;

	GLint modelMatLoc = 0;
	GLint otherMatLoc = 0;

	void getLocations() {
		modelMatLoc		= glGetUniformLocation(ID, "modelMat");
		otherMatLoc		= glGetUniformLocation(ID, "viewProjMat");
		if (DEBUG_MODE) {
			cout << "\tGEOMETRY MESH PROGRAM: " << endl;
			cout << "modelMatLoc: "		<< modelMatLoc << endl;
			cout << "otherMatLoc: "		<< otherMatLoc << endl;
		}
	}
	
	void use(const glm::mat4 &otherMat) {
		glUseProgram(geoMeshProg::ID);
		glUniformMatrix4fv(geoMeshProg::otherMatLoc, 1, false, glm::value_ptr(otherMat));
	}
	
	// Geometry pass for all meshes (must use geometry program before calling)
	void renderScene(vector<MeshGL> &allMeshes, aiNode *node, glm::mat4 &parentMat, int level) {
		// Create and pass in new model matrix
		glm::mat4 nodeMat;
		aiMatToGLM4(node->mTransformation, nodeMat);
		glm::mat4 modelMat = parentMat * nodeMat;
		glUniformMatrix4fv(geoMeshProg::modelMatLoc, 1, false, glm::value_ptr(modelMat));

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
	inline void renderScene(Scene &scene) {
		renderScene(scene.allMeshes, scene.ais->mRootNode, glm::mat4(1.0), 0);
	}

	// Render a MeshGL
	inline void renderMesh(MeshGL &mgl) {
		glUniformMatrix4fv(geoMeshProg::modelMatLoc, 1, false, glm::value_ptr(glm::mat4(1.0f)));
		drawMesh(mgl);
	}
}


void __brush();

glm::vec2 mouse_pos = glm::vec2(0.0f);
static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
	int fwidth, fheight;
	glfwGetFramebufferSize(window, &fwidth, &fheight);
	
	mouse_pos = glm::vec2(float(xpos) / float(fwidth), float(ypos) / float(fheight));
	mouse_pos = (mouse_pos * 2.0f) - 1.0f;
	
	if (shift_pressed) {
		__brush();
	}
}

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
			disp_step = 0;
			break;
		case GLFW_KEY_UP:
			spf(1);
			break;
		case GLFW_KEY_DOWN:
			spf(-1);
			break;
		case GLFW_KEY_RIGHT:
			fps(1);
			break;
		case GLFW_KEY_LEFT:
			fps(-1);
			break;
		case GLFW_KEY_LEFT_SHIFT:
		case GLFW_KEY_RIGHT_SHIFT:
			__brush();
			break;
		}
	}
}

int initRenderer(FBO &fbo, float &aspect_ratio,
	char *stepFS_filepath, char *displayFS_filepath,
	char *geoVS_filepath = GEO_SHAD_VS, char *geoFS_filepath = GEO_SHAD_FS
) {
	window = setupGLFW("Press Space to Pause/Play", WINDOW_WIDTH, WINDOW_HEIGHT, DEBUG_MODE);
	setupGLEW(window);

	// Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_position_callback);

	checkOpenGLVersion();

	if (DEBUG_MODE) checkAndSetupOpenGLDebugging();

	glClearColor(CLEAR_COLOR);

	// Create a load shaders
	try {
		// stepProg::ID	= loadAndCreateShaderProgram(VERT_SHADER, stepFS_filepath);
		// displayProg::ID	= loadAndCreateShaderProgram(VERT_SHADER, displayFS_filepath);
		// geoMeshProg::ID	= loadAndCreateShaderProgram(GEO_SHAD_VS, GEO_SHAD_FS);
		#define SHADER_DIR std::string("./src/shaders/BasicGOL/")
		geoMeshProg::ID = loadAndCreateShaderProgram(SHADER_DIR + "GeoMesh.vs",	SHADER_DIR + "GeoMesh.fs");
		displayProg::ID = loadAndCreateShaderProgram(SHADER_DIR + "Display.vs",	SHADER_DIR + "Display.fs");
		stepProg::ID	= loadAndCreateShaderProgram(SHADER_DIR + "Step.vs",	SHADER_DIR + "Step.fs");
	}
	catch (exception e) {
		// Close program
		cleanupGLFW(window);
		return(-1);
	}

		/// RENDERING OPTIONS ///

	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	glDisable(GL_DITHER);

		/// END OF RENDERING OPTIONS ///

	glUseProgram(geoMeshProg::ID);
	geoMeshProg::getLocations();
	glUseProgram(displayProg::ID);
	displayProg::getLocations();
	glUseProgram(stepProg::ID);
	stepProg::getLocations();
	glUseProgram(0);


	// Create screen quad
	Mesh m = createQuad();
	createMeshGL(m, SCREEN_QUAD);

	// Reuse for brush mesh
	brush_mesh = m;

	// Initialize frambuffer and create double buffer
	if (fbo.is_init) {
		cerr << "ERROR: RenderGOL.hpp: initRenderer(): FBO is already initialized. Don't initialize before calling initRenderer(). Please. I love you <3" << endl;
		return -1;
	}

	fbo.init(GAME_WIDTH, GAME_HEIGHT);
	gbuff = GDBuffer(fbo);

	gbuff.getLocs(stepProg::ID, stepProg::gbLocs);
	gbuff.getLocs(displayProg::ID, displayProg::gbLocs);

	
	// Get aspect ratio of window
	int fwidth, fheight;
	aspect_ratio = 1.0f;
	glfwGetFramebufferSize(window, &fwidth, &fheight);
	if (fwidth != 0 && fheight != 0) {
		aspect_ratio = float(fwidth) / float(fheight);
	}

	disp_step = 0;

	return(0);
}

void drawGeometry(Scene &scene, bool clear = true, const glm::mat4 &transform = glm::mat4(1.0f)) {
	gbuff.swap();
	glViewport(0,0,GAME_WIDTH,GAME_HEIGHT);

	gbuff.bind();

	if (clear) {
		glClearColor(CLEAR_COLOR);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	geoMeshProg::use(transform);
	geoMeshProg::renderScene(scene);

	gbuff.unbind();
	gbuff.swap();
}

void drawGeometry(MeshGL &mgl, bool clear = true, const glm::mat4 &transform = glm::mat4(1.0f)) {
	gbuff.swap();
	glViewport(0,0,GAME_WIDTH,GAME_HEIGHT);

	gbuff.bind();

	if (clear) {
		glClearColor(CLEAR_COLOR);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	geoMeshProg::use(transform);
	geoMeshProg::renderMesh(mgl);

	gbuff.unbind();
	gbuff.swap();
}


void drawGeometry(Mesh &mesh, bool clear = true, const glm::mat4 &transform = glm::mat4(1.0f)) {
	MeshGL mgl;
	createMeshGL(mesh, mgl);

	drawGeometry(mgl, false, transform);
	cleanupMesh(mgl);
}


void drawingLoop() {
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
			glClearColor(CLEAR_COLOR);
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

		if (++disp_step >= steps_per_frame) {
			disp_step = 0;

			// Set viewport size
			int fwidth, fheight;
			glfwGetFramebufferSize(window, &fwidth, &fheight);

			
			glUseProgram(displayProg::ID); // Use display program
			glViewport(0, 0, fwidth, fheight); // Set viewport

			gbuff.use(displayProg::gbLocs); // Read from gbuff


			// Clear framebuffer
			glClearColor(CLEAR_COLOR);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Display game status
			displayProg::use();
			displayProg::draw();

			gbuff.unuse(displayProg::gbLocs); // Stop reading gbuff
			glUseProgram(0);

			// Swap framebuffers and poll for window events
			glfwSwapBuffers(window);
		}

		glfwPollEvents();
		
		// Sleep for a bit
		if (STEPTIME < 1'000) {
			this_thread::sleep_for(chrono::nanoseconds(STEPTIME));
		}
		else {
			// this_thread::sleep_for(chrono::nanoseconds(STEPTIME % 1'000));
			this_thread::sleep_for(chrono::milliseconds(STEPTIME / 1'000));
		}
	}
}


void __brush() {
	float xoff = float(brush_size) / float(gbuff.width);
	float yoff = float(brush_size) / float(gbuff.height);

	// Rescale then translate a fullscreen quad to the desired brush size
	glm::mat4 transform = buildTranslate(glm::vec4(mouse_pos.x, -mouse_pos.y, 0.0f, 1.0f))
						* buildScale(glm::vec4(xoff,yoff,0.0f,1.0f));
	print(transform);
	drawGeometry(brush_mesh, false, transform);
}


void cleanupRenderer() {
	glDeleteProgram(geoMeshProg::ID);
	glDeleteProgram(displayProg::ID);
	glDeleteProgram(stepProg::ID);

	cleanupMesh(SCREEN_QUAD);
	cleanupGLFW(window);
}