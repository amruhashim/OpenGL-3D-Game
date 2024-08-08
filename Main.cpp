#include "utilities.h"
#include "Texture.h"
#include "Camera.h"
#include "SimpleModel.h"

// MARK: - Global Varibales

// Global variables for window dimensions
unsigned int gWindowWidth = 800;
unsigned int gWindowHeight = 600;

// Shader programs
ShaderProgram gShader;
ShaderProgram gLightShader;
ShaderProgram gNormalMapShader;

// Frame rate settings
float gFrameRate = 120.0f;
float gFrameTime = 1 / gFrameRate; // Frame time calculated based on frame rate

// Rotation settings
const float rotationSpeed = 20.0f; // Speed of rotation
float gRotation = 0.0f;            // Current rotation angle
bool rotationEnabled = true;       // Flag to control rotation

// Point light settings
Light gPointLight;  // Point light properties

// Models
SimpleModel torusModel;           // Torus model
SimpleModel floorModel;           // Floor model
SimpleModel viewportBorderModel;  // Viewport border model
SimpleModel wallModel;            // Wall model
SimpleModel paintingModel;        // Painting model

// Camera settings
float gYaw = 0.0;         // Yaw angle for camera orientation
float gPitch = 0.0;       // Pitch angle for camera orientation
float CameraZoom = 15.0f; // Zoom level for camera

// Viewport data structure to manage multiple viewports
struct ViewportData
{
    int x = 0;
    int y = 0;
    int width = 0;
    int height = 0;
    Camera cam; // Camera associated with the viewports

    ViewportData() {}

    // Constructor to initialize viewport data with specific dimensions
    ViewportData(int x, int y, int width, int height) : x(x), y(y), width(width), height(height) {}
};

std::vector<ViewportData> ViewportNumber; // Vector to store viewport data




// MARK: - Setup functions
// These functions initialize the geometry, materials, and textures for each respective model (for viewport border, floor, walls, and painting models)
    
void SetupViewportBorder() {
    auto mesh = viewportBorderModel.GetMesh();
    mesh->hasTexCoords = false;
    mesh->numOfIndices = 0;
    mesh->numOfVertices = 4;

    viewportBorderModel.mIsValid = true;

    std::vector<GLfloat> borderVertices = {
        -1.0f, 0.0f, 0.0f, // Vertex 1
         1.0f, 1.0f, 1.0f, // Color 1
         1.0f, 0.0f, 0.0f, // Vertex 2
         1.0f, 1.0f, 1.0f, // Color 2
         0.0f, 1.0f, 0.0f, // Vertex 3
         1.0f, 1.0f, 1.0f, // Color 3
         0.0f, -1.0f, 0.0f,// Vertex 4
         1.0f, 1.0f, 1.0f  // Color 4
    };

    // Generate and bind VAO
    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);

    // Generate, bind, and fill VBO
    glGenBuffers(1, &mesh->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * borderVertices.size(), borderVertices.data(), GL_STATIC_DRAW);

    // Define vertex attributes layout
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

    // Enable vertex attributes
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Unbind VAO
    glBindVertexArray(0);
}




void SetupFloor() {
    Mesh* mesh =  floorModel.GetMesh();

    // Configure mesh properties
    mesh->hasTexCoords = false;
    mesh->numOfIndices = 0;
    mesh->numOfVertices = 4;
    floorModel.mIsValid = true; 

    // Set material properties
    mesh->material.Ka = glm::vec3(0.25f, 0.21f, 0.21f);
    mesh->material.Kd = glm::vec3(1.0f, 0.83f, 0.83f);
    mesh->material.Ks = glm::vec3(0.3f, 0.3f, 0.3f);
    mesh->material.shininess = 11.3f;

    // Load texture
    mesh->texture.generate("./images/check.bmp");

    // Define floor vertices
    std::vector<GLfloat> vertices = {
        -1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   0.0f, 0.0f,
         1.0f, 0.0f, 1.0f,   0.0f, 1.0f, 0.0f,   5.0f, 0.0f,
        -1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,   0.0f, 5.0f,
         1.0f, 0.0f, -1.0f,  0.0f, 1.0f, 0.0f,   5.0f, 5.0f,
    };

    // Setup Vertex Buffer Object (VBO )
    glGenBuffers(1, &mesh->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    // Setup VAO (Vertex Array Object) and configure it
    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);

    // Set up vertex attributes
    auto setupVertexAttrib = [&](GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset) {
        glVertexAttribPointer(index, size, type, GL_FALSE, stride, reinterpret_cast<void*>(offset));
        glEnableVertexAttribArray(index);
    };

    GLsizei stride = sizeof(VertexNormTex);
    setupVertexAttrib(0, 3, GL_FLOAT, stride, offsetof(VertexNormTex, position));
    setupVertexAttrib(1, 3, GL_FLOAT, stride, offsetof(VertexNormTex, normal));
    setupVertexAttrib(2, 2, GL_FLOAT, stride, offsetof(VertexNormTex, texCoord));

    glBindVertexArray(0); // Unbind VAO
}




