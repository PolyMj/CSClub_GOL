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

#include <assimp/Importer.hpp> // For importing 3D models
#include <assimp/scene.h> // For aiScene type
#include <assimp/postprocess.h> // For postprocessing flags

using namespace std;


	///// END INCLUDES : START DEFINES /////

#define PI	3.14159265f

#define GL_CLEAR_COLOR	0.0f, 0.2f, 0.25f, 1.0f
#define VERTEX_COLOR	1.0f, 1.0f, 1.0f, 1.0f

#define GLOBAL_X	glm::vec3(1,0,0)
#define GLOBAL_Y	glm::vec3(0,1,0)
#define GLOBAL_Z	glm::vec3(0,0,1)

#define FOV			90.0f
#define NEAR_PLANE	0.001f
#define FAR_PLANE	100.0f

#define MOVE_SPEED		0.1f
#define MOVE_SPEED_SLOW 0.006f
#define MOUSE_SENS		6.0f
#define MIN_ANGLE_FROM_Y (1.0f / 128.0f)

#define LIGHT_CNT	4

#define SHADER_DIR	std::string("./src/shaders/BasicGOL/")

#define QUAD_SCALE	1.0f

	///// END DEFINES : START STRUCTS AND STUFF /////

struct PointLight {
	glm::vec4 pos = glm::vec4(0,0,0,1);
	glm::vec4 color = glm::vec4(1,1,1,1);
	GLint posLoc = -1;
	GLint colorLoc = -1;
};

struct FBO {
    unsigned int ID;
    int width;
    int height;
    vector<unsigned int> colorIDs;
    unsigned int depthRBO;

    void clear() {
        ID = 0;
        width = 0;
        height = 0;
        colorIDs.clear();
        depthRBO = 0;
    };
};

struct GBuffer {
	FBO fbo;
	vector<int> locs;

	void startGeometry() {
		glBindFramebuffer(GL_FRAMEBUFFER, fbo.ID);
	};

	void endGeomtry() {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	};

	void startLighting() {
		for (int i = 0; i < locs.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, fbo.colorIDs.at(i));
			glUniform1i(locs.at(i), i);
		}
	};

	void endLighting() {
		for (int i = 0; i < locs.size(); i++) {
			glActiveTexture(GL_TEXTURE0 + i);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	};
};

namespace lightProg {
	PointLight lights[LIGHT_CNT];
	MeshGL renderArea;
	float aspectRatio;

	GLuint ID = 0;

	// Light locations are in lights[] -- This SHOULD be changed later
	GLuint cameraPosLoc = 0;

	void getLocations() {
		cout << "\tLIGHTING PROGRAM: " << endl;
		cout << "LOT NOT'S HERE CUZ YOU'RE AN UNORGANIZED DUMBASS, FIX IT MJ" << endl;

		cameraPosLoc = glGetUniformLocation(ID, "cameraPos");

		if (DEBUG_MODE) {
			cout << "\tLIGHTING PROGRAM: "	<< endl;
			cout << "cameraPosLoc: "		<< cameraPosLoc << endl;
		}
	}

	
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

	// Creates a light, creates uniform locations for them, and places it in lights[index]
	void createLight(glm::vec3 pos, glm::vec4 color, unsigned int index) {
		if (index >= LIGHT_CNT) {
			cerr << "ERROR: Attempt to create light with index (" << index << ") that exceeds LIGHT_CNT (" << LIGHT_CNT << ")" << endl;
			return;
		}

		string colorLoc_str = "lights[" + to_string(index) + "].color";
		string posLoc_str = "lights[" + to_string(index) + "].pos";

		PointLight light;
		light.pos = glm::vec4(pos, 1.0f);
		light.color = color;
		light.posLoc = glGetUniformLocation(lightProg::ID, posLoc_str.c_str());
		light.colorLoc = glGetUniformLocation(lightProg::ID, colorLoc_str.c_str());

		if (DEBUG_MODE) {
			cout << posLoc_str << "Loc: " << light.posLoc << endl;
			cout << colorLoc_str << "Loc: " << light.colorLoc << endl;
		}

		lights[index] = light;
	}
	
