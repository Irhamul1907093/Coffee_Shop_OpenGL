#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "pointLight.h"
#include "sphere.h"
#include "cube.h" //e
#include "stb_image.h" //e

#include <iostream>

using namespace std;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model, float r, float g, float b, float shininess);
void axis(unsigned int& cubeVAO, Shader& lightingShader);
void bed(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether);
void floor(unsigned int& cubeVAO, Shader& lightingShader);
void frontWall(unsigned int& cubeVAO, Shader& lightingShader, Cylinder& cylinder, HemiSphere& hemisphere, Cone& cone, CutCone& con);
void rightWall(unsigned int& cubeVAO, Shader& lightingShader, CutCone& cone, Cylinder& cylinder);
void ambienton_off(Shader& lightingShader);
void diffuse_on_off(Shader& lightingShader);
void specular_on_off(Shader& lightingShader);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);


void drawSphere(Shader& shader, glm::mat4 model, float r, float g, float b);

glm::mat4 myPerspective(float fov, float aspect, float near, float far) {
    glm::mat4 result(0.0f); // Initialize to a zero matrix

    float tanHalfFovy = tan(fov / 2.0f);

    result[0][0] = 1.0f / (aspect * tanHalfFovy);
    result[1][1] = 1.0f / tanHalfFovy;
    result[2][2] = -(far + near) / (far - near);
    result[2][3] = -1.0f;
    result[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// Time management
double lastKeyPressTime = 0.0;
const double keyPressDelay = 0.2; // delay in seconds

// modelling transform
float rotateAngle_X = 0.0;
float rotateAngle_Y = 0.0;
float rotateAngle_Z = 0.0;
float rotateAxis_X = 0.0;
float rotateAxis_Y = 0.0;
float rotateAxis_Z = 1.0;
float translate_X = 0.0;
float translate_Y = 0.0;
float translate_Z = 0.0;
float scale_X = 1.0;
float scale_Y = 1.0;
float scale_Z = 1.0;

// camera
//Camera camera(glm::vec3(0.0f, 1.1f, -5.2f));
Camera camera(glm::vec3(-11.0f, 14.1f, 12.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);


// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(11.50f,  2.50f,  -0.5f),
    glm::vec3(4.5f,  2.50f,  8.5f)
    //glm::vec3(-1.5f,  1.5f,  0.0f),
    //glm::vec3(-1.5f,  -1.5f,  0.0f)
};
PointLight pointlight1(

    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1       // light number
);
PointLight pointlight2(

    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    2       // light number
);
//PointLight pointlight3(
//
//    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
//    0.05f, 0.05f, 0.05f,     // ambient
//    0.8f, 0.8f, 0.8f,     // diffuse
//    1.0f, 1.0f, 1.0f,        // specular
//    1.0f,   //k_c
//    0.09f,  //k_l
//    0.032f, //k_q
//    3       // light number
//);
//PointLight pointlight4(
//
//    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
//    0.05f, 0.05f, 0.05f,     // ambient
//    0.8f, 0.8f, 0.8f,     // diffuse
//    1.0f, 1.0f, 1.0f,        // specular
//    1.0f,   //k_c
//    0.09f,  //k_l
//    0.032f, //k_q
//    4       // light number
//);


// light settings
bool pointLightOn = true;
bool directionalLightOn = true;
bool SpotLightOn = true;
bool AmbientON = true;
bool DiffusionON = true;
bool SpecularON = true;
bool ambientToggle = true;
bool diffuseToggle = true;
bool specularToggle = true;


// timing
float deltaTime = 0.0f;    // time between current frame and last frame
float lastFrame = 0.0f;

unsigned int sphereVAO, sphereVBO, sphereEBO;
std::vector<float> sphereVertices;
std::vector<unsigned int> sphereIndices;

// Torus Variables
unsigned int torusVAO, torusVBO, torusEBO;
std::vector<float> torusVertices;
std::vector<unsigned int> torusIndices;

// Function to create a torus
void createTorus(float innerRadius, float outerRadius, int sides, int rings,
    std::vector<float>& vertices, std::vector<unsigned int>& indices);

// Function to draw the torus
void drawTorus(Shader& shader, glm::mat4 model, float r, float g, float b);


int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CSE 4208: Computer Graphics Laboratory", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    //glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------
    Shader lightingShader("vertexShaderForPhongShading.vs", "fragmentShaderForPhongShading.fs");
    //Shader lightingShader("vertexShaderForGouraudShading.vs", "fragmentShaderForGouraudShading.fs");
    Shader ourShader("vertexShader.vs", "fragmentShader.fs");
    Shader lightingShaderWithTexture("vertexShaderForPhongShadingWithTexture.vs", "fragmentShaderForPhongShadingWithTexture.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------


    float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2,
        2, 1, 0,

        4, 5, 7,
        7, 6, 4,

        8, 9, 10,
        10, 11, 8,

        12, 13, 14,
        14, 15, 12,

        16, 17, 18,
        18, 19, 16,

        20, 21, 22,
        22, 23, 20
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);


    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // vertex normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)12);
    glEnableVertexAttribArray(1);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    // note that we update the lamp's position attribute's stride to b reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    string laughEmoPath = "football.jpg";
    string coffeePath = "cf.png";
    //string patrn = "vector.jpg";
    unsigned int coffepic = loadTexture(coffeePath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);

    unsigned int laughEmoji = loadTexture(laughEmoPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube = Cube(coffepic, coffepic, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    SphereTex spheretex = SphereTex();
    CylinderTex cylindertex = CylinderTex();
    ConeTex conetex = ConeTex();

    Sphere sphere = Sphere();
    Cylinder cylinder = Cylinder();
    Cone cone = Cone();
    CutCone cutcone = CutCone();
    HemiSphere hemisphere = HemiSphere();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);



    //ourShader.use();
    //lightingShader.use();

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // pass projection matrix to shader (note that in this case it could change every frame)
        //glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);


        // camera/view transformation
        //glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();



        // point light 1
        pointlight1.setUpPointLight(lightingShader);
        // point light 2
        pointlight2.setUpPointLight(lightingShader);
        // point light 3
        //pointlight3.setUpPointLight(lightingShader);
        // point light 4
        //pointlight4.setUpPointLight(lightingShader);


        // activate shader
        lightingShader.use();

        lightingShader.setVec3("directionalLight.direction", 0.5f, -3.0f, -3.0f);
        if (AmbientON) {
            lightingShader.setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
            lightingShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
        }
        if (DiffusionON) {
            lightingShader.setVec3("directionalLight.diffuse", 0.8f, 0.8f, 0.8f);
            lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        }
        if (SpecularON) {

            lightingShader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);
            lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        }

        lightingShader.setBool("directionalLightON", directionalLightOn);
        lightingShader.setBool("SpotLightON", SpotLightOn);

        lightingShader.setVec3("spotLight.direction", 0.0f, -1.0f, 0.0f);
        lightingShader.setVec3("spotLight.position", -3.0f, 4.0f, 4.0f);



        lightingShader.setFloat("spotLight.k_c", 1.0f);
        lightingShader.setFloat("spotLight.k_l", 0.09f);
        lightingShader.setFloat("spotLight.k_q", 0.032f);
        lightingShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(35.5f)));
        lightingShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(40.5f)));

        // pass projection matrix to shader (note that in this case it could change every frame)
       // glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        //glm::mat4 projection = glm::ortho(-2.0f, +2.0f, -1.5f, +1.5f, 0.1f, 100.0f);

        glm::mat4 projection = myPerspective(
            glm::radians(camera.Zoom),         // fov: field of view in radians
            (float)SCR_WIDTH / (float)SCR_HEIGHT, // aspect: aspect ratio
            0.1f,                              // near: near clipping plane
            100.0f                             // far: far clipping plane
        );

        lightingShader.setMat4("projection", projection);

        // camera/view transformation
        glm::mat4 view = camera.GetViewMatrix();
        //glm::mat4 view = basic_camera.createViewMatrix();
        lightingShader.setMat4("view", view);

        // Modelling Transformation
        glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
        glm::mat4 translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model;
        translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        model = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;
        lightingShader.setMat4("model", model);

        //glBindVertexArray(cubeVAO);
        //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        //glDrawArrays(GL_TRIANGLES, 0, 36);
        //sphere
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-3.0f, 1.55f, 9.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        sphere.drawSphere(lightingShader, model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-2.0f, 1.55f, 9.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        sphere.drawSphere(lightingShader, model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 1.55f, 9.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        sphere.drawSphere(lightingShader, model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 1.55f, 9.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        sphere.drawSphere(lightingShader, model);

        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-4.0f, 1.55f, 9.0f));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f, 0.4f));
        sphere.drawSphere(lightingShader, model);

        bed(cubeVAO, lightingShader, model);
        //draw floor
        floor(cubeVAO, lightingShader);
        //axis(cubeVAO, lightingShader);
        frontWall(cubeVAO, lightingShader, cylinder, hemisphere, cone, cutcone);
        rightWall(cubeVAO, lightingShader, cutcone, cylinder);

        /*glm::mat4 sphereTranslate = glm::translate(identityMatrix, glm::vec3(-13.0f, 4.4f, -4.3f)); // Slightly above the table
        glm::mat4 sphereScale = glm::scale(identityMatrix, glm::vec3(0.5f, 0.5f, 0.5f));          // Adjust sphere size
        glm::mat4 sphereModel = sphereTranslate * sphereScale;
        drawSphere(lightingShader, sphereModel, 0.8f, 0.3f, 0.3f);*/
        // also draw the lamp object(s)
        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 2; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
            cone.drawCone(lightingShader, model);
            //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS)
        {
            ambienton_off(lightingShader);
        }
        if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS)
        {
            diffuse_on_off(lightingShader);
        }
        if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS)
        {
            specular_on_off(lightingShader);
        }

        // be sure to activate shader when setting uniforms/drawing objects
        lightingShaderWithTexture.use();
        lightingShaderWithTexture.setVec3("viewPos", camera.Position);
        lightingShaderWithTexture.setMat4("projection", projection);
        lightingShaderWithTexture.setMat4("view", view);

        // point light 1
        pointlight1.setUpPointLight(lightingShaderWithTexture);
        // point light 2
        pointlight2.setUpPointLight(lightingShaderWithTexture);

        glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);

        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(7.0f, 1.3f, 8.4f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(2.7f, 2.4f, 0.2f));
        cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        //sphere with tex
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(11.0f, 1.25f, 2.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5f, 0.5f, 0.5f));
        //cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        spheretex.drawSphere(lightingShaderWithTexture, laughEmoji, modelMatrixForContainer);
        //cylindertex.drawCylinder(lightingShaderWithTexture, laughEmoji, modelMatrixForContainer);
        //conetex.drawCone(lightingShaderWithTexture, laughEmoji, modelMatrixForContainer);



        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightCubeVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}