void SetupWalls() {
    Mesh* mesh = wallModel.GetMesh();

    // Set mesh properties
    mesh->hasTexCoords = false;
    mesh->numOfIndices = 0;
    mesh->numOfVertices = 4;
    wallModel.mIsValid = true; // Validate wall model

    // Texture loading
    mesh->texture.generate("./images/Fieldstone.bmp");
    mesh->normalTexture.generate("./images/FieldstoneBumpDOT3.bmp");

    // Material configuration
    mesh->material.Ka = glm::vec3(0.2f);
    mesh->material.Kd = glm::vec3(0.2f, 0.7f, 1.0f);
    mesh->material.Ks = glm::vec3(0.2f, 0.7f, 1.0f);
    mesh->material.shininess = 40.0f;

    // Define vertices with position, normal, tangent, and texture coordinates
    std::vector<GLfloat> vertices = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f, 0.0f,
        -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 2.0f,
         1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f,
    };

    // Setup Vertex Buffer Object (VBO )
    glGenBuffers(1, &mesh->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Setup VAO (Vertex Array Object) and configure it
    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);

    auto setVertexAttribute = [&](GLuint index, GLint size, GLenum type, bool normalized, GLsizei stride, std::size_t offset) {
        glVertexAttribPointer(index, size, type, normalized, stride, reinterpret_cast<void*>(offset));
        glEnableVertexAttribArray(index);
    };

    GLsizei stride = sizeof(VertexNormTanTex); // Assuming a struct named VertexNormTanTex defines the stride
    setVertexAttribute(0, 3, GL_FLOAT, GL_FALSE, stride, offsetof(VertexNormTanTex, position));
    setVertexAttribute(1, 3, GL_FLOAT, GL_FALSE, stride, offsetof(VertexNormTanTex, normal));
    setVertexAttribute(2, 3, GL_FLOAT, GL_FALSE, stride, offsetof(VertexNormTanTex, tangent));
    setVertexAttribute(3, 2, GL_FLOAT, GL_FALSE, stride, offsetof(VertexNormTanTex, texCoord));

    glBindVertexArray(0); // Unbind VAO
}




void SetupPainting() {
    Mesh* mesh = paintingModel.GetMesh();

    // Initialize mesh properties
    mesh->hasTexCoords = false;
    mesh->numOfIndices = 0;
    mesh->numOfVertices = 4;
    paintingModel.mIsValid = true; // Mark the painting model as valid

    // Set material properties
    mesh->material.Ka = glm::vec3(0.25f, 0.21f, 0.21f);
    mesh->material.Kd = glm::vec3(1.0f, 0.83f, 0.83f);
    mesh->material.Ks = glm::vec3(0.3f, 0.3f, 0.3f);
    mesh->material.shininess = 11.3f;

    // Generate texture
    mesh->texture.generate("./images/painting.png");

    // Define vertices for the painting
    std::vector<GLfloat> vertices = {
        -1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    };

    // Setup Vertex Buffer Object (VBO )
    glGenBuffers(1, &mesh->VBO);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices[0]) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // Setup VAO (Vertex Array Object) and configure it
    glGenVertexArrays(1, &mesh->VAO);
    glBindVertexArray(mesh->VAO);

    // Simplify attribute setup with a lambda function
    auto setupVertexAttrib = [&](GLuint index, GLint size, GLenum type, GLsizei stride, size_t offset) {
        glVertexAttribPointer(index, size, type, GL_FALSE, stride, reinterpret_cast<void*>(offset));
        glEnableVertexAttribArray(index);
    };

    GLsizei stride = sizeof(VertexNormTex); // Assuming VertexNormTex structure defines the layout
    setupVertexAttrib(0, 3, GL_FLOAT, stride, offsetof(VertexNormTex, position));
    setupVertexAttrib(1, 3, GL_FLOAT, stride, offsetof(VertexNormTex, normal));
    setupVertexAttrib(2, 2, GL_FLOAT, stride, offsetof(VertexNormTex, texCoord));

    glBindVertexArray(0); // Unbind VAOs
}




