
#pragma once
#ifndef NETWORKRENDERER_HPP
#define NETWORKRENDERER_HPP

#include <memory>

#include "MatrixStack.h"
#include "Shape.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Materials.hpp"
#include "Lighting.hpp"
#include "Network.hpp"


struct NeuronProps {
   Material base_mat;
   Material act_mat;
   float base_size;
};

const NeuronProps std_neuron_props = {neuron_base, neuron_act, 5.0f};
const NeuronProps input_neuron_props = {neuron_input_base, neuron_input_act, 8.0f};


// Define a collection of data needed for rendering
struct LayerRenderInfo {
   unsigned int size;
   std::shared_ptr<Matrix> output;
   std::shared_ptr<Matrix> weights; // for connections
   std::vector<glm::vec3> positions;
   NeuronProps neuron_props;
};

// Defines the information that makes up a single connection
struct ConnectionInfo {
   glm::vec3 pos;
   float theta;
   float size;
   float length;

   // indices used for light placement
   unsigned int start_neuron_idx;
   unsigned int end_neuron_idx;
};

enum MovementType {
   LINEAR,
   COS,
   SIN
};
static const char* MovementTypeStrings[] = { "LINEAR", "COS", "SIN" };
const char* movement_type_str(int enumVal);

// Defines a set of settings that change how the network is rendered
struct RenderSettings {
   float animation_speed; // how fast the animations should be
   float start_delay; // the delay between starting and restarting the animation
   bool loop; // Whether or not to loop the animation

   MovementType move_type;
   float move_exp;
};

const RenderSettings default_render_settings = {0.5, 0.1, true, COS, 1.0};

void print_render_settings(const RenderSettings render_settings);

class NetworkRenderer {
public:
   NetworkRenderer(std::shared_ptr<Network> network,
                   std::shared_ptr<Shape> neuron_shape,
                   std::shared_ptr<Shape> connection_shape,
                   std::shared_ptr<Program> prog,
                   NeuronProps std_props = std_neuron_props, 
                   NeuronProps input_props = input_neuron_props,
                   RenderSettings render_settings = default_render_settings);
   
	virtual ~NetworkRenderer();
   
   // Setting Render Settings -------------------------------------------------
   void set_prog(std::shared_ptr<Program> prog);
   void set_neuron_shape(std::shared_ptr<Shape> neuron_shape);
   void set_movement(MovementType move_type, float move_exp = 1.0);
   void set_animation_speed(float animation_speed);
   
   void set_render_settings(const RenderSettings render_settings);

   // Getting Lighting Model --------------------------------------------------
   const std::shared_ptr<Lighting> get_lighting() const;
   
   // Settings Neural Network Input -------------------------------------------
   void set_input(const std::vector<float> input);
   void set_input(const std::shared_ptr<Matrix> input);
   
   // Main Draw Function
   void render(glm::vec3 position, float ambient_scale, float global_brightness,
               std::shared_ptr<MatrixStack> P, std::shared_ptr<MatrixStack> V, 
               std::shared_ptr<MatrixStack> M);


private:

   // Utilities ---------------------------------------------------------------
   bool are_settings_new(const RenderSettings render_settings) const;
   float get_neuron_spacing(NeuronProps props) const;
   LayerRenderInfo get_layer_render_info(unsigned int layer_num) const;

   // Precomputations ---------------------------------------------------------
   void compute_neuron_positions();
   void compute_neuron_connections();
   void compute_neuron_connection(unsigned int layer_num);
   
   // Lighting ----------------------------------------------------------------
   void compute_propagation_lighting(std::shared_ptr<MatrixStack> M);
   void compute_lighting(std::shared_ptr<MatrixStack> M);
   
   void compute_layer_lighting(unsigned int layer_num,
                               std::shared_ptr<MatrixStack> M);

   // Neurons -----------------------------------------------------------------
   void render_neurons(std::shared_ptr<MatrixStack> P, 
                       std::shared_ptr<MatrixStack> V, 
                       std::shared_ptr<MatrixStack> M);
      
   void render_layer_neurons(unsigned int layer_num,
                            std::shared_ptr<MatrixStack> P,
                            std::shared_ptr<MatrixStack> V,
                            std::shared_ptr<MatrixStack> M);

   // Connections -------------------------------------------------------------
   void render_connections(std::shared_ptr<MatrixStack> P, 
                       std::shared_ptr<MatrixStack> V, 
                       std::shared_ptr<MatrixStack> M);
      
   void render_layer_connections(unsigned int layer_num,
                            std::shared_ptr<MatrixStack> P,
                            std::shared_ptr<MatrixStack> V,
                            std::shared_ptr<MatrixStack> M);
   
   std::shared_ptr<Network> network;
   std::shared_ptr<Shape> neuron_shape;
   std::shared_ptr<Shape> connection_shape;
   std::shared_ptr<Program> prog;
   
   std::vector<std::vector<glm::vec3>> positions;
   std::vector<std::vector<ConnectionInfo>> connections;

   NeuronProps std_props;
   NeuronProps input_props;

   std::shared_ptr<Lighting> lighting;
   float ambient_scale;
   float layer_spacing;

   float internal_time;
   float prev_timestamp; // for computing time deltas

   unsigned int get_current_layer() const;
   float get_current_layer_progress() const;
   bool should_light_layer(unsigned int layer_num) const;


   RenderSettings render_settings;
   
   /*
   float ani_speed;
   float start_delay;
   MovementType move_type;
   */
};

#endif

