
#ifndef BezierCurve_h
#define BezierCurve_h

#include <glad/glad.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "shader.h"

# define PI 3.1416

using namespace std;

class BezierCurve
{
public:
    vector <float> cntrlPoints;
    glm::vec4 ambient;
    glm::vec4 diffuse;
    glm::vec4 specular;
    float shininess;
    unsigned int texture;
    // ctor/dtor
    BezierCurve(GLfloat controlpoints[], int size, unsigned int tex, glm::vec4 amb = glm::vec4(1.0, 0.0, 0.0, 1.0f), glm::vec4 diff = glm::vec4(1.0, 0.0, 0.0, 1.0f), glm::vec4 spec = glm::vec4(0.1f, 0.1f, 0.1f, 0.5f), float shiny = 32.0f)
    {
        for (int i = 0; i < size; i++)
        {
            this->cntrlPoints.push_back(controlpoints[i]);
        }
        this->ambient = amb;
        this->diffuse = diff;
        this->specular = spec;
        this->shininess = shiny;
        this->texture = tex;
        sphereVAO = hollowBezier(cntrlPoints.data(), ((unsigned int)cntrlPoints.size() / 3) - 1);

    }
    ~BezierCurve() {}
    // draw in VertexArray mode
    void drawBezierCurve(Shader& lightingShader, glm::mat4 model) const      // draw surface
    {
        //glBindTexture(GL_TEXTURE_2D, this->texture);
        lightingShader.use();
        lightingShader.setVec3("material.ambient", glm::vec3(0.969, 0.776, 0.561));
        lightingShader.setVec3("material.diffuse", glm::vec3(0.969, 0.776, 0.561));
        lightingShader.setVec3("material.specular", glm::vec3(1.0f, 1.0f, 1.0f));
        lightingShader.setFloat("material.shininess", 32.0f);
        lightingShader.setMat4("model", model);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture);

        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES,                    // primitive type
            (unsigned int)indices.size(),          // # of indices
            GL_UNSIGNED_INT,                 // data type
            (void*)0);                       // offset to indices

        // unbind VAO
        glBindVertexArray(0);
    }

