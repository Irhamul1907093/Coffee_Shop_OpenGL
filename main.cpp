#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "basic_camera.h"
#include "BezierCurve.h"
#include "pointLight.h"
#include "SpotLight.h"
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
//void ambienton_off(Shader& lightingShader);
//void diffuse_on_off(Shader& lightingShader);
//void specular_on_off(Shader& lightingShader);
unsigned int loadTexture(char const* path, GLenum textureWrappingModeS, GLenum textureWrappingModeT, GLenum textureFilteringModeMin, GLenum textureFilteringModeMax);
void Glass_window_2(unsigned int TransparentVAO, Shader transparentShader, float x_shift, float y_shift, float z_shift, float rot,float angle, float length, float H);
void GenSheet_Transparent(unsigned int VAO, Shader ourShader, float len, float width, float height, float local_x, float local_y, float local_z, float rot_x, float rot_y, float rot_z, float trans_x, float trans_y, float trans_z, float x_shift, float y_shift, float z_shift, glm::vec4 color, unsigned int texture,float angle);
void load_texture(unsigned int& texture, string image_name, GLenum format);

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

bool isFreezeOpen = false;
bool isOpening = true;
float freezeThreshold = 95.0f;
float freezeStep = 1.0f;
float FreezeAngle = 0.0f;

float keyCooldown = 0.2f;

bool isLiftDoorOpenDown = false;
float liftDoorPositionDown = 0.0f;

