#include "RenderGOL.hpp"
#include <random>
using namespace std;

#define STEP_FS "./src/shaders/ExampleGOL/Step.fs"
#define DISP_FS "./src/shaders/ExampleGOL/Display.fs"

#define RATIO 0.002f
#define RANDOMNESS 10'000
#define STEPS 500
#define EX_RANGE 0.5f


const float S1 = (1.0f + EX_RANGE), S2 = (1.0f + EX_RANGE*2.0f);
// Get a random float within a range of [-EX_RANGE, 1.0+EX_RANGE]
inline float randFloat() {
    return S1 - S2*float(rand() % (RANDOMNESS+1)) / float(RANDOMNESS);
}


// Variables for "epicenter"
glm::vec2 locData = glm::vec2(0.5f); // Actual location of epicenter
glm::vec2 intpLoc = glm::vec2(0.5f); // Point that epicenter is moving towards
glm::vec2 newLoc = glm::vec2(0.5f); // Point that the above point is moving towards
uint16_t step = STEPS;

// Pick new epicenter, move towards epicenter
void locStep() {
    // Every now and then...
    if (++step > STEPS) { step = 0;
        newLoc = glm::vec2(randFloat(), randFloat()); // Get new random location to accelerate towards
    }

    // Move points
    intpLoc = intpLoc*(1.0f - RATIO) + newLoc*RATIO;
    locData = locData*(1.0f - RATIO) + intpLoc*RATIO;
}



int main() {
    // Create framebuffer
    FBO fbo;
    fbo.pushUInt8Attachment("lifeBool", GL_NEAREST, GL_CLAMP_TO_BORDER); // 8-bit unsigned integer (1=alive, 0=dead)

    // Create a custom uniform variable (value that is the same no matter what pixel you're looking at)
    Uniform locUni = Uniform(Uniform::Vec2f, "epicenter", (void*)(&locData), locStep);
    stepProg::addCustomUniform(locUni);

    // Initialize renderer
    float aspect_ratio;
    initRenderer(fbo, aspect_ratio, STEP_FS, DISP_FS);

    // Enter drawing loop
    drawingLoop();
}