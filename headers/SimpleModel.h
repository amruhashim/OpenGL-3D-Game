#ifndef MODEL_H
#define MODEL_H

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // output data structure
#include <assimp/postprocess.h>     // post processing flags

#include "utilities.h"
#include "ShaderProgram.h"
#include "Texture.h"

struct Mesh
{
    // OpenGL buffer objects
    GLuint VBO = 0;
    GLuint IBO = 0;
    GLuint VAO = 0;
    int numOfIndices = 0;
    int numOfVertices = 0;
    bool hasTexCoords = false;

    Material material;
    Texture texture;
    Texture normalTexture;

    glm::mat4 modelMatrix;

    Mesh() : modelMatrix(1.0f)
    {
        material.Ka = glm::vec3(0.25f, 0.21f, 0.21f);
        material.Kd = glm::vec3(1.0f, 0.83f, 0.83f);
        material.Ks = glm::vec3(0.3f, 0.3f, 0.3f);
        material.shininess = 32.0f;
    }
};

/*****************************************************************
 * simple model class that loads the first mesh of a model
 *****************************************************************/

class SimpleModel
{
public:
    SimpleModel();
    ~SimpleModel();

    void loadModel(const char *filename, bool texture = false);
    void drawModel(GLenum topology = GL_TRIANGLES);

    inline Mesh* GetMesh() { return &mMesh; }

    bool mIsValid = false;

private:
    
    Mesh mMesh;
 
    void LoadMesh(const aiMesh *mesh);
    void loadMeshWithTexture(const aiMesh* mesh);
};

#endif
