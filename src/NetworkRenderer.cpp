
#include <memory>
#include <iostream>
#include <cmath>

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "MatrixStack.h"
#include "Shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Materials.hpp"
#include "Lighting.hpp"

#include "Network.hpp"
#include "NetworkRenderer.hpp"

using namespace std;

float SPACING_SCALE = 4.0;

const int lights_per_conn = 16;
const int lights_per_neuron = 16;

// General Utilities ----------------------------------------------------------

const char* movement_type_str(int enumVal) {
   return MovementTypeStrings[enumVal];
}

void print_render_settings(const RenderSettings render_settings) {
   printf("--- Current Render Settings ---\n");
   printf("   Animation Speed   : %.3f\n", render_settings.animation_speed);
   printf("   Start Delay       : %.3f\n", render_settings.start_delay);
   printf("   Loop              : %s\n", (render_settings.loop ? "True" : "False"));
   const char* move_type_str = movement_type_str(render_settings.move_type);
   if(render_settings.move_exp == 1.0) {
      printf("   Movement Type/Exp : %s\n", move_type_str);
   } else {
      // we want to print the exponent
      const float epsilon = 0.000001;
      float exp = render_settings.move_exp;
      if(abs(exp - (int)(exp)) <= epsilon) {
        printf("   Movement Type/Exp : %s^%d\n", move_type_str, (int)exp);
      } else {
        printf("   Movement Type/Exp : %s^%.2f\n", move_type_str, exp);
      } 
   } 
   printf("\n");
} 

template<typename T>
T lerp(T start, T end, float p) {
   return (1-p)*start + p*end;
} 

// Network Renderer Definition ------------------------------------------------

NetworkRenderer::NetworkRenderer(shared_ptr<Network> network, 
                                 shared_ptr<Shape> neuron_shape, 
                                 shared_ptr<Shape> connection_shape,
                                 shared_ptr<Program> prog,
                                 NeuronProps std_props, NeuronProps input_props,
                                 RenderSettings render_settings) {
   this->network = network; 
   
   int max_layer_size = 0;
   auto layer_sizes = this->network->layer_sizes(true);
   for(int i = 0; i < this->network->get_num_layers(); i++)
      if(layer_sizes->at(i) > max_layer_size) 
         max_layer_size = layer_sizes->at(i);

   SPACING_SCALE = SPACING_SCALE + (int)(this->network->get_num_layers() / 2) + (int)(max_layer_size / 2);

   this->neuron_shape = neuron_shape;
   this->connection_shape = connection_shape;
   this->prog = prog;

   this->std_props = std_props;
   this->input_props = input_props;

   this->lighting = make_shared<Lighting>();
   this->layer_spacing = std_props.base_size * (SPACING_SCALE * 1.5);
   
   this->render_settings = render_settings;

   this->internal_time = -this->render_settings.start_delay;
   this->prev_timestamp = 0;

   this->compute_neuron_positions();
   this->compute_neuron_connections();
} 

NetworkRenderer::~NetworkRenderer() {} 

// Public ---------------------------------------------------------------------

// Setting Render Settings
void NetworkRenderer::set_prog(shared_ptr<Program> prog) {
   this->prog = prog;  
} 

void NetworkRenderer::set_neuron_shape(shared_ptr<Shape> neuron_shape) {
   this->neuron_shape = neuron_shape;
} 

void NetworkRenderer::set_movement(MovementType move_type, float move_exp) {
   RenderSettings new_settings = this->render_settings;
   new_settings.move_type = move_type;
   new_settings.move_exp = move_exp;
   this->set_render_settings(new_settings);
} 

void NetworkRenderer::set_animation_speed(float animation_speed) {
   RenderSettings new_settings = this->render_settings;
   new_settings.animation_speed = animation_speed;
   this->set_render_settings(new_settings);
} 

void NetworkRenderer::set_render_settings(const RenderSettings render_settings) {
   
   this->render_settings = render_settings;
} 