// MARK: - Initialization function
void init(GLFWwindow* window) {
    // Get the size of the framebuffer
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    // Update global window size variables based on the framebuffer size
    gWindowWidth = framebufferWidth;
    gWindowHeight = framebufferHeight;

    //Add andi Initialize viewport data based on the framebuffer size
    ViewportNumber.emplace_back(0, framebufferHeight / 2, framebufferWidth / 2, framebufferHeight / 2);
    ViewportNumber.emplace_back(framebufferWidth / 2, framebufferHeight / 2, framebufferWidth / 2, framebufferHeight / 2);
    ViewportNumber.emplace_back(0, 0, framebufferWidth / 2, framebufferHeight / 2);
    ViewportNumber.emplace_back(framebufferWidth / 2, 0, framebufferWidth / 2, framebufferHeight / 2);
    
    // Update projection matrices for each viewport
    float aspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
    float scale1 = tanf(glm::radians(60.0f) / 2.0f) * CameraZoom;
    float scale2 = tanf(glm::radians(60.0f) / 2.0f) * 12.0f;

    // Projection matrix for viewport 1
    glm::mat4 projMat = glm::ortho(-scale1 * aspectRatio, scale1 * aspectRatio, -scale1, scale1, 0.01f, 100.0f);
    ViewportNumber[1].cam.setProjMatrix(projMat);
    ViewportNumber[1].cam.setViewMatrix(glm::vec3(0.0f, CameraZoom, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0, -1.0));

    // Projection matrix for viewport 2
    projMat = glm::ortho(-scale2 * aspectRatio, scale2 * aspectRatio, -scale2, scale2, 0.01f, 100.0f);
    ViewportNumber[2].cam.setProjMatrix(projMat);
    ViewportNumber[2].cam.setViewMatrix(glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    // Projection matrix for viewport 3
    projMat = glm::perspective(glm::radians(70.0f), aspectRatio, 0.1f, 100.0f);
    ViewportNumber[3].cam.setProjMatrix(projMat);
    ViewportNumber[3].cam.setViewMatrix(glm::vec3(0.0f, 0.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));

    // Initialize shaders
    gShader.compileAndLink("SimpleTransform.vert", "color.frag");
    gLightShader.compileAndLink("lightingAndTexture.vert", "pointLightTexture.frag");
    gNormalMapShader.compileAndLink("normalMap.vert", "normalMap.frag");

    SetupViewportBorder();
    SetupFloor();
    SetupWalls();
    SetupPainting();
    
    /// Create Torus Model
    // Set material properties for the Torus Model
    torusModel.GetMesh()->material.Ka = glm::vec3(0.6f, 0.6f, 0.6f);  // Ambient color
    torusModel.GetMesh()->material.Kd = glm::vec3(1.0f, 1.0f, 1.0f);  // Diffuse color
    torusModel.GetMesh()->material.Ks = glm::vec3(0.8f, 0.8f, 0.8f);  // Specular color
    torusModel.GetMesh()->material.shininess = 40.0f;  // Shininess

    // Generate texture for the Torus Model
    torusModel.GetMesh()->texture.generate(
        "./images/cm_front.bmp", "./images/cm_back.bmp",
        "./images/cm_left.bmp", "./images/cm_right.bmp",
        "./images/cm_top.bmp", "./images/cm_bottom.bmp");

    // Load model data for the Torus Model
    torusModel.loadModel("./models/torus.obj");

    // Initialize model matrix for the Torus Model
    auto& torusModelMatrix = torusModel.GetMesh()->modelMatrix;
    torusModelMatrix = glm::mat4(1.0f);  // Identity matrix
    torusModelMatrix = glm::translate(torusModelMatrix, glm::vec3(0.0, -1.0, 0.0));  // Translate
    torusModelMatrix = glm::rotate(torusModelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0, 0));  // Rotate
    torusModelMatrix = glm::scale(torusModelMatrix, glm::vec3(2.0));  // Scale

    /// Setup light
    gPointLight.pos = glm::vec3(3.0f, 3.0f, 0.0f);
    gPointLight.La = glm::vec3(1.0f, 0.9f, 0.3f);
    gPointLight.Ld = glm::vec3(0.8f);
    gPointLight.Ls = glm::vec3(0.8f);
    gPointLight.att = glm::vec3(1.0f, 0.0f, 0.0f);

    // Set OpenGL state
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
}




