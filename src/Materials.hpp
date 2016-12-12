
#pragma once
#ifndef MATERIALS_HPP
#define MATERIALS_HPP

#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Program.h"

typedef struct Material {
   glm::vec3 ambient;
   glm::vec3 diffuse;
   glm::vec3 specular;
   glm::vec3 emissive;
   float shine;
} Material;


const Material blue_plastic = {glm::vec3(0.02, 0.04, 0.2),
                               glm::vec3(0.0, 0.16, 0.9),
                               glm::vec3(0.14, 0.2, 0.8),
                               glm::vec3(0),
                               120.0};

const Material flat_grey = {glm::vec3(0.13, 0.13, 0.14),
                            glm::vec3(0.3, 0.3, 0.4),
                            glm::vec3(0.3, 0.3, 0.4),
                            glm::vec3(0),
                            4.0};

const Material flat_grey_no_spec = {glm::vec3(0.13, 0.13, 0.14),
                                    glm::vec3(0.3, 0.3, 0.4),
                                    glm::vec3(0.0, 0.0, 0.0),
                                    glm::vec3(0),
                                    0};

const Material brass = {glm::vec3(0.3294, 0.2235, 0.02745),
                        glm::vec3(0.7804, 0.5686, 0.11373),
                        glm::vec3(0.9922, 0.941176, 0.80784),
                        glm::vec3(0),
                        27.9};

const Material copper = {glm::vec3(0.1913, 0.0735, 0.0225),
                         glm::vec3(0.7038, 0.27048, 0.0828),
                         glm::vec3(0.257, 0.1376, 0.08601),
                         glm::vec3(0),
                         12.8};

const Material iridescent = {glm::vec3(0.2,  0.1, 0.2),
                             glm::vec3(0.7,  0.3, 0.8),
                             glm::vec3(-0.25, 0.8,  -0.25),
                             glm::vec3(0),
                             8.0};

const Material emerald = {glm::vec3(0.0215, 0.1745, 0.0215),
                          glm::vec3(0.07568,  0.61424, 0.07568),
                          glm::vec3(0.633, 0.727811,  0.633),
                          glm::vec3(0),
                          0.6};

const Material flat_grass = {glm::vec3(0.0980, 0.1892, 0.0941) * 2.5f,
                             glm::vec3(0.3921,  0.7568, 0.3764),
                             glm::vec3(0.0, 0.0, 0.0),
                             glm::vec3(0),
                             1.0};

const Material flat_sky = {glm::vec3(0.42, 0.44, 0.8) * 20.0f,
                           glm::vec3(0.0, 0.0, 0.0),
                           glm::vec3(0.0, 0.0, 0.0),
                           glm::vec3(0),
                           1.0};

const Material shadow = {glm::vec3(0.1, 0.1, 0.1),
                         glm::vec3(0.0, 0.0, 0.0),
                         glm::vec3(0.0, 0.0, 0.0),
                         glm::vec3(0),
                         1.0};

const Material snake_red = {glm::vec3(0.35, 0.15, 0.15),
                            glm::vec3(0.42, 0.16, 0.11),
                            glm::vec3(0.55, 0.3, 0.25),
                            glm::vec3(0),
                            18.0};

const Material snake_black = {glm::vec3(0.18, 0.1, 0.1),
                              glm::vec3(0.16, 0.13, 0.13),
                              glm::vec3(0.45, 0.4, 0.4),
                              glm::vec3(0),
                              18.0};

const Material snake_yellow = {glm::vec3(0.15, 0.15, 0.1),
                               glm::vec3(0.78, 0.78, 0.28),
                               glm::vec3(0.6, 0.5, 0.2),
                               glm::vec3(0),
                               18.0};


//rgb(86, 103, 255)
//rgb(0.34, 0.40, 1.0);
const Material neuron_base = {glm::vec3(0.10, 0.13, 0.19),
                              glm::vec3(0.34, 0.40, 1.0),
                              glm::vec3(0.5, 0.6, 1.0),
                              glm::vec3(0.88,0.61,-0.21),
                              4.0};

//rgb(226, 155, 54)
//rgb(0.88, 0.61, 0.21);
const Material neuron_act = {glm::vec3(0.22, 0.15, 0.05),
                             glm::vec3(0.88, 0.61, 0.21),
                             glm::vec3(1.0, 0.8, 0.4),
                             glm::vec3(0.88, 0.61, 0.21),
                             2.0};

//rgb(137, 54, 226)
//rgb(0.54, 0.21, 0.88)
const Material neuron_input_base = {glm::vec3(0.15, 0.11, 0.22),
                                    glm::vec3(0.54, 0.21, 0.88),
                                    glm::vec3(0.7, 0.4, 1.0),
                                    glm::vec3(0.95, 0.88, -0.31),
                                    4.0};

//rgb(192, 226, 54)
//rgb(0.75, 0.88, 0.21)
const Material neuron_input_act = {glm::vec3(0.18, 0.22, 0.05),
                                   glm::vec3(0.75, 0.88, 0.21),
                                   glm::vec3(0.9, 1.0, 0.4),
                                   glm::vec3(0.75, 0.88, 0.21),
                                   2.0};


inline void load_material(std::shared_ptr<Program> prog, Material mat) {
   glUniform3f(prog->getUniform("MatAmb"), mat.ambient.r, mat.ambient.g, mat.ambient.b);
   glUniform3f(prog->getUniform("MatDif"), mat.diffuse.r, mat.diffuse.g, mat.diffuse.b);
   glUniform3f(prog->getUniform("MatSpec"), mat.specular.r, mat.specular.g, mat.specular.b);
   glUniform3f(prog->getUniform("MatEmis"), mat.emissive.r, mat.emissive.g, mat.emissive.b);
   glUniform1f(prog->getUniform("shine"), mat.shine);
} 

#endif

