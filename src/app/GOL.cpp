#include "RenderGOL.hpp"
#include "Buffer.hpp"
#include "Scene.hpp"
using namespace std;

#define STEP_FS "./src/shaders/FloatingGOL/Step.fs"
#define DISP_FS "./src/shaders/FloatingGOL/Display.fs"
#define GEO_VS  "./src/shaders/FloatingGOL/GeoMesh.vs"
#define GEO_FS  "./src/shaders/FloatingGOL/GeoMesh.fs"

#define CAMERA_POS      0.0f, 0.0f, 1.0f
#define CAMERA_LOOKAT   0.0f, 0.0f, 0.0f
#define FOV             90.0f
#define NEAR_PLANE      0.001f
#define FAR_PLANE       100.0f


int main() {
    FBO fbo;
	fbo.pushFloatAttachment("life");

    float aspect_ratio;
    initRenderer(fbo, aspect_ratio, STEP_FS, DISP_FS, GEO_VS, GEO_FS);

    
    Scene scene = importSceneFromFile("./assets/models/bunnyteatime.glb");
    glm::mat4 viewMat = glm::lookAt(glm::vec3(CAMERA_POS), glm::vec3(CAMERA_LOOKAT), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projMat = glm::perspective(glm::radians(FOV), aspect_ratio, NEAR_PLANE, FAR_PLANE);
    glm::mat4 viewProjMat = projMat * viewMat;

    drawGeometry(scene, true, viewProjMat);

    drawingLoop();

    cleanupRenderer();
}