	void use(glm::vec3 &cameraPos) {
		// Pass in camera position
		glUniform3fv(cameraPosLoc, 1, glm::value_ptr(cameraPos));

		// Pass in lights
		for (unsigned int i = 0; i < LIGHT_CNT; i++) {
            glm::vec4 lightPos = lightProg::lights[i].pos;
			glUniform4fv(lightProg::lights[i].posLoc, 1, glm::value_ptr(lightPos));
			glUniform4fv(lightProg::lights[i].colorLoc, 1, glm::value_ptr(lightProg::lights[i].color));
		}
	}

	void draw() {
		drawMesh(renderArea);
	}
}

namespace geoMeshProg {
	struct Scene {
		vector<MeshGL> allMeshes;
		const aiScene* ais;
	};

	Assimp::Importer importer;
	unsigned int DEFAULT_FLAGS = (
		aiProcess_Triangulate | 
		aiProcess_FlipUVs | 
		aiProcess_GenNormals | 
		aiProcess_JoinIdenticalVertices
	);

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
	
	// Get data from aiMesh (mesh) and place it into a Mesh (m)
	void extractMeshData(aiMesh *mesh, Mesh &m) {
		// Clear vertices/indicies
		m.vertices.clear();
		m.indices.clear();

		// Loop through vertices
		for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
			// Create a Vertex
			Vertex v;
			v.color = glm::vec4(VERTEX_COLOR);

			// Get vertex position infor from mesh and store it in the new Vertex
			aiVector3D aiV = mesh->mVertices[i];
			v.position = glm::vec3(aiV[0], aiV[1], aiV[2]);

			// Get normal for the vertex
			aiVector3D aiN = mesh->mNormals[i];
			v.normal = glm::vec3(aiN[0], aiN[1], aiN[2]);
			
			// Add new vertex to the mesh
			m.vertices.push_back(v);
		}

		// Loop through all faces
		for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
			// Get aiFace
			aiFace aiF = mesh->mFaces[i];

			// For each vert index in the face, add index to m.indices
			for (unsigned int j = 0; j < aiF.mNumIndices; j++) {
				m.indices.push_back(aiF.mIndices[j]);
			}
		}
	}

	// Creates a Scene struct and adds it do allScenes
	void importScene(char *file, int flags = DEFAULT_FLAGS) {
		Scene scene = {
			vector<MeshGL>(),
			importer.ReadFile(file, flags)
		};

		// Import file

		// Check for errors when loading the aiScene
		if (scene.ais == NULL) {
			cerr << "Object Load Error: Resulting scene is NULL | ";
			cerr << importer.GetErrorString() << endl;
			exit(1);
		}
		if (scene.ais->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
			cerr << "Object Load Error: Flags incomplete" << endl;
			exit(1);
		}
		if (scene.ais->mRootNode == NULL) {
			cerr << "Object Load Error: Root node is NULL" << endl;
			exit(1);
		}

		
		// Create list of meshes
		for (unsigned int i = 0; i < scene.ais->mNumMeshes; i++) {
			// Get Mesh
			Mesh m;
			extractMeshData(scene.ais->mMeshes[i], m);
			// Convert to MeshGL
			MeshGL mgl;
			createMeshGL(m, mgl);
			// Add MeshGL
			scene.allMeshes.push_back(mgl);
		}

		allScenes.push_back(scene);
	}

	// Cleanup all meshes
	void cleanup() {
		for (Scene &scene : allScenes) {
			for (int i = 0; i < scene.allMeshes.size(); i++) {
				cleanupMesh(scene.allMeshes.at(i));
			}
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


	///// END GLOBALS : START FUNCTIONS /////

// Rotate an object about a point and axis by an angle
glm::mat4 makeLocalRotate(glm::vec3 point, glm::vec3 axis, float angle, bool isInDegrees = true) {
	if (isInDegrees) angle = glm::radians(angle);

	glm::mat4 toOrigin = glm::translate(-point);
	glm::mat4 rotMat = glm::rotate(angle, glm::normalize(axis));
	glm::mat4 fromOrigin = glm::translate(point);

	return fromOrigin * rotMat * toOrigin;
}

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

// Create a color attachment (output for fragment shaders)
// Remember that format determines how many elements (e.g. float vs. vec2 vs. vec3)
// **Is colorAttach the same as "location" in the fragment shader?
unsigned int createColorAttachment(
	int width, int height,
	int internal, int format, 
	int type,
	int texFilter, int colorAttach) {
	
	unsigned int texID = 0;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_2D, texID);
	glTexImage2D(GL_TEXTURE_2D, 0, internal, width, height, 0, format, type, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, texFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, texFilter);

	// Will probably need to be optional if actual textures are used rather than only gbuffers
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRROR_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRROR_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + colorAttach, GL_TEXTURE_2D, texID, 0);

	glBindTexture(GL_TEXTURE_2D, 0);
	return texID;
}