// MARK: - Scene Rendering Function
static void RenderScene(ViewportData &viewportData)
{
    // Calculate the projection-view matrix
    glm::mat4 projViewMatrix = viewportData.cam.getProjMatrix() * viewportData.cam.getViewMatrix();

    // Use the light shader for rendering
    gLightShader.use();

    // Set light properties
    gLightShader.setUniform("uLight.pos", gPointLight.pos);
    gLightShader.setUniform("uLight.La", gPointLight.La);
    gLightShader.setUniform("uLight.Ld", gPointLight.Ld);
    gLightShader.setUniform("uLight.Ls", gPointLight.Ls);
    gLightShader.setUniform("uLight.att", gPointLight.att);
    gLightShader.setUniform("uViewpoint", viewportData.cam.getPosition());

    // Set material properties for the floor
    auto& floorMaterial = floorModel.GetMesh()->material;
    gLightShader.setUniform("uMaterial.Ka", floorMaterial.Ka);
    gLightShader.setUniform("uMaterial.Kd", floorMaterial.Kd);
    gLightShader.setUniform("uMaterial.Ks", floorMaterial.Ks);
    gLightShader.setUniform("uMaterial.shininess", floorMaterial.shininess);

    // Set model matrix for the floor
    glm::mat4 modelMatrix(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, -4.0, 0.0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(8.0, 1.0, 8.0));

    // Calculate MVP matrix and normal matrix for the floor
    glm::mat4 MVP = projViewMatrix * modelMatrix;
    glm::mat3 normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

    // Set uniform variables for the floor
    gLightShader.setUniform("uMVPMatrix", MVP);
    gLightShader.setUniform("uModelMatrix", modelMatrix);
    gLightShader.setUniform("uNormalMatrix", normalMatrix);
    gLightShader.setUniform("uTextureSampler", 0);
    gLightShader.setUniform("uEnvironmentMap", 1);
    gLightShader.setUniform("hasEnvMap", 0);

    // Bind textures and draw the floor model
    glActiveTexture(GL_TEXTURE0);
    floorModel.GetMesh()->texture.bind();
    floorModel.drawModel(GL_TRIANGLE_STRIP);

    // Set material properties for the painting
    auto& paintingMaterial = paintingModel.GetMesh()->material;
    gLightShader.setUniform("uMaterial.Ka", paintingMaterial.Ka);
    gLightShader.setUniform("uMaterial.Kd", paintingMaterial.Kd);
    gLightShader.setUniform("uMaterial.Ks", paintingMaterial.Ks);
    gLightShader.setUniform("uMaterial.shininess", paintingMaterial.shininess);

    // Set model matrix for the painting
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 0.0f, -3.9f));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(3.0f, 1.0f, 2.0f));

    // Calculate MVP matrix and normal matrix for the painting
    MVP = projViewMatrix * modelMatrix;
    normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));

    // Set uniform variables for the painting
    gLightShader.setUniform("uMVPMatrix", MVP);
    gLightShader.setUniform("uModelMatrix", modelMatrix);
    gLightShader.setUniform("uNormalMatrix", normalMatrix);

    // Bind textures and draw the painting model
    glActiveTexture(GL_TEXTURE0);
    paintingModel.GetMesh()->texture.bind();
    paintingModel.drawModel(GL_TRIANGLE_STRIP);

    // Set material properties for the torus
    auto& torusMaterial = torusModel.GetMesh()->material;
    gLightShader.setUniform("uMaterial.Ka", torusMaterial.Ka);
    gLightShader.setUniform("uMaterial.Kd", torusMaterial.Kd);
    gLightShader.setUniform("uMaterial.Ks", torusMaterial.Ks);
    gLightShader.setUniform("uMaterial.shininess", torusMaterial.shininess);

    // Set model matrix for the torus
    auto torusModelMatrix = torusModel.GetMesh()->modelMatrix;

    // Calculate MVP matrix and normal matrix for the torus
    MVP = projViewMatrix * torusModelMatrix;
    normalMatrix = glm::mat3(glm::transpose(glm::inverse(torusModelMatrix)));

    // Set uniform variables for the torus
    gLightShader.setUniform("uMVPMatrix", MVP);
    gLightShader.setUniform("uModelMatrix", torusModelMatrix);
    gLightShader.setUniform("uNormalMatrix", normalMatrix);
    gLightShader.setUniform("uEnvironmentMap", 0);
    gLightShader.setUniform("uTextureSampler", 1);
    gLightShader.setUniform("hasEnvMap", 1);

    // Bind textures and draw the torus model
    glActiveTexture(GL_TEXTURE0);
    torusModel.GetMesh()->texture.bind();
    torusModel.drawModel();

    // Use the normal map shader for rendering
    gNormalMapShader.use();

    // Set light properties for the normal map shader
    gNormalMapShader.setUniform("uLight.pos", gPointLight.pos);
    gNormalMapShader.setUniform("uLight.La", gPointLight.La);
    gNormalMapShader.setUniform("uLight.Ld", gPointLight.Ld);
    gNormalMapShader.setUniform("uLight.Ls", gPointLight.Ls);
    gNormalMapShader.setUniform("uLight.att", gPointLight.att);
    gNormalMapShader.setUniform("uViewpoint", viewportData.cam.getPosition());

    // Set material properties for the wall
    auto& wallMaterial = wallModel.GetMesh()->material;
    gNormalMapShader.setUniform("uMaterial.Ka", wallMaterial.Ka);
    gNormalMapShader.setUniform("uMaterial.Kd", wallMaterial.Kd);
    gNormalMapShader.setUniform("uMaterial.Ks", wallMaterial.Ks);
    gNormalMapShader.setUniform("uMaterial.shininess", wallMaterial.shininess);
    gNormalMapShader.setUniform("uTextureSampler", 0);
    gNormalMapShader.setUniform("uNormalSampler", 1);

    // Bind textures for the wall model
    glActiveTexture(GL_TEXTURE0);
    wallModel.GetMesh()->texture.bind();
    glActiveTexture(GL_TEXTURE1);
    wallModel.GetMesh()->normalTexture.bind();

    // Render the back wall
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, -4.0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(8.0, 4.0, 1.0));
    MVP = projViewMatrix * modelMatrix;
    normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    gNormalMapShader.setUniform("uMVPMatrix", MVP);
    gNormalMapShader.setUniform("uModelMatrix", modelMatrix);
    gNormalMapShader.setUniform("uNormalMatrix", normalMatrix);
    wallModel.drawModel(GL_TRIANGLE_STRIP);

    // Render the left wall
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(-8.0f, 0.0, 0.0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(90.0f), glm::vec3(0, 1.0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(8.0, 4.0, 1.0));
    MVP = projViewMatrix * modelMatrix;
    normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    gNormalMapShader.setUniform("uMVPMatrix", MVP);
    gNormalMapShader.setUniform("uModelMatrix", modelMatrix);
    gNormalMapShader.setUniform("uNormalMatrix", normalMatrix);
    wallModel.drawModel(GL_TRIANGLE_STRIP);

    // Render the right wall
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(8.0f, 0.0, 0.0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(-90.0f), glm::vec3(0, 1.0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(8.0, 4.0, 1.0));
    MVP = projViewMatrix * modelMatrix;
    normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    gNormalMapShader.setUniform("uMVPMatrix", MVP);
    gNormalMapShader.setUniform("uModelMatrix", modelMatrix);
    gNormalMapShader.setUniform("uNormalMatrix", normalMatrix);
    wallModel.drawModel(GL_TRIANGLE_STRIP);

    // Render the front wall
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0, 0.0, 8.0));
    modelMatrix = glm::rotate(modelMatrix, glm::radians(180.0f), glm::vec3(0, 1.0, 0));
    modelMatrix = glm::scale(modelMatrix, glm::vec3(8.0, 4.0, 1.0));
    MVP = projViewMatrix * modelMatrix;
    normalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
    gNormalMapShader.setUniform("uMVPMatrix", MVP);
    gNormalMapShader.setUniform("uModelMatrix", modelMatrix);
    gNormalMapShader.setUniform("uNormalMatrix", normalMatrix);
    wallModel.drawModel(GL_TRIANGLE_STRIP);
}




