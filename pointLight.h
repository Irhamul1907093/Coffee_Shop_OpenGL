//
//  pointLight.h
//  test
//
//  Created by Nazirul Hasan on 22/9/23.
//

#ifndef pointLight_h
#define pointLight_h
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"
#include <iostream>
#include <string>

using namespace std;

class PointLight {
public:
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float k_c;
    float k_l;
    float k_q;
    int Number;

    PointLight(float posX, float posY, float posZ, float ambR, float ambG, float ambB, float diffR, float diffG, float diffB, float specR, float specG, float specB, float constant, float linear, float quadratic, int num) {

        position = glm::vec3(posX, posY, posZ);
        ambient = glm::vec3(ambR, ambG, ambB);
        diffuse = glm::vec3(diffR, diffG, diffB);
        specular = glm::vec3(specR, specG, specB);
        k_c = constant;
        k_l = linear;
        k_q = quadratic;
        Number = num - 1;
    }
    void setUpPointLight(Shader& lightingShader)
    {
        lightingShader.use();
        string lightNumber1 = to_string(Number);
        lightingShader.setVec3("pointLights[" + lightNumber1 + "].position", position);
        lightingShader.setVec3("pointLights[" + lightNumber1 + "].ambient", ambient * ambientOn);
        lightingShader.setVec3("pointLights[" + lightNumber1 + "].diffuse", diffuse * diffuseOn);
        lightingShader.setVec3("pointLights[" + lightNumber1 + "].specular", specular * specularOn);
        lightingShader.setFloat("pointLights[" + lightNumber1 + "].k_c", k_c);
        lightingShader.setFloat("pointLights[" + lightNumber1 + "].k_l", k_l);
        lightingShader.setFloat("pointLights[" + lightNumber1 + "].k_q", k_q);


        /*
        if (lightNumber == 1) {
            lightingShader.setVec3("pointLights[0].position", position);
            lightingShader.setVec3("pointLights[0].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[0].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[0].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[0].k_c", k_c);
            lightingShader.setFloat("pointLights[0].k_l", k_l);
            lightingShader.setFloat("pointLights[0].k_q", k_q);
        }

        else if (lightNumber == 2)
        {
            lightingShader.setVec3("pointLights[1].position", position);
            lightingShader.setVec3("pointLights[1].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[1].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[1].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[1].k_c", k_c);
            lightingShader.setFloat("pointLights[1].k_l", k_l);
            lightingShader.setFloat("pointLights[1].k_q", k_q);
        }
        else if (lightNumber == 3)
        {
            lightingShader.setVec3("pointLights[2].position", position);
            lightingShader.setVec3("pointLights[2].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[2].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[2].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[2].k_c", k_c);
            lightingShader.setFloat("pointLights[2].k_l", k_l);
            lightingShader.setFloat("pointLights[2].k_q", k_q);
        }
        else if (lightNumber == 4)
        {
            lightingShader.setVec3("pointLights[3].position", position);
            lightingShader.setVec3("pointLights[3].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[3].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[3].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[3].k_c", k_c);
            lightingShader.setFloat("pointLights[3].k_l", k_l);
            lightingShader.setFloat("pointLights[3].k_q", k_q);
        }
        else if (lightNumber == 5)
        {
            lightingShader.setVec3("pointLights[4].position", position);
            lightingShader.setVec3("pointLights[4].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[4].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[4].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[4].k_c", k_c);
            lightingShader.setFloat("pointLights[4].k_l", k_l);
            lightingShader.setFloat("pointLights[4].k_q", k_q);
        }
        else if (lightNumber == 6)
        {
            lightingShader.setVec3("pointLights[5].position", position);
            lightingShader.setVec3("pointLights[5].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[5].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[5].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[5].k_c", k_c);
            lightingShader.setFloat("pointLights[5].k_l", k_l);
            lightingShader.setFloat("pointLights[5].k_q", k_q);
        }
        else if (lightNumber == 7)
        {
            lightingShader.setVec3("pointLights[6].position", position);
            lightingShader.setVec3("pointLights[6].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[6].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[6].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[6].k_c", k_c);
            lightingShader.setFloat("pointLights[6].k_l", k_l);
            lightingShader.setFloat("pointLights[6].k_q", k_q);
        }
        else if (lightNumber == 8)
        {
            lightingShader.setVec3("pointLights[7].position", position);
            lightingShader.setVec3("pointLights[7].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[7].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[7].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[7].k_c", k_c);
            lightingShader.setFloat("pointLights[7].k_l", k_l);
            lightingShader.setFloat("pointLights[7].k_q", k_q);
        }
        else if (lightNumber == 9)
        {
            lightingShader.setVec3("pointLights[8].position", position);
            lightingShader.setVec3("pointLights[8].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[8].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[8].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[8].k_c", k_c);
            lightingShader.setFloat("pointLights[8].k_l", k_l);
            lightingShader.setFloat("pointLights[8].k_q", k_q);
        }
        else if (lightNumber == 10)
        {
            lightingShader.setVec3("pointLights[9].position", position);
            lightingShader.setVec3("pointLights[9].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[9].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[9].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[9].k_c", k_c);
            lightingShader.setFloat("pointLights[9].k_l", k_l);
            lightingShader.setFloat("pointLights[9].k_q", k_q);
        }
        else
        {
            lightingShader.setVec3("pointLights[10].position", position);
            lightingShader.setVec3("pointLights[10].ambient", ambientOn * ambient);
            lightingShader.setVec3("pointLights[10].diffuse", diffuseOn * diffuse);
            lightingShader.setVec3("pointLights[10].specular", specularOn * specular);
            lightingShader.setFloat("pointLights[10].k_c", k_c);
            lightingShader.setFloat("pointLights[10].k_l", k_l);
            lightingShader.setFloat("pointLights[10].k_q", k_q);
        }*/
    }
    void turnOff()
    {
        ambientOn = 0.0;
        diffuseOn = 0.0;
        specularOn = 0.0;
    }
    void turnOn()
    {
        ambientOn = 1.0;
        diffuseOn = 1.0;
        specularOn = 1.0;
    }
    void turnAmbientOn()
    {
        ambientOn = 1.0;
        diffuseOn = 0.0;
        specularOn = 0.0;
    }
    void turnAmbientOff()
    {
        ambientOn = 0.0;

    }
    void turnDiffuseOn()
    {
        diffuseOn = 1.0;
        ambientOn = 0.0;
        specularOn = 0.0;
    }
    void turnDiffuseOff()
    {
        diffuseOn = 0.0;

    }
    void turnSpecularOn()
    {
        specularOn = 1.0;
        ambientOn = 0.0;
        diffuseOn = 0.0;
    }
    void turnSpecularOff()
    {
        specularOn = 0.0;

    }
private:
    float ambientOn = 1.0;
    float diffuseOn = 1.0;
    float specularOn = 1.0;
};

#endif /* pointLight_h */