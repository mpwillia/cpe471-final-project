
#include <stdexcept>
#include <memory>
#include <vector>
#include <set>
#include <algorithm>
#include <iterator>
#include <cmath>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Lighting.hpp"

using namespace std;
using namespace glm;

void send_light_vector(shared_ptr<Program> prog, const vector<Light>& lights, float global_brightness) {
   unsigned int num_lights = lights.size(); 

   vector<vec3> light_positions = vector<vec3>();
   vector<vec3> light_colors = vector<vec3>();
   vector<vec3> light_falloffs = vector<vec3>();
   vector<float> light_brightness = vector<float>();

   for(int i = 0; i < num_lights; i++) {
      light_positions.push_back(lights.at(i).pos);
      light_colors.push_back(lights.at(i).color);
      light_falloffs.push_back(lights.at(i).falloff);
      light_brightness.push_back(lights.at(i).brightness);
   } 

   const float* flat_pos = &light_positions[0].x;
   const float* flat_colors = &light_colors[0].x;
   const float* flat_falloffs = &light_falloffs[0].x;
   const float* flat_brightness = &light_brightness[0];

   glUniform1i(prog->getUniform("num_lights"), num_lights);
   glUniform3fv(prog->getUniform("light_positions"), num_lights, flat_pos);
   glUniform3fv(prog->getUniform("light_colors"), num_lights, flat_colors);
   glUniform3fv(prog->getUniform("light_falloffs"), num_lights, flat_falloffs);
   glUniform1fv(prog->getUniform("light_brightness"), num_lights, flat_brightness);
      
   glUniform1f(prog->getUniform("global_brightness"), global_brightness);   
} 

bool light_cmp (const Light& lhs, const Light& rhs) {
   return lhs.pos.z < rhs.pos.z;
};



Lighting::Lighting(unsigned int max_lights) : max_lights(max_lights), global_brightness(1.0f) {
   lights = vector<Light>();
   light_set = multiset<Light,bool(*)(const Light&,const Light&)>(light_cmp);
} 

Lighting::~Lighting() {} 


unsigned int Lighting::num_lights() const {
   return this->lights.size();
} 

void Lighting::set_global_brightness(float global_brightness) {
   this->global_brightness = global_brightness;  
} 

void Lighting::add_lights(const shared_ptr<Lighting> lights) {
   for(int i = 0; i < lights->num_lights(); i++) {
      this->add_light(lights->lights[i]);
   } 
} 

void Lighting::add_light(const Light light) {
   this->add_light(light.pos, light.color, light.falloff, light.brightness);
}

void Lighting::add_light(vec3 pos, vec3 color, vec3 falloff, float brightness) {
   if(this->num_lights() >= max_lights) {
      printf("Too many lights in current lighting model!\n");
      printf("Max is %d!", max_lights);
      throw out_of_range("Too many lights in current lighting model!");
   } 
  
   Light light = {pos, color, falloff, brightness};
   this->lights.push_back(light);
   this->light_set.insert(light);
} 

void Lighting::clear_lights() {
   this->lights.clear();
   this->light_set.clear();
} 

void Lighting::load_zero_lights(shared_ptr<Program> prog) const {
   glUniform1i(prog->getUniform("num_lights"), 0);
   //glUniform1f(prog->getUniform("global_brightness"), 1.0);   
} 

void Lighting::load_lights(shared_ptr<Program> prog) const {
   send_light_vector(prog, this->lights, this->global_brightness);
} 

void Lighting::load_lights_near(shared_ptr<Program> prog, vec3 obj_position, 
                                int max_lights, float max_dist) const {

   if(max_lights > this->light_set.size())
      max_lights = this->light_set.size();
   
   vector<Light> subset;
   for(auto& light : this->lights) {
      if(max_dist < 0 || distance(light.pos, obj_position) < max_dist)
         subset.push_back(light);
      
      /*
      if(max_dist < 0 || abs(light.pos.z - obj_position.z) < max_dist)
         subset.push_back(light);
      */
   } 

   if(subset.size() < max_lights) {
      send_light_vector(prog, subset, this->global_brightness);
   } else {

      auto dist_cmp = [&](const Light& lhs, const Light& rhs) {
         //return (abs(lhs.pos.z - obj_position.z) < (abs(rhs.pos.z - obj_position.z)));
         return (distance(lhs.pos, obj_position) < distance(rhs.pos, obj_position));
      };

      sort(subset.begin(), subset.end(), dist_cmp);
      
      send_light_vector(prog, vector<Light>(subset.begin(), subset.begin()+max_lights), 
                        this->global_brightness);
   }

}