// Private
bool NetworkRenderer::are_settings_new(const RenderSettings render_settings) const {
   if(this->render_settings.animation_speed != render_settings.animation_speed) return true;
   if(this->render_settings.start_delay != render_settings.start_delay) return true;
   if(this->render_settings.loop != render_settings.loop) return true;
   if(this->render_settings.move_type != render_settings.move_type) return true;
   if(this->render_settings.move_exp != render_settings.move_exp) return true;
   return false;
} 


// Setting Inputs to the Neural Network
void NetworkRenderer::set_input(const vector<float> input) {
   //int input_size = static_cast<int>(input.size());
   auto input_mat = make_shared<Matrix>(1, input.size(), input);
   this->set_input(input_mat);
} 

void NetworkRenderer::set_input(const shared_ptr<Matrix> input) {
   if(this->network->input() == nullptr || !this->network->input()->equals(input)) {
      this->network->compute(input);
      this->internal_time = -this->render_settings.start_delay;

      //this->network->print_network_state();
   } 
} 

// Get the lighting model to properly render global objects
const shared_ptr<Lighting> NetworkRenderer::get_lighting() const {
   return this->lighting;
} 

// Main Draw Function
void NetworkRenderer::render(vec3 position, 
                             float ambient_scale, 
                             float global_brightness,
                             shared_ptr<MatrixStack> P, 
                             shared_ptr<MatrixStack> V, 
                             shared_ptr<MatrixStack> M) {

   //float time = glfwGetTime() * this->ani_speed;
   float time = glfwGetTime();
   float delta = time - this->prev_timestamp;
   this->prev_timestamp = time;
   this->internal_time += delta * this->render_settings.animation_speed;

   this->lighting->clear_lights();
   this->lighting->set_global_brightness(global_brightness);
   //this->lighting->add_light(vec3(0,10,0), vec3(1), vec3(1,0,0), 1.0);

   this->ambient_scale = ambient_scale;

   auto layer_outputs = this->network->layer_outputs();
   M->pushMatrix();
      M->translate(position);
      
      //this->compute_lighting(M);
      this->compute_propagation_lighting(M);

      this->render_neurons(P,V,M);
      this->render_connections(P,V,M);

   M->popMatrix();
   
   float end_time = this->network->get_num_layers();
   
   if(this->render_settings.loop && this->internal_time > end_time + this->render_settings.start_delay) {
      this->internal_time = -this->render_settings.start_delay; 
   } else if(!this->render_settings.loop && this->internal_time > end_time){
      this->internal_time = end_time; 
   } 
} 

// Private - Utilities --------------------------------------------------------
float NetworkRenderer::get_neuron_spacing(NeuronProps props) const {
   return props.base_size * (SPACING_SCALE - 0.0);
} 

LayerRenderInfo NetworkRenderer::get_layer_render_info(unsigned int layer_num) const {
   // Layer Info
   unsigned int layer_size;
   shared_ptr<Matrix> layer_output;
   shared_ptr<Matrix> layer_weights;
   vector<vec3> layer_positions = this->positions[layer_num];
   NeuronProps neuron_props;

   // Handle Input vs Hidden Layer
   if(layer_num == 0) {
      layer_size = this->network->get_input_size();
      layer_output = this->network->input();
      layer_weights = nullptr;
      neuron_props = this->input_props;
   } else {
      layer_size = this->network->get_layer_size(layer_num-1);
      layer_output = this->network->get_layer_output(layer_num-1);
      layer_weights = this->network->get_layer_weights(layer_num-1);
      neuron_props = this->std_props;
   } 

   return {layer_size, layer_output, layer_weights, layer_positions, neuron_props};
} 

unsigned int NetworkRenderer::get_current_layer() const {
   int layer_num = (int)this->internal_time;
   if(layer_num < 0) {
      return 0;
   } else if(layer_num >= this->network->get_num_layers()) {
      return this->network->get_num_layers()-1;
   } else {
      return layer_num; 
   } 
} 