// MARK: - Scene Update Function
static void UpdateScene(GLFWwindow* window)
{
    // Calculate time elapsed since last frame
    static float lastFrameTime = 0.0f;
    float currentFrameTime = glfwGetTime();
    gFrameTime = currentFrameTime - lastFrameTime;
    lastFrameTime = currentFrameTime;

    if (rotationEnabled)
    {
        gRotation += rotationSpeed * gFrameTime;
        if (gRotation >= 360.0f)
            gRotation = 0.0f;

        // Update torus model matrix
        auto& torusModelMatrix = torusModel.GetMesh()->modelMatrix;
        torusModelMatrix = glm::mat4(1.0f); // Identity matrix initialization
        torusModelMatrix = glm::translate(torusModelMatrix, glm::vec3(0.0, -1.0, 0.0));
        torusModelMatrix = glm::rotate(torusModelMatrix, glm::radians(gRotation * 5), glm::vec3(0.0f, 1.0f, 0.0f));
        torusModelMatrix = glm::rotate(torusModelMatrix, glm::radians(90.0f), glm::vec3(1.0, 0, 0));
        torusModelMatrix = glm::scale(torusModelMatrix, glm::vec3(2.0));
    }

    // Update camera yaw and pitch
    ViewportNumber[3].cam.mYaw = gYaw;
    ViewportNumber[3].cam.mPitch = gPitch;

    // Update camera
    ViewportNumber[3].cam.update(0.0f, 0.0f);
}