// Create a renderbuffer
unsigned int createDepthRBO(int width, int height) {
	// Generate an rbo
	unsigned int rbo = 0;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo); // Bind rbo

	// Establish data storage format and size for the rbo
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	// Attaches the rbo to the specified attachment point of the currently bound framebuffer
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
	
	glBindRenderbuffer(GL_RENDERBUFFER, 0); // Unbind rbo
	return rbo;
}

// Create an FBO
void createFBO(FBO &fbo, int width, int height) {
	fbo.clear();
	glGenFramebuffers(1, &(fbo.ID));
	fbo.width = width; fbo.height = height;
	glBindFramebuffer(GL_FRAMEBUFFER, fbo.ID);

	fbo.colorIDs.push_back(createColorAttachment(
		width, height,
		GL_RGB, GL_RGB, GL_UNSIGNED_BYTE,
		GL_LINEAR, 0
	));

	fbo.depthRBO = createDepthRBO(width, height);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cerr << "ERROR: Incomplete FBO!" << endl;
		fbo.clear();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void createGBuffer(GBuffer &gb, int width, int height, string *uniformNames) {
	// Create and bind framebuffer
	glGenFramebuffers(1, &(gb.fbo.ID));
	gb.fbo.width = width; gb.fbo.height = height;
	glBindFramebuffer(GL_FRAMEBUFFER, gb.fbo.ID); // Bind fbo


	// gb.fbo.colorIDs.push_back(createColorAttachment(
	// 	width, height,
	// 	GL_R8, GL_RED,
	// 	GL_UNSIGNED_BYTE,
	// 	GL_NEAREST, 0
	// ));
	// gPosition
	gb.fbo.colorIDs.push_back(createColorAttachment(
		width, height, 
		GL_RGBA16F, GL_RGBA,
		GL_FLOAT,
		GL_NEAREST, 0
	));
	// gNormal
	gb.fbo.colorIDs.push_back(createColorAttachment(
		width, height, 
		GL_RGBA16F, GL_RGBA,
		GL_FLOAT,
		GL_NEAREST, 1
	));
	// gAlbedoSpec
	gb.fbo.colorIDs.push_back(createColorAttachment(
		width, height, 
		GL_RGBA, GL_RGBA,
		GL_UNSIGNED_BYTE,
		GL_NEAREST, 2
	));

	unsigned int attachments[3] = {
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2
	};

	glDrawBuffers(3, attachments);

	for (int i = 0; i < gb.fbo.colorIDs.size(); i++) {
		gb.locs.push_back(
			glGetUniformLocation(lightProg::ID, uniformNames[i].c_str())
		);
	}

	gb.fbo.depthRBO = createDepthRBO(width, height);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		cerr << "ERROR: Incomplete GBuffer::FBO!" << endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0); // Unbind fbo
}

	///// END FUNCTIONS : START CALLBACKS /////