void drawSphere(Shader& shader, glm::mat4 model, float r, float g, float b) {
    shader.use();

    shader.setVec3("material.ambient", glm::vec3(r, g, b));
    shader.setVec3("material.diffuse", glm::vec3(r, g, b));
    shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setFloat("material.shininess", 32.0f);

    shader.setMat4("model", model);

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLES, sphereIndices.size(), GL_UNSIGNED_INT, 0);
}

void drawTorus(Shader& shader, glm::mat4 model, float r, float g, float b) {
    shader.use();
    shader.setVec3("material.ambient", glm::vec3(r, g, b));
    shader.setVec3("material.diffuse", glm::vec3(r, g, b));
    shader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    shader.setFloat("material.shininess", 32.0f);

    shader.setMat4("model", model);
    glBindVertexArray(torusVAO);
    glDrawElements(GL_TRIANGLES, torusIndices.size(), GL_UNSIGNED_INT, 0);
}

void createTorus(float innerRadius, float outerRadius, int sides, int rings,
    std::vector<float>& vertices, std::vector<unsigned int>& indices) {
    const float P = 3.1415926f;
    float ringStep = 2 * P / rings;
    float sideStep = 2 * P / sides;

    for (int i = 0; i <= rings; ++i) {
        float ringAngle = i * ringStep;
        float cosRing = cos(ringAngle);
        float sinRing = sin(ringAngle);

        for (int j = 0; j <= sides; ++j) {
            float sideAngle = j * sideStep;
            float cosSide = cos(sideAngle);
            float sinSide = sin(sideAngle);

            // Vertex position
            float x = (outerRadius + innerRadius * cosSide) * cosRing;
            float y = (outerRadius + innerRadius * cosSide) * sinRing;
            float z = innerRadius * sinSide;
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal vector
            vertices.push_back(cosSide * cosRing);
            vertices.push_back(cosSide * sinRing);
            vertices.push_back(sinSide);
        }
    }

    for (int i = 0; i < rings; ++i) {
        for (int j = 0; j < sides; ++j) {
            int first = i * (sides + 1) + j;
            int second = first + sides + 1;

            indices.push_back(first);
            indices.push_back(second);
            indices.push_back(first + 1);

            indices.push_back(second);
            indices.push_back(second + 1);
            indices.push_back(first + 1);
        }
    }
}


