#include "RenderGOL.hpp"
using namespace std;

#define STEP_FS "./src/shaders/BasicGOL/Step.fs"
#define DISP_FS "./src/shaders/BasicGOL/Display.fs"
#define GEO_VS  "./src/shaders/BasicGOL/GeoMesh.vs"
#define GEO_FS  "./src/shaders/BasicGOL/GeoMesh.fs"


int main() {
    FBO fbo;
	fbo.pushByteAttachment("life");

    float aspect_ratio;
    initRenderer(fbo, aspect_ratio, STEP_FS, DISP_FS, GEO_VS, GEO_FS);

    drawingLoop();

    cleanupRenderer();
}