// settings
const unsigned int SCR_WIDTH = 1500;
const unsigned int SCR_HEIGHT = 800;

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
float moveZ = 0.0;
// camera
//Camera camera(glm::vec3(0.0f, 1.1f, -5.2f));
Camera camera(glm::vec3(10.0f, 14.1f, -62.2f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

float eyeX = 0.0, eyeY = 1.0, eyeZ = 3.0;
float lookAtX = 0.0, lookAtY = 0.0, lookAtZ = 0.0;
glm::vec3 V = glm::vec3(0.0f, 1.0f, 0.0f);
BasicCamera basic_camera(eyeX, eyeY, eyeZ, lookAtX, lookAtY, lookAtZ, V);


// positions of the point lights
glm::vec3 pointLightPositions[] = {
    glm::vec3(6.0f, 6.4f, 10.7f),
    glm::vec3(6.0f, 6.4f, 26.7f),
    //left round table 3
    glm::vec3(46.0f, 6.4f, 0.7f),
    glm::vec3(46.0f, 6.4f, 11.7f),
    glm::vec3(46.0f, 6.4f, 22.7f),
    //left study
    glm::vec3(46.0f, 6.4f, 33.7f),
    glm::vec3(46.0f, 6.4f, 44.7f),
    glm::vec3(46.0f, 6.4f, 55.7f),
    //mid study 
    glm::vec3(36.0f, 6.4f, 55.7f),
    glm::vec3(22.0f, 6.4f, 10.7f), //drawing sofa
    //right study
    glm::vec3(26.0f, 6.4f, 33.7f),
    glm::vec3(26.0f, 6.4f, 44.7f),
    glm::vec3(26.0f, 6.4f, 55.7f),
    //kitchen
    glm::vec3(6.0f, 6.4f, 36.7f),
    glm::vec3(6.0f, 6.4f, 50.7f),
    //kitchen right
    glm::vec3(-13.0f, 6.4f, 36.7f),
    glm::vec3(-13.0f, 6.4f, 50.7f),
    //washroom 
    glm::vec3(-40.0f, 6.4f, 50.7f),
    glm::vec3(-26.0f, 6.4f, 50.7f),
    //bookshelf
    glm::vec3(-15.0f, 6.4f, 16.7f),
    glm::vec3(-26.0f, 6.4f, 20.7f),
    //round table right
    glm::vec3(-36.0f, 6.4f, 4.7f),
    glm::vec3(-16.0f, 6.4f, 4.7f),
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
PointLight pointlight3(

    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    3       // light number
);
PointLight pointlight4(

    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    4       // light number
);
PointLight pointlight5(

    pointLightPositions[4].x, pointLightPositions[4].y, pointLightPositions[4].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    5       // light number
);
PointLight pointlight6(

    pointLightPositions[5].x, pointLightPositions[5].y, pointLightPositions[5].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    6       // light number
);
PointLight pointlight7(

    pointLightPositions[6].x, pointLightPositions[6].y, pointLightPositions[6].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    7       // light number
);
PointLight pointlight8(

    pointLightPositions[7].x, pointLightPositions[7].y, pointLightPositions[7].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    8       // light number
);
PointLight pointlight9(

    pointLightPositions[8].x, pointLightPositions[8].y, pointLightPositions[8].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    9       // light number
);
PointLight pointlight10(

    pointLightPositions[9].x, pointLightPositions[9].y, pointLightPositions[9].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    10       // light number
);
PointLight pointlight11(

    pointLightPositions[10].x, pointLightPositions[10].y, pointLightPositions[10].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    11       // light number
);
PointLight pointlight12(

    pointLightPositions[11].x, pointLightPositions[11].y, pointLightPositions[11].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    12       // light number
);
PointLight pointlight13(

    pointLightPositions[12].x, pointLightPositions[12].y, pointLightPositions[12].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    13       // light number
);
//now more 10
PointLight pointlight14(

    pointLightPositions[13].x, pointLightPositions[13].y, pointLightPositions[13].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    14       // light number
);
PointLight pointlight15(

    pointLightPositions[14].x, pointLightPositions[14].y, pointLightPositions[14].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    15      // light number
);
PointLight pointlight16(

    pointLightPositions[15].x, pointLightPositions[15].y, pointLightPositions[15].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    16       // light number
);
PointLight pointlight17(

    pointLightPositions[16].x, pointLightPositions[16].y, pointLightPositions[16].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    17       // light number
);
PointLight pointlight18(

    pointLightPositions[17].x, pointLightPositions[17].y, pointLightPositions[17].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    18       // light number
);
PointLight pointlight19(

    pointLightPositions[18].x, pointLightPositions[18].y, pointLightPositions[18].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    19       // light number
);
PointLight pointlight20(

    pointLightPositions[19].x, pointLightPositions[19].y, pointLightPositions[19].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    20       // light number
);
PointLight pointlight21(

    pointLightPositions[20].x, pointLightPositions[20].y, pointLightPositions[20].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    21       // light number
);
PointLight pointlight22(

    pointLightPositions[21].x, pointLightPositions[21].y, pointLightPositions[21].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    22       // light number
);
PointLight pointlight23(

    pointLightPositions[22].x, pointLightPositions[22].y, pointLightPositions[22].z,  // position
    0.05f, 0.05f, 0.05f,     // ambient
    0.8f, 0.8f, 0.8f,     // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    23       // light number
);


float spotLightXpos = -1.0f, spotLightYpos = 3.7;
glm::vec3 spotPositions[] = {
    glm::vec3(-1.0f,3.7, 1.0f),
};
SpotLight spotlight1(
    spotPositions[0].x, spotPositions[0].y, spotPositions[0].z,  // position
    0.2f, 0.2f, 0.2f,     // ambient
    0.8f, 0.8f, 0.8f,      // diffuse
    1.0f, 1.0f, 1.0f,        // specular
    1.0f,   //k_c
    0.09f,  //k_l
    0.032f, //k_q
    1,       // light number
    glm::cos(glm::radians(20.5f)),
    glm::cos(glm::radians(25.0f)),
    0, -1, 0
);
//glass
//win_1_1
float rotateAxis_Y_win_2_1 = 0.0;

unsigned int texture0, glass, glass2, hanging_plant, clock_bg, fountain_tex, collapsible_gate;
glm::vec4 off_white = glm::vec4(0.9098039215686274, 0.8549019607843137, 0.8, 1.0f);




// light settings
bool pointLightOn = true;
bool directionLightOn = true;
bool SpotLightOn = true;
//bool AmbientON = true;
//bool DiffusionON = true;
//bool SpecularON = true;
//bool ambientToggle = true;
//bool diffuseToggle = true;
//bool specularToggle = true;

bool onOffToggle = true;

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
    Shader TransparentShader("vertexShaderForBlending.vs", "fragmentShaderForBlending.fs");
    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------

    float transparentVertices[] = {
        // positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
         0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        0.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        1.0f,  0.0f,  0.0f,  1.0f,  1.0f,

        0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  0.0f,  1.0f,  0.0f
    };

    // transparent VAO
    unsigned int transparentVAO, transparentVBO;
    glGenVertexArrays(1, &transparentVAO);
    glGenBuffers(1, &transparentVBO);
    glBindVertexArray(transparentVAO);
    glBindBuffer(GL_ARRAY_BUFFER, transparentVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glBindVertexArray(0);
    
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load("transparent.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture 0" << endl;
    }
    stbi_image_free(data);

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

    //Bezier Curve Control Points
    GLfloat cp_cup[] =
    {
    -0.85, 1.54, 0.0,
    -0.79, 0.965, 0.0,
    -0.605, 0.455, 0.0,
    -0.265, 0.345, 0.0,
    -0.0, 0.34, 0.0

    };





    string laughEmoPath = "football.jpg";
    unsigned int laughEmoji = loadTexture(laughEmoPath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    //string coffeePath = "cf.png";
    //string patrn = "vector.jpg";

    string coffeePath = "cf.png";
    unsigned int coffepic = loadTexture(coffeePath.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cube = Cube(coffepic, coffepic, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string football = "football.jpg";
    unsigned int football_tex = loadTexture(football.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube football_cube = Cube(football_tex, football_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    
    string books = "booktex.jpg";
    unsigned int books_tex = loadTexture(books.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube book_cube = Cube(books_tex, books_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    
    string floors = "floortex5.jpg";
    unsigned int floor_tex = loadTexture(floors.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube floor_cube = Cube(floor_tex, floor_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string longTable = "long_table.jpg";
    unsigned int lombatable_tex = loadTexture(longTable.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube long_table_cube = Cube(lombatable_tex, lombatable_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string wall_one = "wall1.jpg";
    unsigned int wall_one_tex = loadTexture(wall_one.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cash_left_wall = Cube(wall_one_tex, wall_one_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string wall_left = "room_wall.jpg";
    unsigned int wall_two_tex = loadTexture(wall_left.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube room_left_wall = Cube(wall_two_tex, wall_two_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string wal_left = "wall3.jpg";
    unsigned int wall_three_tex = loadTexture(wal_left.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube room_left_wall_inside1 = Cube(wall_three_tex, wall_three_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string wa_one = "wall2.jpg";
    unsigned int wall_four_tex = loadTexture(wa_one.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube room_left_wall_inside2= Cube(wall_four_tex, wall_four_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string wa_two = "wall4.jpg";
    unsigned int wall_fr_tex = loadTexture(wa_two.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube room_mid_wall_study = Cube(wall_fr_tex, wall_four_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //outside
    string stair1 = "stairs.jpg";
    unsigned int stair1ff = loadTexture(stair1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube stair = Cube(stair1ff, stair1ff, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string street1 = "street.jpg";
    unsigned int street1dd = loadTexture(street1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube street = Cube(street1dd, street1dd, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string grass1 = "grass3.jpg";
    unsigned int grass1dd = loadTexture(grass1.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube grass = Cube(grass1dd, grass1dd, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //inside
    string cash_w = "cash_wall4.jpg";
    unsigned int cash_wdd = loadTexture(cash_w.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube cash_wall = Cube(cash_wdd, cash_wdd, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //patterns
    string cute = "cute_color.jpg";
    unsigned int cud = loadTexture(cute.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube pattern4 = Cube(cud, cud, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //ceiling
    string cuze = "3071.jpg";
    unsigned int cuzea = loadTexture(cuze.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube ceiling = Cube(cuzea, cuzea, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //ceil out
    string cuzse = "ceiling_out.jpg";
    unsigned int cuzett = loadTexture(cuzse.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube ceiling2 = Cube(cuzett, cuzett, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);



    string cutesc = "sofa2.jpg";
    unsigned int cdsud = loadTexture(cutesc.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube sofa_seat = Cube(cdsud, cdsud, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //sofa table
    string cuteasc = "pattern3.jpg";
    unsigned int cdsfud = loadTexture(cuteasc.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube sofa_table = Cube(cdsfud, cdsfud, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    //chair tex
    string cdc = "pattern3.jpg";
    unsigned int chai = loadTexture(cdc.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube chair = Cube(chai, chai, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string cdxc = "wood.jpg";
    unsigned int chaiwood = loadTexture(cdxc.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube wood = Cube(chaiwood, chaiwood, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string cdsf = "steel.jpg";
    unsigned int rackwood = loadTexture(cdsf.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube steel = Cube(rackwood, rackwood, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string ciosf = "freeze.png";
    unsigned int rackwood_fr = loadTexture(ciosf.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube freeze = Cube(rackwood_fr, rackwood_fr, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string cposf = "oven.png";
    unsigned int rackwood_ove = loadTexture(cposf.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube oven = Cube(rackwood_ove, rackwood_ove, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //food
    string casc = "food.jpg";
    unsigned int zfud = loadTexture(casc.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube food = Cube(zfud, zfud, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    //menu
    string tata = "menu1.jpg";
    unsigned int ztata = loadTexture(tata.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube menu = Cube(ztata, ztata, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);
    /////////////////////////
    string lamp = "pattern.jpg";
    unsigned int lamp_tex = loadTexture(lamp.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    Cube letters_cube = Cube(lamp_tex, lamp_tex, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    string goltable = "roundtable.jpg";
    unsigned int round_table_tex = loadTexture(goltable.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
   
    string woodStripe = "wood_stripe.jpg";
    unsigned int wood_strip_tex = loadTexture(woodStripe.c_str(), GL_REPEAT, GL_REPEAT, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR);
    
    load_texture(glass, "glass-png-3.png", GL_RGBA);//used
    
    // Cube cube = Cube(coffepic, coffepic, 32.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    SphereTex spheretex = SphereTex();
    CylinderTex cylindertex = CylinderTex();
    ConeTex conetex = ConeTex();
    DiscTex disctex = DiscTex(2);
    
    DiscTex disctex_2 = DiscTex(1);

    Sphere sphere = Sphere();
    Cylinder cylinder = Cylinder();
    Cone cone = Cone();
    CutCone cutcone = CutCone();
    HemiSphere hemisphere = HemiSphere();

    BezierCurve cup = BezierCurve(cp_cup, 5 * 3, lamp_tex);

    TorusTex cup_handle = TorusTex(2, 5.0, 1.0);
    TorusTex letter_O = TorusTex(1, 5.0, 1.0);
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


        //key functionality
        float currentTime_2 = glfwGetTime();
        if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS && currentTime_2 - lastKeyPressTime > keyCooldown) {
            lastKeyPressTime = currentTime_2; // Update the last key press time
            if (!isFreezeOpen) {
                isFreezeOpen = true;         // Activate the door
                isOpening = (FreezeAngle == 0.0f); // Determine if it should open or close
            }
            else {
                isOpening = !isOpening;      // Toggle between opening and closing
            }
        }

        if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS && currentTime_2 - lastKeyPressTime > keyCooldown) {
            lastKeyPressTime = currentTime_2; // Update the last key press time

            isLiftDoorOpenDown = !isLiftDoorOpenDown; // Toggle the swing state
        }


        // be sure to activate shader when setting uniforms/drawing objects
        lightingShader.use();
        lightingShader.setVec3("viewPos", camera.Position);

        // point light 1
        pointlight1.setUpPointLight(lightingShader);
        // point light 2
        pointlight2.setUpPointLight(lightingShader);
        // point light 3
        pointlight3.setUpPointLight(lightingShader);
        // point light 4
        pointlight4.setUpPointLight(lightingShader);
        // point light 5
        pointlight5.setUpPointLight(lightingShader);
        // point light 6
        pointlight6.setUpPointLight(lightingShader);
        // point light 7
        pointlight7.setUpPointLight(lightingShader);
        // point light 8
        pointlight8.setUpPointLight(lightingShader);
        // point light 9
        pointlight9.setUpPointLight(lightingShader);
        // point light 10
        pointlight10.setUpPointLight(lightingShader);
        // point light 11
        pointlight11.setUpPointLight(lightingShader);
        // point light 12
        pointlight12.setUpPointLight(lightingShader);
        // point light 13
        pointlight13.setUpPointLight(lightingShader);
        /////////more 10//////////
        // point light 14
        pointlight14.setUpPointLight(lightingShader);
        // point light 15
        pointlight15.setUpPointLight(lightingShader);
        // point light 16
        pointlight16.setUpPointLight(lightingShader);
        // point light 17
        pointlight17.setUpPointLight(lightingShader);
        // point light 18
        pointlight18.setUpPointLight(lightingShader);
        // point light 19
        pointlight19.setUpPointLight(lightingShader);
        // point light 20
        pointlight20.setUpPointLight(lightingShader);
        // point light 21
        pointlight21.setUpPointLight(lightingShader);
        // point light 22
        pointlight22.setUpPointLight(lightingShader);
        // point light 23
        pointlight23.setUpPointLight(lightingShader);

        lightingShader.setVec3("directionalLight.directiaon", 0.5f, -3.0f, -3.0f);
        lightingShader.setVec3("directionalLight.ambient", .5f, .5f, .5f);
        lightingShader.setVec3("directionalLight.diffuse", .8f, .8f, .8f);
        lightingShader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);


        lightingShader.setBool("directionLightOn", directionLightOn);

        spotlight1.setUpspotLight(lightingShader);

        glm::mat4 projection = myPerspective(
            glm::radians(camera.Zoom),         // fov: field of view in radians
            (float)SCR_WIDTH / (float)SCR_HEIGHT, // aspect: aspect ratio
            0.1f,                              // near: near clipping plane
            300.0f                             // far: far clipping plane
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
       
        frontWall(cubeVAO, lightingShader, cylinder, hemisphere, cone, cutcone);
        rightWall(cubeVAO, lightingShader, cutcone, cylinder);

        ourShader.use();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 23 ; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.6f)); // Make it a smaller cube
            ourShader.setMat4("model", model);
            ourShader.setVec3("color", glm::vec3(0.8f, 0.8f, 0.8f));
            //cone.drawCone(lightingShader, model);
            conetex.drawCone(lightingShaderWithTexture, lamp_tex, model);
            //glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
            //glDrawArrays(GL_TRIANGLES, 0, 36);

            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(pointLightPositions[i].x - 0.05, pointLightPositions[i].y + 1.0f, pointLightPositions[i].z - 0.05));
            model = glm::scale(model, glm::vec3(0.1f,1.4f,0.1f)); // Make it a smaller cube
            chair.drawCubeWithTexture(lightingShaderWithTexture, model);
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
        // point light 3
        pointlight3.setUpPointLight(lightingShaderWithTexture);
        // point light 4
        pointlight4.setUpPointLight(lightingShaderWithTexture);
        // point light 5
        pointlight5.setUpPointLight(lightingShaderWithTexture);
        // point light 6
        pointlight6.setUpPointLight(lightingShaderWithTexture);
        // point light 7
        pointlight7.setUpPointLight(lightingShaderWithTexture);
        // point light 8
        pointlight8.setUpPointLight(lightingShaderWithTexture);
        // point light 9
        pointlight9.setUpPointLight(lightingShaderWithTexture);
        // point light 10
        pointlight10.setUpPointLight(lightingShaderWithTexture);
        // point light 11
        pointlight11.setUpPointLight(lightingShaderWithTexture);
        // point light 12
        pointlight12.setUpPointLight(lightingShaderWithTexture);
        // point light 13
        pointlight13.setUpPointLight(lightingShaderWithTexture);
        /////////more 10//////////
        // point light 14
        pointlight14.setUpPointLight(lightingShaderWithTexture);
        // point light 15
        pointlight15.setUpPointLight(lightingShaderWithTexture);
        // point light 16
        pointlight16.setUpPointLight(lightingShaderWithTexture);
        // point light 17
        pointlight17.setUpPointLight(lightingShaderWithTexture);
        // point light 18
        pointlight18.setUpPointLight(lightingShaderWithTexture);
        // point light 19
        pointlight19.setUpPointLight(lightingShaderWithTexture);
        // point light 20
        pointlight20.setUpPointLight(lightingShaderWithTexture);
        // point light 21
        pointlight21.setUpPointLight(lightingShaderWithTexture);
        // point light 22
        pointlight22.setUpPointLight(lightingShaderWithTexture);
        // point light 23
        pointlight23.setUpPointLight(lightingShaderWithTexture);

        lightingShaderWithTexture.setVec3("directionalLight.directiaon", 0.5f, -3.0f, -3.0f);
        lightingShaderWithTexture.setVec3("directionalLight.ambient", 0.5f, 0.5f, 0.5f);
        lightingShaderWithTexture.setVec3("directionalLight.diffuse", .8f, .8f, .8f);
        lightingShaderWithTexture.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);


        lightingShaderWithTexture.setBool("directionLightOn", directionLightOn);

        spotlight1.setUpspotLight(lightingShaderWithTexture);
        //lightingShaderWithTexture.use();

        glm::mat4 modelMatrixForContainer = glm::mat4(1.0f);

        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(7.0f, 1.3f, 8.4f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(2.7f, 2.4f, 0.2f));
        football_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        //floor
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-46.0, -1.0, -5.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(100.0, 0.8, 70.0));
        floor_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //ceiling inner
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-46.0, 9.0, -5.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(100.0, 0.4, 70.0));
        ceiling.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //ceiling outer
       modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-46.0, 9.4, -5.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(100.0, 1.0, 70.0));
        ceiling2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //stairs
        int stairCount = 4;
        float stair_y = -3.0, stair_translate_z = -10.0, stair_scale_z = 75.0;
        while (stairCount--)
        {
            modelMatrixForContainer = glm::mat4(1.0f);
            modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-46.0, stair_y, stair_translate_z));
            modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(100.0, 2.0, 75.0));
            stair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            stair_y -= 2.0;
            stair_scale_z += 9.8;
            stair_translate_z -= 3.8;
        }
        //street
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-4.0, -10.0, -60.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(10.0, 1.0, 40.0));
        street.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        int stand = 8;
        float gap = -60.0;
        while (stand--)
        {
            modelMatrixForContainer = glm::mat4(1.0f);
            modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-4.0, -10.0, gap));
            modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.4f, 3.5f, 0.2f));
            chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

            modelMatrixForContainer = glm::mat4(1.0f);
            modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(6.0, -10.0, gap));
            modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.4f, 3.5f, 0.2f));
            chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
            gap += 4.0;
        }
        //stand 1
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(52.0, -10.0, -22.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.8f, 11.0f, 0.4f));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //sphere light 1
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(52.5, 1.0, -22.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 1.0f, 1.0f));
        sphere.drawSphere(lightingShaderWithTexture, modelMatrixForContainer);

        //stand 2
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(52.0, -10.0, -60.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.8f, 11.0f, 0.4f));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //light 2
        //sphere light 2
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(52.5, 1.0, -60.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 1.0f, 1.0f));
        sphere.drawSphere(lightingShaderWithTexture, modelMatrixForContainer);
        
        //stand 3
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-47.0, -10.0, -22.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.8f, 11.0f, 0.4f));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //light 3
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-47.0, 1.0, -22.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 1.0f, 1.0f));
        sphere.drawSphere(lightingShaderWithTexture, modelMatrixForContainer);

        //stand 4
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-47.0, -10.0, -60.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.8f, 11.0f, 0.4f));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        //sphere light 4
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-47.0, 1.0, -60.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 1.0f, 1.0f));
        sphere.drawSphere(lightingShaderWithTexture, modelMatrixForContainer);

        //grass
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(6.0, -10.0, -60.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(48.0, 1.0, 40.0));
        grass.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-47.0, -10.0, -60.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(43.0, 1.0, 40.0));
        grass.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        
        //sphere with tex
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(11.0f, 1.25f, 2.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5f, 0.5f, 0.5f));
        //cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        spheretex.drawSphere(lightingShaderWithTexture, laughEmoji, modelMatrixForContainer);
        //cylindertex.drawCylinder(lightingShaderWithTexture, laughEmoji, modelMatrixForContainer);
        //conetex.drawCone(lightingShaderWithTexture, laughEmoji, modelMatrixForContainer);

        //Bezier Curve with texture
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(40.0f, 10.0f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(8.5f, 8.5f, 8.5f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5f, 0.5f, 0.5f));
        cup.drawBezierCurve(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(42.7f, 14.4f, -5.0f));
        modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(-100.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.6f, 0.6f,0.6f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5f, 0.5f, 0.5f));
        cup_handle.drawTorus(lightingShaderWithTexture, lamp_tex,modelMatrixForContainer);

        glm::mat4 circle1 = glm::mat4(1.0f);
        circle1 = glm::translate(circle1, glm::vec3(40.0f, 11.0f, -5.0f));
        circle1 = glm::rotate(circle1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        circle1 = glm::scale(circle1, glm::vec3(0.4f, 5.0f, 5.0f));
        cylinder.drawCylinder(lightingShader, circle1);
        //C
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(27.7f, 14.4f, -5.0f));
        modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.7f, 0.7f, 0.7f));
        cup_handle.drawTorus(lightingShaderWithTexture, lamp_tex, modelMatrixForContainer);
        //O
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(20.7f, 14.4f, -5.0f));
        modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.7f, 0.7f, 0.7f));
        letter_O.drawTorus(lightingShaderWithTexture, lamp_tex, modelMatrixForContainer);
        //F
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(12.7f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 8.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(9.7f, 17.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(9.7f, 13.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //F - 2
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(5.7f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 8.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(2.7f, 17.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(2.7f, 13.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //E
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-1.3f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 8.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-4.3f, 17.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-4.3f, 13.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-4.3f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //T
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-14.3f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 8.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-16.7f, 17.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(6.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //I
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-22.3f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 8.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-24.7f, 17.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(6.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-24.7f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(6.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //M
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-28.3f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 8.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-30.4f, 14.9f, -5.0f));
        modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(40.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 4.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-30.6f, 15.0f, -5.0f));
        modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(-35.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 4.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-32.8f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 8.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //E
        //E
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-36.3f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0, 8.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-40.3f, 17.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-40.3f, 13.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-40.3f, 10.4f, -5.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0, 1.0, 1.0));
        letters_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        //sitting phot booth
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-41.0f, 1.25f, 2.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.5f, 1.5f, 1.5f));
        //cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //spheretex.drawSphere(lightingShaderWithTexture, laughEmoji, modelMatrixForContainer);
       // cylindertex.drawCylinder(lightingShaderWithTexture, laughEmoji, modelMatrixForContainer);
      
 
        int round_table_count = 3;
        float round_table_distance = 2.0,just=2.0,hey=2.0f;
    //    while (round_table_count--)
    //{
    //    modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(46.0f, 1.5f, round_table_distance));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(2.0f, 0.1f, 2.0f));
    //    disctex_2.drawCylinder(lightingShaderWithTexture, round_table_tex, modelMatrixForContainer);
    //    round_table_distance += 10.0f;

    //    modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(46.0f, 0.5f, just));
    //    modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 0.2f, 0.2f));
    //    cylindertex.drawCylinder(lightingShaderWithTexture, wood_strip_tex, modelMatrixForContainer);
    //    just += 10.0f;

    //    glm::mat4 circle1 = glm::mat4(1.0f);
    //    circle1 = glm::translate(circle1, glm::vec3(46.0f, -0.2f, hey));
    //    circle1 = glm::rotate(circle1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //    circle1 = glm::scale(circle1, glm::vec3(0.05f, 1.0f, 1.0f));
    //    hey += 10.0f;
    //    //cylindertex.drawCylinder(lightingShaderWithTexture, cud, circle1);
    //    //disctex_2.drawCylinder(lightingShaderWithTexture, wood_strip_tex, circle1);
    //    cylinder.drawCylinder(lightingShader, circle1);
    //}
    //round table seat
        int round_table_seat_count = 6;
    //    hey = -0.1;
    //    while (round_table_seat_count--)
    //{
    //    glm::mat4 circle1 = glm::mat4(1.0f);
    //    circle1 = glm::translate(circle1, glm::vec3(46.0f, 1.0f, hey));
    //    circle1 = glm::rotate(circle1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    //    circle1 = glm::scale(circle1, glm::vec3(0.05f, 1.0f, 1.0f));

    //    cylinder.drawCylinder(lightingShader, circle1);

    //    modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(45.2f, -0.4, hey));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 1.4, 0.2));
    //    chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    //    modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(46.5f, -0.4, hey));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 1.4, 0.2));
    //    chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //    hey += 5.0f;
    //}
    //
    //round table right side
    round_table_count = 3;
    round_table_distance = -35.0, just = -35.0, hey = -35.0f;
    while (round_table_count--)
    {
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(round_table_distance, 1.2f, 1.0));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(2.0f, 0.1f, 2.0f));
        disctex_2.drawCylinder(lightingShaderWithTexture, round_table_tex, modelMatrixForContainer);
        round_table_distance += 12.0f;

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(just, 0.2f, 1.0));
        modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 0.2f, 0.2f));
        cylindertex.drawCylinder(lightingShaderWithTexture, wood_strip_tex, modelMatrixForContainer);
        just += 12.0f;

        glm::mat4 circle1 = glm::mat4(1.0f);
        circle1 = glm::translate(circle1, glm::vec3(hey, -0.2f, 1.0));
        circle1 = glm::rotate(circle1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        circle1 = glm::scale(circle1, glm::vec3(0.05f, 1.0f, 1.0f));
        hey += 12.0f;
        //cylindertex.drawCylinder(lightingShaderWithTexture, cud, circle1);
        //disctex_2.drawCylinder(lightingShaderWithTexture, wood_strip_tex, circle1);
        cylinder.drawCylinder(lightingShader, circle1);
    }
    //seat round table right side
    round_table_seat_count = 2;
    hey = -37.0f;
    while (round_table_seat_count--)
    {
        glm::mat4 circle1 = glm::mat4(1.0f);
        circle1 = glm::translate(circle1, glm::vec3(hey, 1.0f, -0.1));
        circle1 = glm::rotate(circle1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        circle1 = glm::scale(circle1, glm::vec3(0.05f, 1.0f, 1.0f));

        cylinder.drawCylinder(lightingShader, circle1);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(hey, -0.4, -0.7));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 1.4, 0.2));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(hey, -0.4, 0.4));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 1.4, 0.2));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        hey += 5.0f;
    }
    round_table_seat_count = 2;
    hey = -25.5f;
    while (round_table_seat_count--)
    {
        glm::mat4 circle1 = glm::mat4(1.0f);
        circle1 = glm::translate(circle1, glm::vec3(hey, 1.0f, -0.1));
        circle1 = glm::rotate(circle1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        circle1 = glm::scale(circle1, glm::vec3(0.05f, 1.0f, 1.0f));

        cylinder.drawCylinder(lightingShader, circle1);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(hey, -0.4, -0.7));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 1.4, 0.2));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(hey, -0.4, 0.4));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 1.4, 0.2));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        hey += 5.0f;
    }
    round_table_seat_count = 2;
    hey = -13.5f;
    while (round_table_seat_count--)
    {
        glm::mat4 circle1 = glm::mat4(1.0f);
        circle1 = glm::translate(circle1, glm::vec3(hey, 1.0f, -0.1));
        circle1 = glm::rotate(circle1, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        circle1 = glm::scale(circle1, glm::vec3(0.05f, 1.0f, 1.0f));

        cylinder.drawCylinder(lightingShader, circle1);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(hey, -0.4, -0.7));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 1.4, 0.2));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(hey, -0.4, 0.4));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 1.4, 0.2));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        hey += 5.0f;
    }
    



    //long table for study 1
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(46.0, 2.0, 35.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(5.0, 0.5, 28.0));
    //long_table_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    //long table cross stands
    float bab = 37.0f;
    int gg = 4;
    while (gg--)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(50.0f, -0.9f, bab));
        model = glm::rotate(model, glm::radians(48.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.2f, 4.8f, 0.2f));
        drawCube(cubeVAO, lightingShader, model, 0.929, 0.929, 0.133, 32.0);
        //rgb01(0.929, 0.929, 0.133)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(46.0f, -1.0f, bab));
        model = glm::rotate(model, glm::radians(-48.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.2f, 5.0f, 0.2f));
        drawCube(cubeVAO, lightingShader, model, 0.929, 0.929, 0.133, 32.0);
        bab += 8.0f;
    }
    //long table for study 2
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(23.0, 2.0, 35.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(5.0, 0.5, 28.0));
    //long_table_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    bab = 37.0f;
    gg = 4;
    //long table cross stands
    while (gg--)
    {
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(27.2f, -0.9f, bab));
        model = glm::rotate(model, glm::radians(48.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.2f, 4.8f, 0.2f));
        drawCube(cubeVAO, lightingShader, model, 0.929, 0.929, 0.133, 32.0);
        //rgb01(0.929, 0.929, 0.133)
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(23.2f, -1.0f, bab));
        model = glm::rotate(model, glm::radians(-48.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.2f, 5.0f, 0.2f));
        drawCube(cubeVAO, lightingShader, model, 0.929, 0.929, 0.133, 32.0);
        bab += 8.0f;
    }
    int chair_count = 4;
    float chair_dist_z = 37.0f,chair_dist_z1=39.2,chair_dist_x=27.5,chair_dist1=29.5;
    //chair
    while (chair_count--)
    {
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(27.5, 1.4, chair_dist_z));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(2.0, 0.4, 2.5));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        //chair back
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(29.5, 1.4, chair_dist_z));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.4, 2.4, 2.5));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //stand
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(29.5, 1.4, chair_dist_z));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.3, -1.6, 0.3));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(27.5, 1.4, chair_dist_z));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.3, -1.6, 0.3));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(27.5, 1.4, chair_dist_z1));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.3, -1.6, 0.3));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(29.5, 1.4, chair_dist_z1));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.3, -1.6, 0.3));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        chair_dist_z += 6.0;
        chair_dist_z1 += 6.0;
    }
    //another time
    chair_count = 4;
    chair_dist_z = 37.0f, chair_dist_z1 = 39.2;
    while (chair_count--)
    {
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(44.0, 1.4, chair_dist_z));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(2.0, 0.4, 2.5));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(44.0, 1.4, chair_dist_z));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.4, 2.4, 2.5));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        //stand
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(44.0, 1.4, chair_dist_z));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.3, -1.6, 0.3));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(45.7, 1.4, chair_dist_z));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.3, -1.6, 0.3));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(44.0, 1.4, chair_dist_z1));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.3, -1.6, 0.3));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(45.7, 1.4, chair_dist_z1));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.3, -1.6, 0.3));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        chair_dist_z += 6.0;
        chair_dist_z1 += 6.0;
    }
   
    
    //books 
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-35.0f, -0.6f, 14.0f));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2f, 7.6f, 16.0f));
    // book_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //sofa in front book shelf
    int sofa_set = 3;
    float dist_seat=15.0, dist_back=10.0;
    while (sofa_set--)
    {
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-25.0, 0.1, dist_seat));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0f, 1.2f, 4.0f));
        sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-22.0, 1.3, dist_seat));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 2.2f, 4.0f));
        sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        //leg
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-21.6, -0.3, dist_seat));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5, 0.4, 0.5));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-25.0, -0.3, dist_seat));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5, 0.4, 0.5));
        chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

        dist_seat += 4.0;
    }
    
    //cash counter left side wall
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(21.0, -0.8, 34.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.1, 10.0, 31.0));
    //cash_left_wall.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //left wall-round table-OUTside)
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(52.5, -0.8, -5.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.5, 10.0, 70.0));
    room_left_wall.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    //back wall-round table-OUTside)
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(13.5, -0.8, 66.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(38.5, 10.0, 1.5));
    room_left_wall.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //2nd portion
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-50.0, -0.8, 66.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(55.5, 10.0, 1.5));
    room_left_wall.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //window up 
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(5.5, 8.0, 66.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(7.9, 1.0, 1.5));
    room_left_wall.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //window down
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(5.5, -0.8, 66.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(7.9, 1.0, 1.5));
    //room_left_wall.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //left wall-round table-INSIde1)
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(52.0, -0.8, -5.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5, 10.0, 40.0));
    //room_left_wall_inside1.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //left wall-round table-INside2)
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(52.0, -0.8, 35.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5, 10.0, 30.0));
    //room_left_wall_inside2.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //mid wall in study section
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(20.0, -0.8, 65.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(32.0, 10.0, 0.5));
    //room_mid_wall_study.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ///////////////////////////////////
    // 
    
    //kitchen right wall
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-4.0, -0.8, 33.5));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.1, 10.0, 30.0));
    //cash_left_wall.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////kitchen door
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(16.6, -0.8, 33.5));
    //modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(-FreezeAngle), glm::vec3(0.0, 1.0, 0.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.4, 10.0, 0.1));
    //wood.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    ////kitchen below
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-3.9, 0.0, 47.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(3.0f, 3.5f, 18.0f));
    //stair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-1.0, 0.0, 62.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(10.0f, 3.5f, 3.0f));
    //stair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    //gg = 4;
    //float okkx = 49.0;
    //while (gg--)
    //{
    //    modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-1.0, 0.0, okkx));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.4f, 3.5f, 0.2f));
    //    chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //    okkx += 4.0;
    //}
    //gg = 4;
    //okkx = 1.0;
    //while (gg--)
    //{
    //    modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(okkx, 0.0, 61.5));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.4f, 3.5f, 0.2f));
    //    chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //    okkx += 2.0;
    //}
    ////fridge
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-3.7, 0.0, 40.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(3.0f, 5.0f, 4.0f));
    //steel.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-0.5, 0.0, 40.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2f, 5.0f, 4.0f));
    //freeze.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    ////oven rack
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(13.0, 0.0, 61.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(5.0f, 4.0f, 4.5f));
    //wood.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    ////oven
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(13.5, 4.5, 61.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(3.0f, 2.0f, 2.0f));
    //steel.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(13.5, 4.5, 60.8));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(3.0f, 2.0f, 0.2f));
    //oven.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    // cash booth behind wall
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-4.0, -0.8, 33.5));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(20.6, 10.0, 0.6));
    //cash_wall.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////cash counter
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(3.0, 0.2, 21.2));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(8.0f, 3.0f, 3.0f));
    //room_mid_wall_study.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(12.0, 0.2, 21.6));
    //modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(-48.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(8.0f, 3.0f, 3.5f));
    //room_mid_wall_study.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-4.6, 0.2, 28.6));
    //modelMatrixForContainer = glm::rotate(modelMatrixForContainer, glm::radians(48.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(10.0f, 3.0f, 3.5f));

    //room_mid_wall_study.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    //gg = 3;
    //bab = 0.0;
    //float bab2 = -2.1;
    //while (gg--)
    //{
    //    // menu stick
    //    modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(bab, 7.0, 21.6));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2, 2.0, 0.2));
    //    chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //    //menu
    //    modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(bab2, 4.8, 21.2));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(3.0f, 3.0f, 0.3f));
    //    menu.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //    
    //    bab += 5.0;
    //    bab2 += 5.0;
    //}
    //
    ////counter food rack
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(0.0, 5.5, 32.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(14.0f, 3.5f, 1.5f));
    //wood.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    gg = 4;
    bab = 0.0;
    while (gg--)
    {
        modelMatrixForContainer = glm::mat4(1.0f);
        modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(bab, 5.5, 31.5));
        modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(3.5f, 3.5f, 0.5f));
        food.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
        bab += 3.6;
    }
    

   
    
    
    //sofa seat
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(20.0, 0.1, 11.2));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0f, 1.2f, 4.0f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////sofa back 1
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(20.0, 1.4, 14.2));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0f, 2.2f, 1.0f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ///*modelMatrixForContainer = glm::mat4(1.0f);
    //    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-21.6, -0.3, dist_seat));
    //    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5, 0.4, 0.5));
    //    chair.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);*/
    ////sofa leg 1
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(20.0, -0.8, 11.2));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.5f, 1.0f, 0.5f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////sofa 2
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(30.0, 0.2, 5.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0f, 1.2f, 4.0f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////sofa back 2
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(33.0, 1.4, 5.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 2.2f, 4.0f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////sofa 3
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(20.0, 0.2, -0.5));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0f, 1.2f, 4.0f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////back 3
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(20.0, 1.4, -0.6));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0f, 2.2f, 1.0f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////sofa 4
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(11.0, 0.2, 5.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(4.0f, 1.2f, 4.0f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////back 4
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(11.0, 1.4, 5.0));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(1.0f, 2.2f, 4.0f));
    //sofa_seat.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    ////between sofa table
    //modelMatrixForContainer = glm::mat4(1.0f);
    //modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(18.0, -0.6, 5.2));
    //modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(8.0f, 1.4f, 3.5f));
    //sofa_table.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //main table
    //table board
   /* scale = glm::scale(identityMatrix, glm::vec3(8.0f, 1.4f, 3.5f));
    translate = glm::translate(identityMatrix, glm::vec3(18.0, -0.8, 5.2));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.212, 0.067, 0.031, 32.0);
    */

    //room right  wall
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-45.0, -0.8, -5.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.4, 10.0, 44.0));
    pattern4.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    

    
    //******wahroom***********//
    // WALL
    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-30.0, -0.8, 38.0));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.1, 10.0, 25.0));
    football_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);

    modelMatrixForContainer = glm::mat4(1.0f);
    modelMatrixForContainer = glm::translate(modelMatrixForContainer, glm::vec3(-45.0f, -0.8f, 38.0f));
    modelMatrixForContainer = glm::scale(modelMatrixForContainer, glm::vec3(0.2f, 10.0f, 25.0f));
    football_cube.drawCubeWithTexture(lightingShaderWithTexture, modelMatrixForContainer);
    //model= glm::mat4(1.0f);
    //model = glm::scale(model, glm::vec3(1.0, 1.0, 1.5));
    //model = glm::translate(model, glm::vec3(-30, -1.8, 28.5));
    //cutcone.drawCutCone(lightingShader, model);
    ////commode
    //model = glm::mat4(1.0f);
    //model  = glm::scale(model, glm::vec3(1.0, 0.1, 1.5));
    //model  = glm::translate(model, glm::vec3(-30, -1.1, 28.5));
    //cutcone.drawCutCone(lightingShader, model);
    ////commode flush tank
    //model = glm::mat4(1.0f);
    //model = glm::scale(model, glm::vec3(1.7, 1.5, 0.7));
    //model = glm::translate(model, glm::vec3(-30.85, -1.1, 29.0));
    //drawCube(cubeVAO, lightingShader, model, 0.122, 0.361, 0.357, 32.0);
    //mirror
    //model = glm::mat4(1.0f);
    //model = glm::scale(model, glm::vec3(0.1, 1.0, 1.0));
    //model = glm::translate(model, glm::vec3(-32.0, 2.5,44.0));
    //cylinder.drawCylinder(lightingShader, model);
    ////basin
    //model = glm::mat4(1.0f);
    //model = glm::scale(model, glm::vec3(1.2, 1.2, 1.2));
    //model = glm::translate(model, glm::vec3(-31.0, 1.6, 44.0));
    //hemisphere.drawHemiSphere(lightingShader, model);




    TransparentShader.use();
    //glm::mat4 projection2 = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    //glm::mat4 view2 = camera.GetViewMatrix();
    TransparentShader.setMat4("projection", projection);
    TransparentShader.setMat4("view", view);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (isFreezeOpen) {
        if (isOpening) {
            if (FreezeAngle < freezeThreshold) {
                FreezeAngle += freezeStep; // Open the door
            }
            else {
                isFreezeOpen = false;      // Stop when fully opened
            }
        }
        else {
            if (FreezeAngle > 0.0f) {
                FreezeAngle -= freezeStep; // Close the door
            }
            else {
                isFreezeOpen = false;      // Stop when fully closed
            }
        }
    }
    //window 1
    float x_shift = 5.5;
    float y_shift = 0.0;
    float z_shift = -66.5f;
    Glass_window_2(transparentVAO, TransparentShader, x_shift, y_shift, z_shift, 0.0f, -FreezeAngle,4.0,8);
    //window 2
    x_shift = 13.5;
    y_shift = 0.0;
    z_shift = -66.5f;
    Glass_window_2(transparentVAO, TransparentShader, x_shift, y_shift, z_shift, 180.0f, FreezeAngle, 4.0, 8);

    if (isLiftDoorOpenDown)
    {
        if (liftDoorPositionDown <= 10.0f)
        {
            liftDoorPositionDown += 0.05f;
        }
    }
    else
    {
        if (liftDoorPositionDown > 0.01f)
        {
            liftDoorPositionDown -= 0.05f;
        }
        else
        {
            liftDoorPositionDown = 0.0f;
        }
    }
    //sliding door
    x_shift = -4.0 - liftDoorPositionDown;
    y_shift = 0.0;
    z_shift = 6.0f;
    Glass_window_2(transparentVAO, TransparentShader, x_shift, y_shift, z_shift, 0.0f, 0.0f, 16.0, 9.5);
    //fixed left
    x_shift = 12.0;
    y_shift = 0.0;
    z_shift = 6.0f;
   Glass_window_2(transparentVAO, TransparentShader, x_shift, y_shift, z_shift, 0.0f, 0.0f, 40.0, 9.5);
   //fixed right
   x_shift = -44.0;
   y_shift = 0.0;
   z_shift = 6.0f;
   Glass_window_2(transparentVAO, TransparentShader, x_shift, y_shift, z_shift, 0.0f, 0.0f, 40.0, 9.5);

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
   /* glm::mat4 scale = glm::mat4(1.0f);
    scale = glm::scale(identityMatrix, glm::vec3(100.0, 0.2, 70.0));
    translate = glm::translate(identityMatrix, glm::vec3(-46.0, -1.0, -5.0));
    glm::mat4 model = translate * scale;*/
    //drawCube(cubeVAO, lightingShader, model, 1.0, 1.0, 1.0, 32.0);

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
    

    

    //washroom wall
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 5.0,10.1));
    translate = glm::translate(identityMatrix, glm::vec3(-23, -0.8, 37.0));
    model = translate * scale;
    drawCube(cubeVAO, lightingShader, model, 0.565f, 0.933f, 0.565f, 32.0);
    ////mirror
    scale = glm::scale(identityMatrix, glm::vec3(0.1, 1.0, 1.0));
    translate = glm::translate(identityMatrix, glm::vec3(-32.0, 2.5, 54.0));
    model = translate * scale;
    cylinder.drawCylinder(lightingShader, model);
    //basin
    scale = glm::scale(identityMatrix, glm::vec3(1.2, 1.2, 1.2));
    translate = glm::translate(identityMatrix, glm::vec3(-31.0, 1.6, 54.0));
    model = translate * scale;
    hemisphere.drawHemiSphere(lightingShader, model);

    
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

    

    //table
    scale = glm::scale(identityMatrix, glm::vec3(2.0, 1.2, 2.0));
    translate = glm::translate(identityMatrix, glm::vec3(-7.0, -0.8, 14.0));
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
    translate = glm::translate(identityMatrix, glm::vec3(-35, 1.8, 44.5));
    model = translate * scale;
    cutcone.drawCutCone(lightingShader, model);
    //commode
    scale = glm::scale(identityMatrix, glm::vec3(1.0, 0.1, 1.5));
    translate = glm::translate(identityMatrix, glm::vec3(-35, 1.1, 44.5));
    model = translate * scale;
    cutcone.drawCutCone(lightingShader, model);
    //commode flush tank
    scale = glm::scale(identityMatrix, glm::vec3(1.7, 1.5, 0.7));
    translate = glm::translate(identityMatrix, glm::vec3(-35.85, 2.1, 45.0));
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
//void ambienton_off(Shader& lightingShader)
//{
//    double currentTime = glfwGetTime();
//    if (currentTime - lastKeyPressTime < keyPressDelay) return;
//    lightingShader.use();
//    if (AmbientON)
//    {
//        pointlight1.turnAmbientOff();
//        pointlight2.turnAmbientOff();
//        lightingShader.setVec3("directionalLight.ambient", 0.0f, 0.0f, 0.0f);
//        lightingShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
//        AmbientON = !AmbientON;
//        lastKeyPressTime = currentTime;
//    }
//    else
//    {
//        pointlight1.turnAmbientOn();
//        pointlight2.turnAmbientOn();
//        lightingShader.setVec3("directionalLight.ambient", 0.2f, 0.2f, 0.2f);
//        lightingShader.setVec3("spotLight.ambient", 0.2f, 0.2f, 0.2f);
//        AmbientON = !AmbientON;
//        lastKeyPressTime = currentTime;
//    }
//}
//void diffuse_on_off(Shader& lightingShader)
//{
//    double currentTime = glfwGetTime();
//    if (currentTime - lastKeyPressTime < keyPressDelay) return;
//    lightingShader.use();
//    if (DiffusionON)
//    {
//        pointlight1.turnDiffuseOff();
//        pointlight2.turnDiffuseOff();
//        lightingShader.setVec3("directionalLight.diffuse", 0.0f, 0.0f, 0.0f);
//        lightingShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
//        DiffusionON = !DiffusionON;
//        lastKeyPressTime = currentTime;
//    }
//    else
//    {
//        pointlight1.turnDiffuseOn();
//        pointlight2.turnDiffuseOn();
//        lightingShader.setVec3("directionalLight.diffuse", 0.8f, 0.8f, 0.8f);
//        lightingShader.setVec3("spotLight.diffuse", 0.8f, 0.8f, 0.8f);
//        DiffusionON = !DiffusionON;
//        lastKeyPressTime = currentTime;
//    }
//}
//void specular_on_off(Shader& lightingShader)
//{
//    double currentTime = glfwGetTime();
//    if (currentTime - lastKeyPressTime < keyPressDelay) return;
//    lightingShader.use();
//    if (SpecularON)
//    {
//        pointlight1.turnSpecularOff();
//        pointlight2.turnSpecularOff();
//        lightingShader.setVec3("directionalLight.specular", 0.0f, 0.0f, 0.0f);
//        lightingShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
//        SpecularON = !SpecularON;
//        lastKeyPressTime = currentTime;
//    }
//    else
//    {
//        pointlight1.turnSpecularOn();
//        pointlight2.turnSpecularOn();
//        lightingShader.setVec3("directionalLight.specular", 1.0f, 1.0f, 1.0f);
//        lightingShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
//        SpecularON = !SpecularON;
//        lastKeyPressTime = currentTime;
//    }
//}
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
            pointlight3.turnOff();
            pointlight4.turnOff();
            pointlight5.turnOff();
            pointlight6.turnOff();
            pointlight7.turnOff();
            pointlight8.turnOff();
            pointlight9.turnOff();
            pointlight10.turnOff();
            pointlight11.turnOff();
            pointlight12.turnOff();
            pointlight13.turnOff();
            //more 10
            pointlight14.turnOff();
            pointlight15.turnOff();
            pointlight16.turnOff();
            pointlight17.turnOff();
            pointlight18.turnOff();
            pointlight19.turnOff();
            pointlight20.turnOff();
            pointlight21.turnOff();
            pointlight22.turnOff();
            pointlight23.turnOff();
            pointLightOn = !pointLightOn;
        }
        else
        {
            pointlight1.turnOn();
            pointlight2.turnOn();
            pointlight3.turnOn();
            pointlight4.turnOn();
            pointlight5.turnOn();
            pointlight6.turnOn();
            pointlight7.turnOn();
            pointlight8.turnOn();
            pointlight9.turnOn();
            pointlight10.turnOn();
            pointlight11.turnOn();
            pointlight12.turnOn();
            pointlight13.turnOn();
            //more 10
            pointlight14.turnOn();
            pointlight15.turnOn();
            pointlight16.turnOn();
            pointlight17.turnOn();
            pointlight18.turnOn();
            pointlight19.turnOn();
            pointlight20.turnOn();
            pointlight21.turnOn();
            pointlight22.turnOn();
            pointlight23.turnOn();
            pointLightOn = !pointLightOn;
        }
    }
    if (key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        directionLightOn = !directionLightOn;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS)
    {
        spotlight1.turnOn();

    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
    {
        spotlight1.turnOff();

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

void Glass_window_2(unsigned int TransparentVAO, Shader transparentShader, float x_shift, float y_shift, float z_shift, float rot,float angle,float length, float H)
{
    //variable declaration
    float local_x = 0.0f, local_y = 0.0f, local_z = 0.f;
    float len = 0.0f, width = 0.0f, height = 0.0f;
    float  rot_x = 0.0f, rot_y = 0.0f, rot_z = 0.0f;
    float trans_x = 0.0f, trans_y = 0.0f, trans_z = 0.0f;

    rot_x = 0.0f, rot_y = rot - rotateAxis_Y_win_2_1, rot_z = 0.0f;
    trans_x = 0.0f, trans_y = 0.0f, trans_z = 0.0f;
    len = length, width = 1.0f, height = H;
    local_x = 0.0f, local_y = 0.0f, local_z = 0.0f;
    GenSheet_Transparent(TransparentVAO, transparentShader, len, width, height, local_x, local_y, local_z, rot_x, rot_y, rot_z, trans_x, trans_y, trans_z, x_shift, y_shift, z_shift, off_white, glass,angle);
}

void GenSheet_Transparent(unsigned int VAO, Shader ourShader, float len, float width, float height, float local_x, float local_y, float local_z, float rot_x, float rot_y, float rot_z, float trans_x, float trans_y, float trans_z, float x_shift, float y_shift, float z_shift, glm::vec4 color, unsigned int texture, float angle)
{
    // Modelling Transformation
    glm::mat4 identityMatrix = glm::mat4(1.0f); // make sure to initialize matrix to identity matrix first
    glm::mat4 local_translateMatrix, translateMatrix, rotateXMatrix, rotateYMatrix, rotateZMatrix, scaleMatrix, model, uni_translate, rotateYMatrix_rot;
    uni_translate = glm::translate(identityMatrix, glm::vec3(x_shift, y_shift, -1.0f * (z_shift)));
    float x_sft = local_x, y_sft = local_y, z_sft = local_z;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);//bindtexture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);//bindtexture
    local_translateMatrix = glm::translate(identityMatrix, glm::vec3(x_sft, y_sft, -1.0f * z_sft));
    translateMatrix = glm::translate(identityMatrix, glm::vec3(trans_x, trans_y, -1.0f * trans_z));
    scaleMatrix = glm::scale(identityMatrix, glm::vec3(len, height, -1.0 * width));
    rotateXMatrix = glm::rotate(identityMatrix, glm::radians(0.0f + rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotateYMatrix_rot = glm::rotate(identityMatrix, glm::radians(0.0f + rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateYMatrix = glm::rotate(identityMatrix, glm::radians(angle), glm::vec3(0.0f, 1.0f, 0.0f));
    rotateZMatrix = glm::rotate(identityMatrix, glm::radians(0.0f + rot_z), glm::vec3(0.0f, 0.0f, 1.0f));

    model = uni_translate * rotateZMatrix * rotateYMatrix_rot * rotateYMatrix * rotateXMatrix * translateMatrix * local_translateMatrix * scaleMatrix;
    ourShader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}
void load_texture(unsigned int& texture, string image_name, GLenum format)
{
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(image_name.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        cout << "Failed to load texture " << image_name << endl;
    }
    stbi_image_free(data);
}

