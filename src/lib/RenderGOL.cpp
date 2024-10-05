#include "RenderGOL.hpp"
using namespace std;


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
MeshGL brush_meshGL;
bool need_update_bmgl = true;

int brush_size = 5;
glm::vec4 brush_color = glm::vec4(1.0f);

glm::vec2 last_mouse_pos = glm::vec2(0.0f);
glm::vec2 mouse_pos = glm::vec2(0.0f);

namespace displayProg {
    GLuint ID;
    vector<GLuint> gbLocs;
}

namespace stepProg {
	GLuint ID;
	vector<GLuint> gbLocs;

	GLuint xIncLoc;
	GLuint yIncLoc;
}

namespace geoMeshProg {
	GLuint ID;

	GLint modelMatLoc;
	GLint otherMatLoc;

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
}


inline void __recompute_frametime() {
	STEPTIME = 1'000'000 / (steps_per_frame*frames_per_second);
	cout << "FPS = " << frames_per_second 
		<< " | Steps per Frame = " << steps_per_frame 
		<< " | Steptime (ns) = " << STEPTIME << endl;
}

inline void __spf(int inc) {
	steps_per_frame = std::max(steps_per_frame+inc, MIN_SPF);
	__recompute_frametime();
}

inline void __fps(int inc) {
	frames_per_second = std::max(frames_per_second+inc, MIN_FPS);
	__recompute_frametime();
}


Mesh createQuad(glm::vec2 c1, glm::vec2 c2) {
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
	v0.color = glm::vec4(1.0f);
	v0.normal = glm::normalize(glm::vec3(-1,-1,1));
	v0.texcoord = glm::vec2(0,0);
	quad.vertices.push_back(v0);

	Vertex v1;
	v1.position = glm::vec3(c1.x, c2.y, 0.0f);
	v1.color = glm::vec4(1.0f);
	v1.normal = glm::normalize(glm::vec3(1,-1,1));
	v1.texcoord = glm::vec2(1,0);
	quad.vertices.push_back(v1);

	Vertex v2;
	v2.position = glm::vec3(c2.x, c1.y, 0.0f);
	v2.color = glm::vec4(1.0f);
	v2.normal = glm::normalize(glm::vec3(-1,1,1));
	v2.texcoord = glm::vec2(0,1);
	quad.vertices.push_back(v2);

	Vertex v3;
	v3.position = glm::vec3(c1.x, c1.y, 0.0f);
	v3.color = glm::vec4(1.0f);
	v3.normal = glm::normalize(glm::vec3(1,1,1));
	v3.texcoord = glm::vec2(1,1);
	quad.vertices.push_back(v3);

	quad.indices = { 0, 1, 2, 1, 3, 2 };

	return quad;
}

void __brush_set_color(glm::vec4 color) {
	boundBetween(color, 0.0f, 1.0f);
	for (Vertex &v : brush_mesh.vertices) {
		v.color = color;
	}
	need_update_bmgl = true;
}

void __brush_inc_color(glm::vec4 color) {
	color = brush_mesh.vertices[0].color + color;
	__brush_set_color(color);
}

inline void __brush_change_size(int diff) {
	brush_size = max(1, min(min(gbuff.width, gbuff.height), brush_size+diff));
	cout << "Brush size = " << brush_size << endl;
}

static void mouse_position_callback(GLFWwindow* window, double xpos, double ypos) {
	// Set last mouse pos
	last_mouse_pos = mouse_pos;
	
	// Get window size
	int fwidth, fheight;
	glfwGetFramebufferSize(window, &fwidth, &fheight);
	
	// Get new mouse pos
	mouse_pos = glm::vec2(float(xpos) / float(fwidth), float(ypos) / float(fheight));
	mouse_pos = (mouse_pos * 2.0f) - 1.0f;
	mouse_pos.y = -mouse_pos.y;
	
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
		__brush();
}

static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	if (action == GLFW_PRESS) {
		switch(button) {
			case GLFW_MOUSE_BUTTON_LEFT:
				last_mouse_pos = mouse_pos; // Mouse isn't moving so just use the current pos
				__brush();
				break;
		}
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
			__spf(1);
			break;
		case GLFW_KEY_DOWN:
			__spf(-1);
			break;
		case GLFW_KEY_RIGHT:
			__fps(1);
			break;
		case GLFW_KEY_LEFT:
			__fps(-1);
			break;
		case GLFW_KEY_I:
			__brush_inc_color(glm::vec4(0.1f, 0.0f, 0.0f, 0.0f));
			break;
		case GLFW_KEY_K:
			__brush_inc_color(glm::vec4(-0.1f,0.0f, 0.0f, 0.0f));
			break;
		case GLFW_KEY_O:
			__brush_inc_color(glm::vec4(0.0f, 0.1f, 0.0f, 0.0f));
			break;
		case GLFW_KEY_L:
			__brush_inc_color(glm::vec4(0.0f,-0.1f, 0.0f, 0.0f));
			break;
		case GLFW_KEY_P:
			__brush_inc_color(glm::vec4(0.0f, 0.0f, 0.1f, 0.0f));
			break;
		case GLFW_KEY_SEMICOLON:
			__brush_inc_color(glm::vec4(0.0f, 0.0f,-0.1f, 0.0f));
			break;
		case GLFW_KEY_U:
			__brush_change_size(1);
			break;
		case GLFW_KEY_J:
			__brush_change_size(-1);
			break;
		case GLFW_KEY_ENTER:
			__clear();
			break;
		}
	}
}

