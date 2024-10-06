#include "RenderGOL.hpp"
using namespace std;

#define STEP_FS "./src/shaders/BasicGOL/Step.fs"
#define DISP_FS "./src/shaders/BasicGOL/Display.fs"
#define GEO_VS  "./src/shaders/BasicGOL/GeoMesh.vs"
#define GEO_FS  "./src/shaders/BasicGOL/GeoMesh.fs"

#define CAMERA_POS      0.0f, 0.0f, 1.0f
#define CAMERA_LOOKAT   0.0f, 0.0f, 0.0f
#define FOV             90.0f
#define NEAR_PLANE      0.001f
#define FAR_PLANE       100.0f


glm::vec2 locData = glm::vec2(-1.0f);
void locStep() {
    locData = locData + glm::vec2(0.002f, 0.0037f);
    print(locData);
}


int main() {
    FBO fbo;
	fbo.pushByteAttachment("life");

    Uniform locUni = Uniform(Uniform::Vec2f, "POI", (void*)(&locData), locStep);
    stepProg::addCustomUniform(locUni);

    float aspect_ratio;
    initRenderer(fbo, aspect_ratio, STEP_FS, DISP_FS, GEO_VS, GEO_FS);

    /// DRAW INITIAL CONDITIONS /// Not required, can use brush instead ///
    Scene scene = importSceneFromFile("./assets/models/bunnyteatime.glb");
    glm::mat4 viewMat = glm::lookAt(glm::vec3(CAMERA_POS), glm::vec3(CAMERA_LOOKAT), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projMat = glm::perspective(glm::radians(FOV), aspect_ratio, NEAR_PLANE, FAR_PLANE);
    glm::mat4 viewProjMat = projMat * viewMat;
    drawGeometry(scene, true, viewProjMat);
    /// END DRAW INITIAL ///

    drawingLoop();

    cleanupRenderer();
}