// MARK: - Rendering ViewPorts based on index
void RenderViewports(int viewportIndex) {
    auto& viewportData = ViewportNumber[viewportIndex];
    
    glViewport(viewportData.x, viewportData.y, viewportData.width, viewportData.height);

    if (viewportIndex == 0) {
        // Render GUI for viewport 0
        TwDraw();
    } else {
        // Render scene for viewports 1, 2, and 3
        RenderScene(viewportData);
    }
}




// MARK: - Main Viewport Rendering
void Render() {
    // Clear colour buffer and depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render 4 viewports
    for (int i = 0; i < 4; ++i) {
        RenderViewports(i);
    }

    // Render the main viewport border
    glViewport(0, 0, gWindowWidth, gWindowHeight); // Set the viewport to cover the entire window
    gShader.use(); // Use the shader for rendering
    glm::mat4 MVP(1.0f); // Identity matrix for MVP (no transformation)
    gShader.setUniform("uMVPMatrix", MVP); // Set the MVP matrix uniform
    viewportBorderModel.drawModel(GL_LINES); // Draw the viewport border model as lines

    glFlush();
}




static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// close the window when the ESCAPE key is pressed
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
		// set flag to close the window
		glfwSetWindowShouldClose(window, GL_TRUE);
		return;
	}
}




static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
{
	// pass cursor position to tweak bar
	TwEventMousePosGLFW(static_cast<int>(xpos), static_cast<int>(ypos));
}




static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	// pass mouse button status to tweak bar
	TwEventMouseButtonGLFW(button, action);
}




static void error_callback(int error, const char* description)
{
	std::cerr << description << std::endl;	// output error description
}




// MARK: - Window Size Manager
static void macOS_HiDPI_WindowManager(GLFWwindow* window, int w, int h) {
    // Get the size of the framebuffer to handle high-DPI displays.
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);

    // Update global variables to reflect the framebuffer size.
    gWindowWidth = framebufferWidth;
    gWindowHeight = framebufferHeight;

    // Update Viewport dimensions
    ViewportNumber[0] = ViewportData(0, gWindowHeight / 2, gWindowWidth / 2, gWindowHeight / 2);
    ViewportNumber[1] = ViewportData(gWindowWidth / 2, gWindowHeight / 2, gWindowWidth / 2, gWindowHeight / 2);
    ViewportNumber[2] = ViewportData(0, 0, gWindowWidth / 2, gWindowHeight / 2);
    ViewportNumber[3] = ViewportData(gWindowWidth / 2, 0, gWindowWidth / 2, gWindowHeight / 2);

    // Recalculate scale for the orthographic projection based on the new window size
    float scale1 = tanf(glm::radians(60.0f) / 2.0f) * CameraZoom ;
    float scale2 = tanf(glm::radians(60.0f) / 2.0f) * 12.0f;
    
    float aspectRatio = static_cast<float>(framebufferWidth) / static_cast<float>(framebufferHeight);
    float scaleWidth1 = scale1 * aspectRatio;
    float scaleWidth2 = scale2 * aspectRatio;

    glm::mat4 projMat = glm::ortho(-scaleWidth1, scaleWidth1, -scale1, scale1, 0.01f, 100.0f);
    ViewportNumber[1].cam.setProjMatrix(projMat);
    ViewportNumber[1].cam.setViewMatrix(glm::vec3(0.0f, CameraZoom , 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 0.0, -1.0));
   
    projMat = glm::ortho(-scaleWidth2, scaleWidth2, -scale2, scale2, 0.01f, 100.0f);
    ViewportNumber[2].cam.setProjMatrix(projMat);
    ViewportNumber[2].cam.setViewMatrix(glm::vec3(0.0f, 0.0f, 8.0f), glm::vec3(0.0f, 0.0f, 0.0f));
   
    projMat = glm::perspective(glm::radians(70.0f), aspectRatio, 0.1f, 100.0f);
    ViewportNumber[3].cam.setProjMatrix(projMat);
    ViewportNumber[3].cam.setViewMatrix(glm::vec3(0.0f, 0.0f, 7.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0, 1.0, 0.0));

    // Update GUI size
    TwWindowSize(gWindowWidth, gWindowHeight);
}