int initRenderer(FBO &fbo, float &aspect_ratio,
	char *stepFS_filepath, char *displayFS_filepath,
	char *geoVS_filepath, char *geoFS_filepath
) {
	window = setupGLFW("Press Space to Pause/Play", WINDOW_WIDTH, WINDOW_HEIGHT, DEBUG_MODE);
	setupGLEW(window);

	// Set callbacks
	glfwSetKeyCallback(window, key_callback);
	glfwSetCursorPosCallback(window, mouse_position_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);

	checkOpenGLVersion();

	if (DEBUG_MODE) checkAndSetupOpenGLDebugging();

	glClearColor(CLEAR_COLOR);

	// Create a load shaders
	try {
		stepProg::ID	= loadAndCreateShaderProgram(VERT_SHADER, stepFS_filepath);
		displayProg::ID	= loadAndCreateShaderProgram(VERT_SHADER, displayFS_filepath);
		geoMeshProg::ID	= loadAndCreateShaderProgram(geoVS_filepath, geoFS_filepath);
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

	glClearColor(CLEAR_COLOR);

		/// END OF RENDERING OPTIONS ///

	geoMeshProg::getLocations();
	displayProg::getLocations();
	stepProg::getLocations();
	glUseProgram(0);


	// Create screen quad
	Mesh m = createQuad();
	createMeshGL(m, SCREEN_QUAD);

	// Reuse for brush mesh
	brush_mesh = m;
	createMeshGL(brush_mesh, brush_meshGL);

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

inline void clearBuffer(bool color) {
	glClear(GL_DEPTH_BUFFER_BIT | (color ? GL_COLOR_BUFFER_BIT : 0));
}

void drawGeometry(Scene &scene, bool clear, const glm::mat4 &transform) {
	gbuff.swap();
	glViewport(0,0,GAME_WIDTH,GAME_HEIGHT);

	gbuff.bind();

	clearBuffer(clear);

	geoMeshProg::use(transform);
	geoMeshProg::renderScene(scene);

	gbuff.unbind();
	gbuff.swap();
}

void drawGeometry(MeshGL &mgl, bool clear, const glm::mat4 &transform) {
	gbuff.swap();
	glViewport(0,0,GAME_WIDTH,GAME_HEIGHT);

	gbuff.bind();

	clearBuffer(clear);

	geoMeshProg::use(transform);
	geoMeshProg::renderMesh(mgl);

	gbuff.unbind();
	gbuff.swap();
}

void drawGeometry(Mesh &mesh, bool clear, const glm::mat4 &transform) {
	MeshGL mgl;
	createMeshGL(mesh, mgl);

	drawGeometry(mgl, clear, transform);
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

	glm::vec2 travel, dir, mid;
	float length;

	travel = mouse_pos - last_mouse_pos;
	length = glm::length(travel);

	// If the travel distance of the mouse is negligable, just draw a dot
	if (abs(length) < 0.00001f) {
		length = 0.0f;
		dir = glm::vec2(1.0f, 0.0f);
		mid = mouse_pos;
	}
	// Otherwise, we'll rescale to a rectangle that covers the travel distance
	else {
		dir = travel / length;
		mid = last_mouse_pos + 0.5f*travel;
	}

	// Scale brush & travel size
	glm::mat2 scale = buildScale(glm::vec2(xoff+length*0.5f, yoff));

	// Rotate to travel direction
	glm::mat2 rot = glm::mat2(
		dir.x, dir.y, 
		-dir.y, dir.x
	);

	// Full-size rotation and scalaing matrix
	glm::mat4 transform = increaseMatrixSize<2>(rot*scale);
	// Add translation portion
	transform[3][0] = mid.x;
	transform[3][1] = mid.y;

	if (need_update_bmgl) {
		need_update_bmgl = false;
		updateMeshGL(brush_mesh, brush_meshGL);
	}

	drawGeometry(brush_meshGL, false, transform);
}


inline void __clear() {
	gbuff.bind();
	clearBuffer(true);
	gbuff.unbind();
	gbuff.swap();

	gbuff.bind();
	clearBuffer(true);
	gbuff.unbind();
	gbuff.swap();
}


void cleanupRenderer() {
	glDeleteProgram(geoMeshProg::ID);
	glDeleteProgram(displayProg::ID);
	glDeleteProgram(stepProg::ID);

	cleanupMesh(SCREEN_QUAD);
	cleanupGLFW(window);
}