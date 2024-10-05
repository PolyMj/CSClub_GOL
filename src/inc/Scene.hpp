#pragma once

#include <iostream>
#include <vector>
#include "MeshData.hpp"
#include "MeshGLData.hpp"
#include <assimp/Importer.hpp> // For importing 3D models
#include <assimp/scene.h> // For aiScene type
#include <assimp/postprocess.h> // For postprocessing flags
using namespace std;

extern Assimp::Importer importer;
extern const unsigned int DEFAULT_FLAGS;

struct Scene {
    vector<MeshGL> allMeshes;
    const aiScene* ais;

    void cleanup() {
        for (MeshGL &m : allMeshes) {
            cleanupMesh(m);
        }
    };
};


// Get data from aiMesh (mesh) and place it into a Mesh (m)
void extractMeshData(aiMesh *mesh, Mesh &m, glm::vec4 vertex_color = glm::vec4(1.0f));

// Creates and returns a Scene imported from the given file
Scene importSceneFromFile(char *file, int flags = DEFAULT_FLAGS);

