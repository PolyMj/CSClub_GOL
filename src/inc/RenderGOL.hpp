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

	/// GLOBAL VARIABLES ///

extern float ASPECT_RATIO;
extern MeshGL SCREEN_QUAD;
extern GDBuffer gbuff;

extern GLFWwindow *window;

extern size_t STEPTIME;
extern bool is_stepping;

extern bool shift_pressed;

extern int steps_per_frame;
extern int frames_per_second;
extern size_t disp_step;

extern Mesh brush_mesh;
extern MeshGL brush_meshGL;
extern bool need_update_bmgl;

extern int brush_size;
extern glm::vec4 brush_color;

extern glm::vec2 last_mouse_pos;
extern glm::vec2 mouse_pos;

	/// END GLOBALS ///


inline void __recompute_frametime();

inline void __spf(int inc);

inline void __fps(int inc);


Mesh createQuad(glm::vec2 c1 = glm::vec2(1.0f), glm::vec2 c2 = glm::vec2(-1.0f));

namespace displayProg {
	extern GLuint ID;
	extern vector<GLuint> gbLocs;

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
	extern GLuint ID;
	extern vector<GLuint> gbLocs;

	extern GLuint xIncLoc;
	extern GLuint yIncLoc;

	inline void getLocations() {
		xIncLoc	= glGetUniformLocation(ID, "inc_x");
		yIncLoc	= glGetUniformLocation(ID, "inc_y");
		if (DEBUG_MODE) {
			cout << "inc_x: " << xIncLoc << endl;
			cout << "inc_y: " << yIncLoc << endl;
		}
	}

	inline void use(int width, int height) {
		glUseProgram(ID);
		glUniform1f(xIncLoc, 1.0f / float(width));
		glUniform1f(yIncLoc, 1.0f / float(height));
	}

	inline void draw() {
		drawMesh(SCREEN_QUAD);
	}
}

namespace geoMeshProg {
	extern GLuint ID;

	extern GLint modelMatLoc;
	extern GLint otherMatLoc;

	inline void getLocations() {
		modelMatLoc		= glGetUniformLocation(ID, "modelMat");
		otherMatLoc		= glGetUniformLocation(ID, "otherTransform");
		if (DEBUG_MODE) {
			cout << "\tGEOMETRY MESH PROGRAM: " << endl;
			cout << "modelMatLoc: "		<< modelMatLoc << endl;
			cout << "otherMatLoc: "		<< otherMatLoc << endl;
		}
	}
	
	inline void use(const glm::mat4 &otherMat) {
		glUseProgram(geoMeshProg::ID);
		glUniformMatrix4fv(geoMeshProg::otherMatLoc, 1, false, glm::value_ptr(otherMat));
	}
	
	// Geometry pass for all meshes (must use geometry program before calling)
	void renderScene(vector<MeshGL> &allMeshes, aiNode *node, glm::mat4 &parentMat, int level);

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

void __brush_set_color(glm::vec4 color);

void __brush_inc_color(glm::vec4 color);

inline void __brush_change_size(int diff);

inline void __clear();
static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int initRenderer(FBO &fbo, float &aspect_ratio,
	char *stepFS_filepath, char *displayFS_filepath,
	char *geoVS_filepath = GEO_SHAD_VS, char *geoFS_filepath = GEO_SHAD_FS
);

inline void clearBuffer(bool color);

void drawGeometry(Scene &scene, bool clear = true, const glm::mat4 &transform = glm::mat4(1.0f));

void drawGeometry(MeshGL &mgl, bool clear = true, const glm::mat4 &transform = glm::mat4(1.0f));

void drawGeometry(Mesh &mesh, bool clear = true, const glm::mat4 &transform = glm::mat4(1.0f));

void drawingLoop();

void cleanupRenderer();