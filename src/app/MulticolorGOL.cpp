#include "RenderGOL.hpp"
using namespace std;

#define STEP_FS "./src/shaders/Multicolor/Step.fs"
#define DISP_FS "./src/shaders/Multicolor/Display.fs"
#define GEOM_VS "./src/shaders/Multicolor/Geo.vs"
#define GEOM_FS "./src/shaders/Multicolor/Geo.fs"


int main() {
    FBO fbo;
    fbo.pushVec3Float16Attachment("life");

    float aspect_ratio;
    initRenderer(fbo, aspect_ratio, STEP_FS, DISP_FS, GEOM_VS, GEOM_FS);

    drawingLoop();

    cleanupRenderer();
}