void drawCube(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 model = glm::mat4(1.0f), float r = 1.0f, float g = 1.0f, float b = 1.0f, float shininess = 32.0f)
{
    lightingShader.use();

    lightingShader.setVec3("material.ambient", glm::vec3(r, g, b));
    lightingShader.setVec3("material.diffuse", glm::vec3(r, g, b));
    lightingShader.setVec3("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
    lightingShader.setFloat("material.shininess", shininess);

    lightingShader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

void floor(unsigned int& cubeVAO, Shader& lightingShader)
{
    lightingShader.use();
    //base
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    //glm::mat4 translate2 = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    scale = glm::scale(identityMatrix, glm::vec3(100.0, 0.2, 70.0));
    translate = glm::translate(identityMatrix, glm::vec3(-46.0, -1.0, -5.0));
    glm::mat4 model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 1.0, 1.0, 1.0, 32.0);

    /*scale = glm::scale(identityMatrix, glm::vec3(0.1, 2.0, 0.1));
    translate = glm::translate(identityMatrix, glm::vec3(-5.0, -0.8, -5.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.1, 0.1, 0.1, 32.0);

    //washroom wall
    scale = glm::scale(identityMatrix, glm::vec3(-5.0, 5.0, 0.1));
    translate = glm::translate(identityMatrix, glm::vec3(4.9, -0.8, 2.0));
    model = translate * scale;
    //drawCube(cubeVAO, lightingShader, model, 0.5, 0.5, 0.5, 32.0); */
}
void frontWall(unsigned int& cubeVAO, Shader& lightingShader, Cylinder& cylinder, HemiSphere& hemisphere, Cone& cone, CutCone& cutcone)
{
    lightingShader.use();
    //base
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 model = translate * scale;
    //window wall
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 8.0, 70.0));
    translate = glm::translate(identityMatrix, glm::vec3(50.9, -0.8, -5.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.565f, 0.933f, 0.565f, 32.0);
    //back wall
    scale = glm::scale(identityMatrix, glm::vec3(62.0, 8.0, 0.1));
    translate = glm::translate(identityMatrix, glm::vec3(-10.0, -0.8, 65.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.565f, 0.933f, 0.565f, 32.0);

    //cash counter wall
    scale = glm::scale(identityMatrix, glm::vec3(30.0, 8.0, 0.1));
    translate = glm::translate(identityMatrix, glm::vec3(1.0, -0.8, 40.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.933f, 0.510f, 0.933f, 32.0);
    //right side wall
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 8.0, 15.0));
    translate = glm::translate(identityMatrix, glm::vec3(1.0, -0.8, 40.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.933f, 0.510f, 0.933f, 32.0);
    //left side wall
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 8.0, 15.0));
    translate = glm::translate(identityMatrix, glm::vec3(31.0, -0.8, 40.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.933f, 0.510f, 0.933f, 32.0);

    scale = glm::scale(identityMatrix, glm::vec3(25.0, 5.0, 4.0));
    translate = glm::translate(identityMatrix, glm::vec3(20, -0.8, 30.0));
    model = translate * scale;
    cutcone.drawCutCone(lightingShader, model);
    

    //washroom wall
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 5.0,10.1));
    translate = glm::translate(identityMatrix, glm::vec3(-23, -0.8, 37.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.565f, 0.933f, 0.565f, 32.0);
    //mirror
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 1.0, 1.0));
    translate = glm::translate(identityMatrix, glm::vec3(-28.0, 2.5, 34.0));
    model = translate * scale;
    cylinder.drawCylinder(lightingShader, model);
    //basin
    scale = glm::scale(identityMatrix, glm::vec3(1.2, 1.2, 1.2));
    translate = glm::translate(identityMatrix, glm::vec3(-27.0, 1.6, 34.0));
    model = translate * scale;
    hemisphere.drawHemiSphere(lightingShader, model);




    //round table 1
    glm::mat4 modelForCylinder = glm::mat4(1.0f);
    modelForCylinder = glm::translate(modelForCylinder, glm::vec3(46.0f, 1.2f, 1.0f));
    modelForCylinder = glm::rotate(modelForCylinder, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder = glm::scale(modelForCylinder, glm::vec3(0.05f, 2.0f, 2.0f));
    cylinder.drawCylinder(lightingShader, modelForCylinder);

    //table stand 1
    glm::mat4 stand1 = glm::mat4(1.0f);
    stand1 = glm::translate(stand1, glm::vec3(46.0f, 0.2f, 1.0f));
    stand1 = glm::rotate(stand1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stand1 = glm::scale(stand1, glm::vec3(1.0f, 0.2f,0.2f));
    cylinder.drawCylinder(lightingShader, stand1);
    //lower circle
    glm::mat4 circle1 = glm::mat4(1.0f);
    circle1 = glm::translate(circle1, glm::vec3(46.0f, -0.8f, 1.0f));
    circle1 = glm::rotate(circle1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    circle1 = glm::scale(circle1, glm::vec3(0.05f, 1.0f, 1.0f));
    cylinder.drawCylinder(lightingShader, circle1);

    //round table 2
    glm::mat4 modelForCylinder2 = glm::mat4(1.0f);
    modelForCylinder2 = glm::translate(modelForCylinder2, glm::vec3(46.0f, 1.2f, 12.0f));
    modelForCylinder2 = glm::rotate(modelForCylinder2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder2 = glm::scale(modelForCylinder2, glm::vec3(0.05f, 2.0f, 2.0f));
    cylinder.drawCylinder(lightingShader, modelForCylinder2);

    //table stand 2
    glm::mat4 stand2 = glm::mat4(1.0f);
    stand2 = glm::translate(stand2, glm::vec3(46.0f, 0.2f, 12.0f));
    stand2 = glm::rotate(stand2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stand2 = glm::scale(stand2, glm::vec3(1.0f, 0.2f, 0.2f));
    cylinder.drawCylinder(lightingShader, stand2);
    //lower circle
    glm::mat4 circle2 = glm::mat4(1.0f);
    circle2 = glm::translate(circle2, glm::vec3(46.0f, -0.8f, 12.0f));
    circle2 = glm::rotate(circle2, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    circle2 = glm::scale(circle2, glm::vec3(0.05f, 1.0f, 1.0f));
    cylinder.drawCylinder(lightingShader, circle2);

    glm::mat4 modelForCylinder3 = glm::mat4(1.0f);
    modelForCylinder3 = glm::translate(modelForCylinder3, glm::vec3(46.0f, 1.2f, 23.0f));
    modelForCylinder3 = glm::rotate(modelForCylinder3, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder3 = glm::scale(modelForCylinder3, glm::vec3(0.05f, 2.0f, 2.0f));
    cylinder.drawCylinder(lightingShader, modelForCylinder3);

    //table stand 3
    glm::mat4 stand3 = glm::mat4(1.0f);
    stand3 = glm::translate(stand3, glm::vec3(46.0f, 0.2f, 23.0f));
    stand3 = glm::rotate(stand3, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    stand3 = glm::scale(stand3, glm::vec3(1.0f, 0.2f, 0.2f));
    cylinder.drawCylinder(lightingShader, stand3);
    //lower circle
    glm::mat4 circle3 = glm::mat4(1.0f);
    circle3 = glm::translate(circle3, glm::vec3(46.0f, -0.8f, 23.0f));
    circle3 = glm::rotate(circle3, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    circle3 = glm::scale(circle3, glm::vec3(0.05f, 1.0f, 1.0f));
    cylinder.drawCylinder(lightingShader, circle3);
    //round table 4
    glm::mat4 modelForCylinder4 = glm::mat4(1.0f);
    modelForCylinder4 = glm::translate(modelForCylinder4, glm::vec3(-25.0f, 1.0f, 15.0f));
    modelForCylinder4 = glm::rotate(modelForCylinder4, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder4 = glm::scale(modelForCylinder4, glm::vec3(0.05f, 2.0f, 2.0f));
    cylinder.drawCylinder(lightingShader, modelForCylinder4);
    //round table 5
    glm::mat4 modelForCylinder5 = glm::mat4(1.0f);
    modelForCylinder5 = glm::translate(modelForCylinder5, glm::vec3(-32.0f, 1.0f, 4.0f));
    modelForCylinder5 = glm::rotate(modelForCylinder5, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder5 = glm::scale(modelForCylinder5, glm::vec3(0.05f, 2.0f, 2.0f));
    cylinder.drawCylinder(lightingShader, modelForCylinder5);
    //round table 6
    glm::mat4 modelForCylinder6 = glm::mat4(1.0f);
    modelForCylinder6 = glm::translate(modelForCylinder6, glm::vec3(-22.0f, 1.0f, 2.0f));
    modelForCylinder6 = glm::rotate(modelForCylinder6, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder6 = glm::scale(modelForCylinder6, glm::vec3(0.05f, 2.0f, 2.0f));
    cylinder.drawCylinder(lightingShader, modelForCylinder6);
    //book shelf
    //base 1
    scale = glm::scale(identityMatrix, glm::vec3(5.0, 0.2, 16.0));
    translate = glm::translate(identityMatrix, glm::vec3(-40.0, -0.8, 14.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //base 2
    scale = glm::scale(identityMatrix, glm::vec3(5.0, 0.2, 16.0));
    translate = glm::translate(identityMatrix, glm::vec3(-40.0, 1.8, 14.0));
    model = translate * scale;
    //base 3
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    scale = glm::scale(identityMatrix, glm::vec3(5.0, 0.2, 16.0));
    translate = glm::translate(identityMatrix, glm::vec3(-40.0, 3.8, 14.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //base 4
    scale = glm::scale(identityMatrix, glm::vec3(5.0, 0.2, 16.0));
    translate = glm::translate(identityMatrix, glm::vec3(-40.0, 5.8, 14.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //side
    scale = glm::scale(identityMatrix, glm::vec3(5.0, 6.6, 0.2));
    translate = glm::translate(identityMatrix, glm::vec3(-40.0, -0.6, 14.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //side 2
    scale = glm::scale(identityMatrix, glm::vec3(5.0, 6.7, 0.2));
    translate = glm::translate(identityMatrix, glm::vec3(-40.0, -0.7, 29.8));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //cross
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(10.0f, 2.0f, 1.0f));
    model = glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.2f, 10.0f, 0.2f));
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(5.0f, 2.0f, 1.0f));
    model = glm::rotate(model, glm::radians(-30.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.2f, 10.0f, 0.2f));
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //table
    scale = glm::scale(identityMatrix, glm::vec3(2.0, 1.2, 2.0));
    translate = glm::translate(identityMatrix, glm::vec3(-7.0, -0.8, 14.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

   

    //main table
    //table board
    scale = glm::scale(identityMatrix, glm::vec3(8.0f, 1.4f, 3.5f));
    translate = glm::translate(identityMatrix, glm::vec3(18.0, -0.8, 5.2));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //sofa 1
    scale = glm::scale(identityMatrix, glm::vec3(4.0f, 1.2f, 4.0f));
    translate = glm::translate(identityMatrix, glm::vec3(20.0, 0.2, 11.2));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //sofa back 1
    scale = glm::scale(identityMatrix, glm::vec3(4.0f, 2.2f, 1.0f));
    translate = glm::translate(identityMatrix, glm::vec3(20.0, 1.4, 14.2));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //sofa leg 1
    scale = glm::scale(identityMatrix, glm::vec3(0.5f, 1.0f, 0.5f));
    translate = glm::translate(identityMatrix, glm::vec3(20.0, -0.8, 11.2));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //sofa 2
    scale = glm::scale(identityMatrix, glm::vec3(4.0f, 1.2f, 4.0f));
    translate = glm::translate(identityMatrix, glm::vec3(30.0, 0.2, 5.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //sofa 3
    scale = glm::scale(identityMatrix, glm::vec3(4.0f, 1.2f, 4.0f));
    translate = glm::translate(identityMatrix, glm::vec3(20.0, 0.2, -0.3));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //sofa 4
    scale = glm::scale(identityMatrix, glm::vec3(4.0f, 1.2f, 4.0f));
    translate = glm::translate(identityMatrix, glm::vec3(11.0, 0.2, 5.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //table leg 1
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.5f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(-2.0, 0.6, -2.2));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //table leg 2
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.5f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(0.8, 0.6, -2.2));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //table leg 3
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.5f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(0.8, 0.6, 2.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //table leg 4
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.5f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(-2.0, 0.6, 2.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //chair
    scale = glm::scale(identityMatrix, glm::vec3(1.2f, 0.2f, 1.2f));
    translate = glm::translate(identityMatrix, glm::vec3(1.2, 0.4, 1.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //leg 1
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.2f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(1.2, 0.4, 1.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //leg 2
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.2f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 0.4, 1.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //leg 3
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.2f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 0.4, 2.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //leg 4
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.2f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(1.2, 0.4, 2.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //chair upper vertical 1
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.4f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 2.0, 2.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    // chari upper vertical 2
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.4f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 2.0, 1.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //chair horizontal part 1
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 1.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 2.0, 1.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //chair horizontal part 2
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 1.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 1.2, 1.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //second chair
    scale = glm::scale(identityMatrix, glm::vec3(1.2f, 0.2f, 1.2f));
    translate = glm::translate(identityMatrix, glm::vec3(1.2, 0.4, -1.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //leg 1
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.2f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(1.2, 0.4, -1.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //leg 2
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.2f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 0.4, -0.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //leg 3
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.2f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 0.4, -1.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //leg 4
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.2f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(1.2, 0.4, -0.8));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //chair upper vertical 1
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.4f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 2.0, -0.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    // chari upper vertical 2
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, -1.4f, 0.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 2.0, -1.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //chair horizontal part 1
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 1.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 2.0, -1.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    //chair horizontal part 2
    scale = glm::scale(identityMatrix, glm::vec3(0.2f, 0.2f, 1.2f));
    translate = glm::translate(identityMatrix, glm::vec3(2.2, 1.2, -1.6));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);




    //lamp holder
    scale = glm::scale(identityMatrix, glm::vec3(0.6, 0.05, 0.05));
    translate = glm::translate(identityMatrix, glm::vec3(11.4f, 2.9f, -0.52f));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.0, 0.0, 0.0, 32.0);

    scale = glm::scale(identityMatrix, glm::vec3(0.05, 0.05, 0.6));
    translate = glm::translate(identityMatrix, glm::vec3(4.48f, 2.9f, 8.42f));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.0, 0.0, 0.0, 32.0);

}
void rightWall(unsigned int& cubeVAO, Shader& lightingShader, CutCone& cutcone, Cylinder& cylinder)
{
    lightingShader.use();
    //base
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    glm::mat4 rotation = glm::mat4(1.0f);
    glm::mat4 model = translate * scale;
    

    //commode
    scale = glm::scale(identityMatrix, glm::vec3(1.0, 1.0, 1.5));
    translate = glm::translate(identityMatrix, glm::vec3(-30, -0.8, 28.5));
    model = translate * scale;
    cutcone.drawCutCone(lightingShader, model);
    //commode
    scale = glm::scale(identityMatrix, glm::vec3(1.0, 0.1, 1.5));
    translate = glm::translate(identityMatrix, glm::vec3(-30, -0.1, 28.5));
    model = translate * scale;
    cutcone.drawCutCone(lightingShader, model);

    scale = glm::scale(identityMatrix, glm::vec3(1.7, 1.5, 0.7));
    translate = glm::translate(identityMatrix, glm::vec3(-30.85, -0.1, 29.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.122, 0.361, 0.357, 32.0);

    //choto stick
    scale = glm::scale(identityMatrix, glm::vec3(0.2, 1.97, 0.2));
    translate = glm::translate(identityMatrix, glm::vec3(-0.1, -0.8, 8.9));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.122, 0.361, 0.357, 32.0);

    scale = glm::scale(identityMatrix, glm::vec3(0.2, 1.97, 0.2));
    translate = glm::translate(identityMatrix, glm::vec3(-1.1, -0.8, 8.9));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.122, 0.361, 0.357, 32.0);

    scale = glm::scale(identityMatrix, glm::vec3(0.2, 1.97, 0.2));
    translate = glm::translate(identityMatrix, glm::vec3(-2.1, -0.8, 8.9));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.122, 0.361, 0.357, 32.0);

    scale = glm::scale(identityMatrix, glm::vec3(0.2, 1.97, 0.2));
    translate = glm::translate(identityMatrix, glm::vec3(-3.1, -0.8, 8.9));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.122, 0.361, 0.357, 32.0);

    scale = glm::scale(identityMatrix, glm::vec3(0.2, 1.97, 0.2));
    translate = glm::translate(identityMatrix, glm::vec3(-4.1, -0.8, 8.9));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.122, 0.361, 0.357, 32.0);

    //long table
    scale = glm::scale(identityMatrix, glm::vec3(5.0, 0.5, 28.0));
    translate = glm::translate(identityMatrix, glm::vec3(46.0,2.0, 35.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.788f, 0.604f, 0.145f, 32.0);


    //cross
    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(50.0f, -0.9f, 35.0f));
    model = glm::rotate(model, glm::radians(48.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.2f, 4.8f, 0.2f));
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    model = glm::mat4(1.0f);
    model = glm::translate(model, glm::vec3(46.0f, -1.0f, 35.0f));
    model = glm::rotate(model, glm::radians(-48.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    model = glm::scale(model, glm::vec3(0.2f,5.0f, 0.2f));
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);

    //long table chairs
    glm::mat4 modelForCylinder = glm::mat4(1.0f);
    modelForCylinder = glm::translate(modelForCylinder, glm::vec3(42.0f, 1.6f, 37.0f));
    modelForCylinder = glm::rotate(modelForCylinder, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder = glm::scale(modelForCylinder, glm::vec3(0.05f, 1.4f, 1.4f));
    cylinder.drawCylinder(lightingShader, modelForCylinder);
    //legs
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 3.0, 0.1));
    translate = glm::translate(identityMatrix, glm::vec3(40.0, -0.8, 35.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.788f, 0.604f, 0.145f, 32.0);


    //chair 2
    modelForCylinder = glm::mat4(1.0f);
    modelForCylinder = glm::translate(modelForCylinder, glm::vec3(42.0f, 1.6f, 44.0f));
    modelForCylinder = glm::rotate(modelForCylinder, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder = glm::scale(modelForCylinder, glm::vec3(0.05f, 1.4f, 1.4f));
    cylinder.drawCylinder(lightingShader, modelForCylinder);
    //chair 3
     modelForCylinder = glm::mat4(1.0f);
    modelForCylinder = glm::translate(modelForCylinder, glm::vec3(42.0f, 1.6f, 51.0f));
    modelForCylinder = glm::rotate(modelForCylinder, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder = glm::scale(modelForCylinder, glm::vec3(0.05f, 1.4f, 1.4f));
    cylinder.drawCylinder(lightingShader, modelForCylinder);


    //chair 3
    modelForCylinder = glm::mat4(1.0f);
    modelForCylinder = glm::translate(modelForCylinder, glm::vec3(42.0f, 1.6f, 57.0f));
    modelForCylinder = glm::rotate(modelForCylinder, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    modelForCylinder = glm::scale(modelForCylinder, glm::vec3(0.05f, 1.4f, 1.4f));
    cylinder.drawCylinder(lightingShader, modelForCylinder);

}

void axis(unsigned int& cubeVAO, Shader& lightingShader)
{
    lightingShader.use();
    //base
    glm::mat4 identityMatrix = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    //glm::mat4 translate2 = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    //x
    scale = glm::scale(identityMatrix, glm::vec3(2.0, 0.1, 0.1));
    translate = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.0));
    glm::mat4 model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 1.0, 0.0, 0.0, 32.0);
    //y
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 2.0, 0.1));
    translate = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.0, 1.0, 0.0, 32.0);
    //z
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 0.1, 2.0));
    translate = glm::translate(identityMatrix, glm::vec3(0.0, 0.0, 0.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.0, 0.0, 1.0, 32.0);
}

void bed(unsigned int& cubeVAO, Shader& lightingShader, glm::mat4 alTogether)
{
    float baseHeight = 0.3;
    float width = 1;
    float length = 2;
    float pillowWidth = 0.3;
    float pillowLength = 0.15;
    float blanketWidth = 0.8;
    float blanketLength = 0.7;
    float headHeight = 0.6;

    //base
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 translate = glm::mat4(1.0f);
    glm::mat4 translate2 = glm::mat4(1.0f);
    glm::mat4 scale = glm::mat4(1.0f);
    scale = glm::scale(model, glm::vec3(width, baseHeight, length));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 0.545, 0.271, 0.075, 32.0);

    //foam
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, baseHeight, 0));
    scale = glm::scale(model, glm::vec3(width, 0.06, length));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 0.804, 0.361, 0.361, 32.0);

    //pillow 1
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3((width / 2) - (0.1 + pillowWidth / 2), baseHeight + 1 * 0.06, (length / 2) - (0.025 + pillowWidth / 2)));
    scale = glm::scale(model, glm::vec3(pillowWidth, 0.04, pillowLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 1, 0.647, 0, 32.0);

    //pillow 2
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3((-width / 2) + (0.1 + pillowWidth / 2), baseHeight + 1 * 0.06, (length / 2) - (0.025 + pillowWidth / 2)));
    scale = glm::scale(model, glm::vec3(pillowWidth, 0.04, pillowLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 1, 0.647, 0, 32.0);

    //blanket
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, baseHeight + 1 * 0.06, -(length / 2 - 0.025) + blanketLength / 2));
    scale = glm::scale(model, glm::vec3(blanketWidth, 0.015, blanketLength));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 0.541, 0.169, 0.886, 32.0);

    //head
    model = glm::mat4(1.0f);
    translate = glm::mat4(1.0f);
    translate2 = glm::mat4(1.0f);
    scale = glm::mat4(1.0f);
    translate2 = glm::translate(model, glm::vec3(0, 0, (length / 2 - 0.02 / 2) + 0.02));
    scale = glm::scale(model, glm::vec3(width, headHeight, 0.02));
    translate = glm::translate(model, glm::vec3(-0.5, 0, -0.5));
    model = alTogether * translate2 * scale * translate;
    drawCube(cubeVAO, lightingShader, model, 0.545, 0.271, 0.075, 32.0);

}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        camera.ProcessKeyboard(FORWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= 0.1;
        else if (rotateAxis_Y) rotateAngle_Y -= 0.1;
        else rotateAngle_Z -= 0.1;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        camera.ProcessKeyboard(UP, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_LEFT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        camera.ProcessKeyboard(Y_RIGHT, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_LEFT, deltaTime);

    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        camera.ProcessKeyboard(R_RIGHT, deltaTime);

    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_UP, deltaTime);

    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        camera.ProcessKeyboard(P_DOWN, deltaTime);
    }
    //if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.01;
    //if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.01;
    //if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.01;
    //if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.01;
    //if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.01;
    //if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.01;
    //if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.001;
    //if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.001;
    //if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.001;
    //if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.001;
    //if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.001;
    //if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.001;

    //if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    //{
    //    rotateAngle_X += 0.1;
    //    rotateAxis_X = 1.0;
    //    rotateAxis_Y = 0.0;
    //    rotateAxis_Z = 0.0;
    //}
    //if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    //{
    //    rotateAngle_Y += 0.1;
    //    rotateAxis_X = 0.0;
    //    rotateAxis_Y = 1.0;
    //    rotateAxis_Z = 0.0;
    //}
    //if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    //{
    //    rotateAngle_Z += 0.1;
    //    rotateAxis_X = 0.0;
    //    rotateAxis_Y = 0.0;
    //    rotateAxis_Z = 1.0;
    //}

    //if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS)
    //{
    //    eyeX += 2.5 * deltaTime;
    //    basic_camera.changeEye(eyeX, eyeY, eyeZ);
    //}
    //if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
    //{
    //    eyeX -= 2.5 * deltaTime;
    //    basic_camera.changeEye(eyeX, eyeY, eyeZ);
    //}
    //if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS)
    //{
    //    eyeZ += 2.5 * deltaTime;
    //    basic_camera.changeEye(eyeX, eyeY, eyeZ);
    //}
    //if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
    //{
    //    eyeZ -= 2.5 * deltaTime;
    //    basic_camera.changeEye(eyeX, eyeY, eyeZ);
    //}
    //if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    //{
    //    eyeY += 2.5 * deltaTime;
    //    basic_camera.changeEye(eyeX, eyeY, eyeZ);
    //}
    //if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
    //{
    //    eyeY -= 2.5 * deltaTime;
    //    basic_camera.changeEye(eyeX, eyeY, eyeZ);
    //}

}
void ambienton_off(Shader& lightingShader)
{
    double currentTime = glfwGetTime();
    if (currentTime - lastKeyPressTime < keyPressDelay) return;
    lightingShader.use();
    if (AmbientON)
    {
        pointlight1.turnAmbientOff();
        pointlight2.turnAmbientOff();
        lightingShader.setVec3("directionalLight.ambient", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        AmbientON = !AmbientON;
        lastKeyPressTime = currentTime;
    }
    else
    {
        pointlight1.turnAmbientOn();
        pointlight2.turnAmbientOn();
        lightingShader.setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
        lightingShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
        AmbientON = !AmbientON;
        lastKeyPressTime = currentTime;
    }
}
void diffuse_on_off(Shader& lightingShader)
{
    double currentTime = glfwGetTime();
    if (currentTime - lastKeyPressTime < keyPressDelay) return;
    lightingShader.use();
    if (DiffusionON)
    {
        pointlight1.turnDiffuseOff();
        pointlight2.turnDiffuseOff();
        lightingShader.setVec3("directionalLight.diffuse", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        DiffusionON = !DiffusionON;
        lastKeyPressTime = currentTime;
    }
    else
    {
        pointlight1.turnDiffuseOn();
        pointlight2.turnDiffuseOn();
        lightingShader.setVec3("directionalLight.diffuse", 0.8f, 0.8f, 0.8f);
        lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
        DiffusionON = !DiffusionON;
        lastKeyPressTime = currentTime;
    }
}
void specular_on_off(Shader& lightingShader)
{
    double currentTime = glfwGetTime();
    if (currentTime - lastKeyPressTime < keyPressDelay) return;
    lightingShader.use();
    if (SpecularON)
    {
        pointlight1.turnSpecularOff();
        pointlight2.turnSpecularOff();
        lightingShader.setVec3("directionalLight.specular", 0.0f, 0.0f, 0.0f);
        lightingShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        SpecularON = !SpecularON;
        lastKeyPressTime = currentTime;
    }
    else
    {
        pointlight1.turnSpecularOn();
        pointlight2.turnSpecularOn();
        lightingShader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);
        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        SpecularON = !SpecularON;
        lastKeyPressTime = currentTime;
    }
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    //{
    //    if (pointLightOn)
    //    {
    //        pointlight1.turnOff();
    //        pointlight2.turnOff();
    //        pointlight3.turnOff();
    //        pointlight4.turnOff();
    //        pointLightOn = !pointLightOn;
    //    }
    //    else
    //    {
    //        pointlight1.turnOn();
    //        pointlight2.turnOn();
    //        pointlight3.turnOn();
    //        pointlight4.turnOn();
    //        pointLightOn = !pointLightOn;
    //    }
    //}

    if (key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        if (pointLightOn)
        {
            pointlight1.turnOff();
            pointlight2.turnOff();
            //pointlight3.turnOff();
            //pointlight4.turnOff();
            pointLightOn = !pointLightOn;
        }
        else
        {
            pointlight1.turnOn();
            pointlight2.turnOn();
            //pointlight3.turnOn();
            //pointlight4.turnOn();
            pointLightOn = !pointLightOn;
        }
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (directionalLightOn)
        {
            directionalLightOn = !directionalLightOn;
        }
        else
        {
            directionalLightOn = !directionalLightOn;
        }
    }
    if (key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        if (SpotLightOn)
        {
            SpotLightOn = !SpotLightOn;
        }
        else
        {
            SpotLightOn = !SpotLightOn;
        }
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, textureWrappingModeS);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, textureWrappingModeT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, textureFilteringModeMin);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, textureFilteringModeMax);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