private:
    // member functions
    long long nCr(int n, int r)
    {
        if (r > n / 2)
            r = n - r; // because C(n, r) == C(n, n - r)
        long long ans = 1;
        int i;

        for (i = 1; i <= r; i++)
        {
            ans *= n - r + i;
            ans /= i;
        }

        return ans;
    }
    //polynomial interpretation for N points
    void BezierCurveFN(double t, float xy[2], GLfloat ctrlpoints[], int L)
    {
        double y = 0;
        double x = 0;
        t = t > 1.0 ? 1.0 : t;
        for (int i = 0; i < L + 1; i++)
        {
            long long ncr = nCr(L, i);
            double oneMinusTpow = pow(1 - t, double(L - i));
            double tPow = pow(t, double(i));
            double coef = oneMinusTpow * tPow * ncr;
            x += coef * ctrlpoints[i * 3];
            y += coef * ctrlpoints[(i * 3) + 1];

        }
        xy[0] = float(x);
        xy[1] = float(y);
    }


    unsigned int hollowBezier(GLfloat ctrlpoints[], int L)
    {
        int i, j;
        float x, y, z, r;                //current coordinates
        float theta;
        float nx, ny, nz, lengthInv;    // vertex normal
        float s, t1; // texturecoord

        const float dtheta = 2 * pi / ntheta;        //angular step size

        float t = 0;
        float dt = 1.0 / nt;
        float xy[2];

        for (i = 0; i <= nt; ++i)              //step through y
        {
            BezierCurveFN(t, xy, ctrlpoints, L);
            r = xy[0];
            y = xy[1];
            theta = 0;
            t += dt;
            lengthInv = 1.0 / r;
            //u = 0;
            //v = 0.025 * (nt - i);//(1/40)

            for (j = 0; j <= ntheta; ++j)
            {
                double cosa = cos(theta);
                double sina = sin(theta);
                z = r * cosa;
                x = r * sina;

                coordinates.push_back(x);
                coordinates.push_back(y);
                coordinates.push_back(z);

                // normalized vertex normal (nx, ny, nz)
                // center point of the circle (0,y,0)
                nx = (x - 0) * lengthInv;
                ny = (y - y) * lengthInv;
                nz = (z - 0) * lengthInv;

                normals.push_back(nx);
                normals.push_back(ny);
                normals.push_back(nz);

                s = (float)j / ntheta; // U-coordinate
                t1 = (float)i / nt; // V-coordinate
                texCoords.push_back(s);
                texCoords.push_back(t1);

                theta += dtheta;
            }
        }

        // generate index list of triangles
        // k1--k1+1
        // |  / |
        // | /  |
        // k2--k2+1

        int k1, k2;
        for (int i = 0; i < nt; ++i)
        {
            k1 = i * (ntheta + 1);     // beginning of current stack
            k2 = k1 + ntheta + 1;      // beginning of next stack

            for (int j = 0; j < ntheta; ++j, ++k1, ++k2)
            {
                // k1 => k2 => k1+1
                indices.push_back(k1);
                indices.push_back(k2);
                indices.push_back(k1 + 1);

                // k1+1 => k2 => k2+1
                indices.push_back(k1 + 1);
                indices.push_back(k2);
                indices.push_back(k2 + 1);
            }
        }

        size_t count = coordinates.size();
        size_t counttex = texCoords.size();
        for (int i = 0; i < count; i += 3)
        {
            vertices.push_back(coordinates[i]);
            vertices.push_back(coordinates[i + 1]);
            vertices.push_back(coordinates[i + 2]);

            vertices.push_back(normals[i]);
            vertices.push_back(normals[i + 1]);
            vertices.push_back(normals[i + 2]);

            size_t texIndex = i / 3 * 2;
            vertices.push_back(texCoords[texIndex]);
            vertices.push_back(texCoords[texIndex + 1]);
        }

        unsigned int bezierVAO;
        glGenVertexArrays(1, &bezierVAO);
        glBindVertexArray(bezierVAO);

        // create VBO to copy vertex data to VBO
        unsigned int bezierVBO;
        glGenBuffers(1, &bezierVBO);
        glBindBuffer(GL_ARRAY_BUFFER, bezierVBO);           // for vertex data
        glBufferData(GL_ARRAY_BUFFER,                   // target
            (unsigned int)vertices.size() * sizeof(float), // data size, # of bytes
            vertices.data(),   // ptr to vertex data
            GL_STATIC_DRAW);                   // usage

        // create EBO to copy index data
        unsigned int bezierEBO;
        glGenBuffers(1, &bezierEBO);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bezierEBO);   // for index data
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,           // target
            (unsigned int)indices.size() * sizeof(unsigned int),             // data size, # of bytes
            indices.data(),               // ptr to index data
            GL_STATIC_DRAW);                   // usage

        // activate attrib arrays
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        // set attrib arrays with stride and offset
        int stride = 32;     // should be 24 bytes
        glVertexAttribPointer(0, 3, GL_FLOAT, false, stride, (void*)0);
        glVertexAttribPointer(1, 3, GL_FLOAT, false, stride, (void*)(sizeof(float) * 3));
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(sizeof(float) * 6));

        // unbind VAO, VBO and EBO
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        return bezierVAO;
    }

    // memeber vars
    unsigned int sphereVAO;

    const double pi = 3.14159265389;
    const int nt = 40;
    const int ntheta = 20;
    vector<float> vertices;
    vector<float> normals;
    vector<unsigned int> indices;
    vector<float> coordinates;
    vector<float> texCoords;
    int verticesStride;                 // # of bytes to hop to the next vertex (should be 24 bytes)

};


#endif /* sphere_h */