bool shift_pressed = false;
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// Update shift_pressed
	if (key == GLFW_KEY_LEFT_SHIFT || key == GLFW_KEY_RIGHT_SHIFT) {
		shift_pressed = (action == GLFW_PRESS);
	}
	
	bool isMoving = false;
	if (action == GLFW_PRESS || action == GLFW_REPEAT) {
		glm::vec3 move_direction = glm::vec3(0,0,0);
		switch (key) {
		case GLFW_KEY_ESCAPE:
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		// Movement WASD + Q/E for up/down
		case GLFW_KEY_W:
			move_direction += glm::normalize(camera_lookat-camera_eye);
			isMoving = true;
			break;
		case GLFW_KEY_S:
			move_direction += glm::normalize(camera_eye-camera_lookat);
			isMoving = true;
			break;
		case GLFW_KEY_D:
			move_direction += glm::normalize(glm::cross(camera_lookat-camera_eye, GLOBAL_Y));
			isMoving = true;
			break;
		case GLFW_KEY_A:
			move_direction += glm::normalize(glm::cross(camera_eye-camera_lookat, GLOBAL_Y));
			isMoving = true;
			break;
		case GLFW_KEY_Q:
			move_direction += -GLOBAL_Y;
			isMoving = true;
			break;
		case GLFW_KEY_E:
			move_direction += GLOBAL_Y;
			isMoving = true;
			break;
		}
		
		if (isMoving) {
			if (shift_pressed) {
				camera_lookat += move_direction * MOVE_SPEED_SLOW;
				camera_eye += move_direction * MOVE_SPEED_SLOW;
			}
			else {
				camera_lookat += move_direction * MOVE_SPEED;
				camera_eye += move_direction * MOVE_SPEED;
			}
		}
	}
}