float NetworkRenderer::get_current_layer_progress() const {
   int layer_num = (int)this->internal_time;
   float prop_amt = this->internal_time - layer_num;
   if(layer_num < 0 || prop_amt < 0) {
      return 0.0;
   } else if(layer_num >= this->network->get_num_layers()) {
      return 1.0;
   } else {
      return prop_amt; 
   } 
} 

bool NetworkRenderer::should_light_layer(unsigned int layer_num) const {
   int current_layer = this->get_current_layer();
   return abs(current_layer - (int)layer_num) <= 2;
} 

// Private - Precomputations --------------------------------------------------
void NetworkRenderer::compute_neuron_positions() {
   
   auto layer_sizes = this->network->layer_sizes(true);
   
   // NOTE: Layer 0 is the input layer 
   for(int layer_num = 0; layer_num < layer_sizes->size(); layer_num++) {
      unsigned int layer_size = layer_sizes->at(layer_num);
      
      auto props = this->std_props;
      if(layer_num == 0)
         props = this->input_props; 

      float neuron_spacing = this->get_neuron_spacing(props);

      float x_pos = -this->layer_spacing * layer_num;
      vector<vec3> neuron_positions;
      for(int i = 0; i < layer_size; i++) {
         float z_pos = neuron_spacing * i;
         neuron_positions.push_back(vec3(x_pos, 0, z_pos));
      } 
      
      this->positions.push_back(neuron_positions);
   } 
} 

void NetworkRenderer::compute_neuron_connections() {
   for(int i = 0; i < this->network->get_num_layers() + 1; i++) {
      this->compute_neuron_connection(i);
   } 
} 

void NetworkRenderer::compute_neuron_connection(unsigned int layer_num) {
   
   if(layer_num <= 0) return;

   LayerRenderInfo layer_info = this->get_layer_render_info(layer_num);
   LayerRenderInfo prev_layer_info = this->get_layer_render_info(layer_num-1);
   float neuron_size = layer_info.neuron_props.base_size;

   vector<ConnectionInfo> layer_connections;

   for(int prev_i = 0; prev_i < prev_layer_info.size; prev_i++) {
      vec3 prev_pos = prev_layer_info.positions[prev_i];

      for(int cur_i = 0; cur_i < layer_info.size; cur_i++) {

         vec3 cur_pos = layer_info.positions[cur_i];
        
         // figure out our size
         float weight = layer_info.weights->at(cur_i, prev_i);
         float weight_mag = abs(weight);
         if(weight_mag < 0.1) continue;
         float conn_size = (neuron_size + (weight*(neuron_size*0.25))) * 1.5;
         
         //printf("prev_i : %d , cur_i : %d | conn_size : %.3f  |  weight : %.3f\n", prev_i, cur_i, conn_size, weight);

         // find target position
         float d = distance(prev_pos, cur_pos);
         vec3 dst = prev_pos + 0.5f * (cur_pos - prev_pos);
         

         // find target angle
         vec3 c = vec3(cur_pos.x,0,cur_pos.z-1);
         vec3 bc = c - cur_pos;
         vec3 ba = prev_pos - cur_pos;
         float theta = acos(dot(normalize(bc), normalize(ba)));

         ConnectionInfo conn_info;
         conn_info.size = conn_size;
         conn_info.pos = dst;
         conn_info.length = (d/2.0) - (neuron_size*0.5);
         conn_info.theta = theta;
      
         //printf("theta : %.3f\n", conn_info.theta);

         conn_info.start_neuron_idx = prev_i;
         conn_info.end_neuron_idx = cur_i;

         layer_connections.push_back(conn_info);
      }
   }   
   this->connections.push_back(layer_connections);
} 

