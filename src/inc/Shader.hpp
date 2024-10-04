///// Credits: Dr. Michael J. Reale //////

#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cstring>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
using namespace std;

string readFileToString(string filename);
void printVertFragCode(string &vertexCode, string &fragCode);
void printShaderCode(string &code, const char* title);
GLuint createAndCompileShader(const char *chaderCode, GLenum shaderType);
GLuint createAndLinkShaderProgram(std::vector<GLuint> allShaderIDs);
GLuint initShaderProgramFromSource(string vertexShaderCode, string fragmentShaderCode);
GLuint loadAndCreateShaderProgram(const string &vertFile, const string &fragFile, bool debug = false);