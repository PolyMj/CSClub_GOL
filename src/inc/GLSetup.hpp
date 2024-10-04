///// Credit: Dr. Michael J. Reale /////

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <vector>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "SETTINGS.hpp"
using namespace std;

GLFWwindow* setupGLFW(string windowTitle, int windowWidth, int windowHeight, bool debugging);
void cleanupGLFW(GLFWwindow* window);
void setupGLEW(GLFWwindow* window);
void checkOpenGLVersion();
void checkAndSetupOpenGLDebugging();