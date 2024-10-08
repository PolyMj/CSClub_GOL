#include "RenderGOL.hpp"
using namespace std;

#define STEP_FS "./src/shaders/FloatingGOL/Step.fs"
#define DISP_FS "./src/shaders/FloatingGOL/Display.fs"
#define GEO__VS "./src/shaders/FloatingGOL/GeoMesh.vs"
#define GEO__FS "./src/shaders/FloatingGOL/GeoMesh.fs"


int main() {
    FBO fbo;
    fbo.pushFloatAttachment("life");

    float aspect_ratio;
    initRenderer(fbo, aspect_ratio, STEP_FS, DISP_FS, GEO__VS, GEO__FS);

    drawingLoop();

    cleanupRenderer();
}