// Private - Computing Lighting -----------------------------------------------
void NetworkRenderer::compute_propagation_lighting(shared_ptr<MatrixStack> M) {
   
   unsigned int layer_num = this->get_current_layer();
   float prop_amt = this->get_current_layer_progress();

   switch(this->render_settings.move_type) {
      case COS:
         /* f(x) = (1 - cos(x * PI)) / 2.0
          *    for x = [0.0, 0.25 , 0.5, 0.75 , 1.0]
          *     f(x) = [0.0, 0.146, 0.5, 0.853, 1.0]
          */
         prop_amt = (1-cos(prop_amt * M_PI))/2.0;
         break;
      
      case SIN:
         /* f(x) = sin(x * PI * 0.5)
          *    for x = [0.0, 0.25 , 0.5  , 0.75 , 1.0]
          *     f(x) = [0.0, 0.382, 0.707, 0.923, 1.0]
          */
         prop_amt = sin(prop_amt * M_PI * 0.5);
         break;

      case LINEAR: // just to be explicit
         /* f(x) = x
          *    for x = [0.0, 0.25, 0.5, 0.75, 1.0]
          *     f(x) = [0.0, 0.25, 0.5, 0.75, 1.0]
          */
      default:
         break;
   }

   if(this->render_settings.move_exp != 1.0) {
      prop_amt = pow(prop_amt, this->render_settings.move_exp);
   } 

   LayerRenderInfo start_layer_info = this->get_layer_render_info(layer_num);
   LayerRenderInfo end_layer_info = this->get_layer_render_info(layer_num+1);

   float bias_bound = 0.925;

   for (auto &conn_info : this->connections[layer_num]) {
      unsigned int start_idx = conn_info.start_neuron_idx;  
      unsigned int end_idx = conn_info.end_neuron_idx;  

      if(abs(start_layer_info.output->at(start_idx)) < 0.05) {
         continue;
      }

      // Compute Position
      vec3 start_pos = this->positions[layer_num][start_idx];
      vec3 end_pos = this->positions[layer_num+1][end_idx];
      vec3 prop_pos = lerp(start_pos, end_pos, prop_amt);
      
      // Compute Brightness
      float start_val = start_layer_info.output->at(start_idx);
      float end_val = abs(start_val * end_layer_info.weights->at(end_idx));

      if(prop_amt >= bias_bound) {
         float bias_p = (prop_amt - bias_bound) / (1.0 - bias_bound);
         end_val = lerp(end_val, end_layer_info.output->at(end_idx), bias_p);
      } 

      float brightness = lerp(start_val, end_val, prop_amt);

      vec3 light_pos = vec3(M->topMatrix() * vec4(prop_pos, 1));
      this->lighting->add_light(light_pos, 
                                start_layer_info.neuron_props.act_mat.diffuse, 
                                vec3(1,0.1,0.025), 
                                brightness * 1.0);
   } 
   
} 

void NetworkRenderer::compute_lighting(shared_ptr<MatrixStack> M) {
   for(int i = 0; i < this->network->get_num_layers() + 1; i++) {
      this->compute_layer_lighting(i,M);
   } 
} 

void NetworkRenderer::compute_layer_lighting(unsigned int layer_num,
                                             shared_ptr<MatrixStack> M) {

   // Layer Info
   LayerRenderInfo layer_info = this->get_layer_render_info(layer_num);
   
   // Place the Lights
   M->pushMatrix();
   for(int i = 0; i < layer_info.output->get_size(); i++) {
      float output = layer_info.output->at(i);

      M->pushMatrix();
         M->translate(layer_info.positions[i]);
         M->scale(vec3(layer_info.neuron_props.base_size));

         if(output > 0.5) {
            float scale = 3.8;
            float shift = 3.75;
            float speed = 1.0;
            //float p = 1.0;
            vec4 light_pos = M->topMatrix() * vec4(vec3(-1*scale*sin(glfwGetTime()*speed)+shift,0,0),1);
            this->lighting->add_light(vec3(light_pos), 
                                      layer_info.neuron_props.act_mat.diffuse, 
                                      vec3(1,0.1,0.0001), 
                                      0.8);
         } 
      M->popMatrix();
   } 
   M->popMatrix();
}