static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
	// Change in mouse position
	glm::vec2 relMouse = glm::vec2(xpos-last_mouse_pos.x, ypos-last_mouse_pos.y);

	// Scale relMouse by the screen width/height
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	relMouse.x /= (float)width, relMouse.y /= (float)height;
	glm::vec2 rotation = glm::vec2(MOUSE_SENS * relMouse);

	
	// Get camera X/Z axes
	glm::vec3 camera_facing = glm::normalize(camera_lookat - camera_eye); // Normalize for good measure
	glm::vec3 camera_x_axis = glm::cross(glm::vec3(0,1,0), camera_facing);

	// Rotate about global Y
	camera_lookat = glm::vec3(
		makeLocalRotate(camera_eye, GLOBAL_Y, -rotation.x, false) *
		glm::vec4(camera_lookat, 1)
	);

	// Get the dot between the camera's z and the global Y (or negative of Y, if rotating down)
	float face_dot = glm::dot(camera_facing, negSign(rotation.y) * GLOBAL_Y);
	// Get the largest allowed dot
	float range_dot = glm::cos(abs(rotation.y) + MIN_ANGLE_FROM_Y);

	// If rotation about X is okay
	if (face_dot < range_dot) {
		// Rotate camera about X
		camera_lookat = glm::vec3(
			makeLocalRotate(camera_eye, camera_x_axis, rotation.y, false) *
			glm::vec4(camera_lookat, 1)
		);
	}
	// Otherwise, max out rotation about X
	else {
		// Get angle needed to max out the rotation about X
		float angle_to_max = sign(rotation.y) * (glm::acos(face_dot) - MIN_ANGLE_FROM_Y);
		// Rotate camera about X
		camera_lookat = glm::vec3(
			makeLocalRotate(camera_eye, camera_x_axis, angle_to_max, false) *
			glm::vec4(camera_lookat, 1)
		);
	}

	last_mouse_pos = glm::vec2(xpos, ypos);
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
	GLFWwindow* window = setupGLFW("Draw Some Meshes", WINDOW_WIDTH, WINDOW_HEIGHT, DEBUG_MODE);
	setupGLEW(window);

	// Get current mouse pos
	double mx, my;
	glfwGetCursorPos(window, &mx, &my);
	last_mouse_pos = glm::vec2(mx, my);
	// Hide cursor
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_position_callback);

	checkOpenGLVersion();

	// Setup debugging if requested
	if (DEBUG_MODE) checkAndSetupOpenGLDebugging();

	glClearColor(GL_CLEAR_COLOR);

	// Create a load shaders
	try {
		geoMeshProg::ID = loadAndCreateShaderProgram(SHADER_DIR + "GeoMesh.vs", SHADER_DIR + "GeoMesh.fs");
		lightProg::ID = loadAndCreateShaderProgram(SHADER_DIR + "Light.vs", SHADER_DIR + "LightDebug.fs");
	}
	catch (exception e) {
		// Close program
		cleanupGLFW(window);
		exit(EXIT_FAILURE);
	}

		/// CREATE TRANSFORMATION MATRICES ///
	geoMeshProg::getLocations();
	lightProg::getLocations();

		/// CREATE LIGHTS ///
	lightProg::createLight(glm::vec3(-1.0f, 1.0f, 1.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f), 0);
	lightProg::createLight(glm::vec3( 0.0f, 1.0f, 1.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f), 1);
	lightProg::createLight(glm::vec3( 1.0f, 1.0f, 1.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f), 2);
	lightProg::createLight(glm::vec3( 0.0f, 0.0f, -1.0f), glm::vec4(1.0f, 1.0f, 1.0f, 0.2f), 3);

		/// CREATE GBUFFER ///
	int frameWidth, frameHeight;
	glfwGetFramebufferSize(window, &frameWidth, &frameHeight);

	GBuffer gb;
	createGBuffer(
		gb, frameWidth, frameHeight, new string[3] { "gPosition", "gNormal", "gAlbedoSpec" }
	);

	// **Load and create textures

		/// CREATE MESHES ///

	geoMeshProg::importScene(file);

	lightProg::createScreenQuad();


	// Enable depth testing
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDepthMask(GL_TRUE);

	// Enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);

	setViewport(glm::vec2(0.0f, 1.0f), glm::vec2(1.0f, 0.0f));
	print(bottomleft);
	print(vpsize);


		/// INITIAL GEOMETRY PASS ///

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


		/// GEOMETRY PASS ////
	gb.startGeometry();
	glClearColor(0.0, 0.0, 0.0, 1.0);
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

	gb.endGeomtry();

	while(!glfwWindowShouldClose(window)) {
		glfwGetFramebufferSize(window, &fwidth, &fheight);
		glViewport(
			(int)((float)fwidth*bottomleft.x), 
			(int)((float)fheight*bottomleft.y),
			(int)((float)fwidth*vpsize.x), 
			(int)((float)fheight*vpsize.y)
		);

		// Get aspect ratio of window
		aspect_ratio = 1.0f;
		if (fwidth != 0 && fheight != 0) {
			aspect_ratio = (float)fwidth / (float)fheight;
			aspect_ratio *= vpsize.z; // vpsize.z = aspect ratio of viewport
		}

			/// LIGHTING PASS ///
		glUseProgram(lightProg::ID);
		gb.startLighting();

		glViewport(0, 0, fwidth, fheight);

		// Clear framebuffer
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		lightProg::use(camera_eye);
		lightProg::draw();

		gb.endLighting();
		glUseProgram(0);

			/// DISPLAY ///
		// Swap buffers and poll for window events
		glfwSwapBuffers(window);
		glfwPollEvents();

		// Sleep for 15ms
		this_thread::sleep_for(chrono::milliseconds(5));
	}

	geoMeshProg::cleanup();

	// Cleanup shader program
	glUseProgram(0);
	glDeleteProgram(geoMeshProg::ID);
	glDeleteProgram(lightProg::ID);

	// Destroy window and stop GLFW
	cleanupGLFW(window);

	return 0;
}