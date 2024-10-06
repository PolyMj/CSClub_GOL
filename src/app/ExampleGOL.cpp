#include "RenderGOL.hpp"
#include <random>
using namespace std;

#define STEP_FS "./src/shaders/ExampleGOL/Step.fs"
#define DISP_FS "./src/shaders/ExampleGOL/Display.fs"

#define RATIO 0.02f
#define RANDOMNESS 10'000


inline float randFloat() {
    return float(rand() % (RANDOMNESS+1)) / float(RANDOMNESS);
}


// Variables for "epicenter"
glm::vec2 locData = glm::vec2(0.5f);
glm::vec2 newLoc = glm::vec2(0.5f);
uint16_t step = 120;
// Pick new epicenter, move towards epicenter
void locStep() {
    if (++step >= 180) {
        step = 0;
        newLoc = glm::vec2(randFloat(), randFloat());
        print(locData);
    }

    locData = locData * (1.0f - RATIO) + newLoc*RATIO;
}

int main() {
    // Create framebuffer
    FBO fbo;
    fbo.pushByteAttachment("lifeBool"); // 8-bit unsigned integer (1=alive, 0=dead)

    // Create a custom uniform variable (value that is the same no matter what pixel you're looking at)
    Uniform locUni = Uniform(Uniform::Vec2f, "randPoint", (void*)(&locData), locStep);
    stepProg::addCustomUniform(locUni);

    // Initialize renderer
    float aspect_ratio;
    initRenderer(fbo, aspect_ratio, STEP_FS, DISP_FS);

    // Enter drawing loop
    drawingLoop();
}