// Rendering the Network ------------------------------------------------------
void NetworkRenderer::render_neurons(shared_ptr<MatrixStack> P,
                                     shared_ptr<MatrixStack> V,
                                     shared_ptr<MatrixStack> M) {
   this->prog->bind();

   // Send data that's constant between all neurons
   //lighting->load_lights(this->prog);
   glUniform1f(prog->getUniform("ambient_scale"), this->ambient_scale);
   glUniformMatrix4fv(this->prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
   glUniformMatrix4fv(this->prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

   for(int i = 0; i < this->network->get_num_layers() + 1; i++) {
      this->render_layer_neurons(i, P,V,M);
   } 
   this->prog->unbind();
}

void NetworkRenderer::render_layer_neurons(unsigned int layer_num,
                                           shared_ptr<MatrixStack> P,
                                           shared_ptr<MatrixStack> V,
                                           shared_ptr<MatrixStack> M) {
   
   // Layer Info
   LayerRenderInfo layer_info = this->get_layer_render_info(layer_num);
   float neuron_size = layer_info.neuron_props.base_size;

   glUniform1f(prog->getUniform("size"), neuron_size*1.0);
   load_material(this->prog, layer_info.neuron_props.base_mat);

   if(this->should_light_layer(layer_num)) {
      //lighting->load_lights_near(this->prog, layer_info.positions[i], lights_per_neuron, -1);
      lighting->load_lights(this->prog);
   } else {
      lighting->load_zero_lights(this->prog);
   } 

   // Draw the Layer
   M->pushMatrix();
   for(int i = 0; i < layer_info.output->get_size(); i++) {
      M->pushMatrix();
         M->translate(layer_info.positions[i]);
         M->scale(vec3(neuron_size));
         
         glUniformMatrix4fv(this->prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));

         this->neuron_shape->draw(this->prog);
      M->popMatrix();
   } 

   M->popMatrix();
}

void NetworkRenderer::render_connections(std::shared_ptr<MatrixStack> P, 
                                         std::shared_ptr<MatrixStack> V, 
                                         std::shared_ptr<MatrixStack> M) {
   this->prog->bind();

   // Send data that's constant between all connections
   //lighting->load_lights(this->prog);
   glUniform1f(prog->getUniform("ambient_scale"), this->ambient_scale);
   glUniformMatrix4fv(this->prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
   glUniformMatrix4fv(this->prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));

   for(int i = 0; i < this->network->get_num_layers() + 1; i++) {
      this->render_layer_connections(i, P,V,M);
   } 
   this->prog->unbind();
} 

void NetworkRenderer::render_layer_connections(unsigned int layer_num,
                                               std::shared_ptr<MatrixStack> P,
                                               std::shared_ptr<MatrixStack> V,
                                               std::shared_ptr<MatrixStack> M) {
   
   if(layer_num == 0) return;

   LayerRenderInfo layer_info = this->get_layer_render_info(layer_num);
   auto layer_connections = this->connections[layer_num-1];
   
   load_material(this->prog, layer_info.neuron_props.base_mat);

   float layer_width = this->get_neuron_spacing(layer_info.neuron_props) * (layer_info.size+0);
   float light_range = (layer_width > this->layer_spacing*2) ? layer_width : this->layer_spacing*2;

   if(this->should_light_layer(layer_num)) {
      //lighting->load_lights_near(this->prog, pos, lights_per_conn, -1);
      lighting->load_lights(this->prog);
   } else {
      lighting->load_zero_lights(this->prog);
   }

   for(int i = 0; i < layer_connections.size(); i++) {
      M->pushMatrix();

         auto conn_info = layer_connections[i];

         M->translate(conn_info.pos);
         M->rotate(-conn_info.theta, vec3(0,1,0));
         M->rotate(M_PI/2.0, vec3(1,0,0));
         M->scale(vec3(conn_info.size, conn_info.length, conn_info.size));
         
         vec3 pos = vec3(M->topMatrix() * vec4(vec3(0), 1));

         glUniform1f(prog->getUniform("size"), conn_info.size*0.5);
         glUniformMatrix4fv(this->prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));

         this->connection_shape->draw(this->prog);

      M->popMatrix();
   } 
}





