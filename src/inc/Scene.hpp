#pragma once

#include <iostream>
#include <vector>
#include "MeshData.hpp"
#include "MeshGLData.hpp"
#include <assimp/Importer.hpp> // For importing 3D models
#include <assimp/scene.h> // For aiScene type
#include <assimp/postprocess.h> // For postprocessing flags
using namespace std;

Assimp::Importer importer;
unsigned int DEFAULT_FLAGS = (
    aiProcess_Triangulate | 
    aiProcess_FlipUVs | 
    aiProcess_GenNormals | 
    aiProcess_JoinIdenticalVertices
);

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
void extractMeshData(aiMesh *mesh, Mesh &m, glm::vec4 vertex_color = glm::vec4(1.0f)) {
    // Clear vertices/indicies
    m.vertices.clear();
    m.indices.clear();

    // Loop through vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        // Create a Vertex
        Vertex v;
        v.color = glm::vec4(vertex_color);

        // Get vertex position infor from mesh and store it in the new Vertex
        aiVector3D aiV = mesh->mVertices[i];
        v.position = glm::vec3(aiV[0], aiV[1], aiV[2]);

        // Get normal for the vertex
        aiVector3D aiN = mesh->mNormals[i];
        v.normal = glm::vec3(aiN[0], aiN[1], aiN[2]);
        
        // Add new vertex to the mesh
        m.vertices.push_back(v);
    }

    // Loop through all faces
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        // Get aiFace
        aiFace aiF = mesh->mFaces[i];

        // For each vert index in the face, add index to m.indices
        for (unsigned int j = 0; j < aiF.mNumIndices; j++) {
            m.indices.push_back(aiF.mIndices[j]);
        }
    }
}

// Creates and returns a Scene imported from the given file
Scene importSceneFromFile(char *file, int flags = DEFAULT_FLAGS) {
    Scene scene = {
        vector<MeshGL>(),
        importer.ReadFile(file, flags)
    };

    // Import file

    // Check for errors when loading the aiScene
    if (scene.ais == NULL) {
        cerr << "Object Load Error: Resulting scene is NULL | ";
        cerr << importer.GetErrorString() << endl;
        exit(1);
    }
    if (scene.ais->mFlags & AI_SCENE_FLAGS_INCOMPLETE) {
        cerr << "Object Load Error: Flags incomplete" << endl;
        exit(1);
    }
    if (scene.ais->mRootNode == NULL) {
        cerr << "Object Load Error: Root node is NULL" << endl;
        exit(1);
    }

    
    // Create list of meshes
    for (unsigned int i = 0; i < scene.ais->mNumMeshes; i++) {
        // Get Mesh
        Mesh m;
        extractMeshData(scene.ais->mMeshes[i], m);
        // Convert to MeshGL
        MeshGL mgl;
        createMeshGL(m, mgl);
        // Add MeshGL
        scene.allMeshes.push_back(mgl);
    }

    return scene;
};

