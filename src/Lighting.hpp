
#ifndef LIGHTING_HPP
#define LIGHTING_HPP

#include <memory>
#include <vector>
#include <set>
#include "Program.h"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#define DEFAULT_MAX_LIGHTS 10000

struct Light {
   glm::vec3 pos;
   glm::vec3 color;
   glm::vec3 falloff;
   float brightness;
};

class Lighting {
public:
   Lighting(unsigned int max_lights = DEFAULT_MAX_LIGHTS);
	virtual ~Lighting();
   
   unsigned int num_lights() const;
   
   void add_lights(const std::shared_ptr<Lighting> lights);

   void add_light(glm::vec3 pos, glm::vec3 color, glm::vec3 falloff, float brightness = 1.0);
   void add_light(const Light light);
   void clear_lights();

   void load_zero_lights(std::shared_ptr<Program> prog) const;
   void load_lights(std::shared_ptr<Program> prog, float global_brightness = 1.0) const;
   void load_lights(std::shared_ptr<Program> prog, glm::vec3 obj_position, 
                    int num_lights, float min_dist, float global_brightness = 1.0) const;
   
private:
   unsigned int max_lights;
   std::vector<Light> lights;
   std::multiset<Light,bool(*)(const Light&,const Light&)> light_set;
};

#endif