// MARK: - GUI Interface
TwBar* CreateUI(const std::string name)
{
	TwBar* twBar = TwNewBar(name.c_str());

	TwWindowSize(gWindowWidth / 2, gWindowHeight);
	TwDefine(" TW_HELP visible=false ");
	TwDefine(" GLOBAL fontsize=3 ");

	TwDefine(" Main label='Controls' refresh=0.02 text=light size='250 250' ");

	TwAddVarRO(twBar, "FPS", TW_TYPE_FLOAT, &gFrameRate, " group='Frame Statistics' precision=2 ");
	TwAddVarRO(twBar, "Frame Time", TW_TYPE_FLOAT, &gFrameTime, " group='Frame Statistics' ");

	TwAddVarRW(twBar, "Toggle", TW_TYPE_BOOLCPP, &rotationEnabled, " group='Animation' ");

	TwAddVarRW(twBar, "Position X", TW_TYPE_FLOAT, &gPointLight.pos.x, " group='Light' step=0.01 ");
	TwAddVarRW(twBar, "Position Y", TW_TYPE_FLOAT, &gPointLight.pos.y, " group='Light' step=0.01 ");
	TwAddVarRW(twBar, "Position Z", TW_TYPE_FLOAT, &gPointLight.pos.z, " group='Light' step=0.01");

	TwAddVarRW(twBar, "Yaw", TW_TYPE_FLOAT, &gYaw, " group='Camera' step=0.01");
	TwAddVarRW(twBar, "Pitch", TW_TYPE_FLOAT, &gPitch, " group='Camera' step=0.01");

	return twBar;
}




// MARK: - Main function
int main(void)
{
    GLFWwindow* window = nullptr;

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
    {
        exit(EXIT_FAILURE);
    }

    // minimum OpenGL version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(gWindowWidth, gWindowHeight, "Assignment 2 - abah609, 7571562", nullptr, nullptr);

    if (window == nullptr)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);
    int framebufferWidth, framebufferHeight;
    glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight);
    glViewport(0, 0, framebufferWidth, framebufferHeight);

    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK)
    {
        std::cerr << "GLEW initialisation failed" << std::endl;
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetWindowSizeCallback(window, macOS_HiDPI_WindowManager);

    init(window);

    TwInit(TW_OPENGL_CORE, nullptr);
    TwBar* tweakBar = CreateUI("Main");


    double lastUpdateTime = glfwGetTime();
    double elapsedTime = lastUpdateTime;
    int frameCount = 0;


    while (!glfwWindowShouldClose(window))
    {
        UpdateScene(window);

        Render();

        glfwSwapBuffers(window);
        glfwPollEvents();

        frameCount++;
        elapsedTime = glfwGetTime() - lastUpdateTime;

        // if elapsed time since last update > 1 second
        if (elapsedTime > 1.0)
        {
            gFrameTime = elapsedTime / frameCount;  // average time per frame
            gFrameRate = 1 / gFrameTime;            // frames per second
            lastUpdateTime = glfwGetTime();         // set last update time to current time
            frameCount = 0;                         // reset frame counter
        }
    }
    

    TwDeleteBar(tweakBar);
    TwTerminate();

    glfwDestroyWindow(window);
    glfwTerminate();

    
    
    exit(EXIT_SUCCESS);
}
