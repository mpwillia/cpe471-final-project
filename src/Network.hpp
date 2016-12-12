
#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <algorithm>
#include <cmath>
#include <vector>
#include <memory>
#include "Matrix.hpp"

// Activation Functions -------------------------------------------------------
float relu(float x); 
float sigmoid(float x); 

typedef enum NetworkType {
   XOR, OR, AND, NOT,
   RAND_4X4, RAND_8X8, RAND_LARGE, RAND_HUGE,
   FULL_4X4
} NetworkType;


// Network Single Layer -------------------------------------------------------
class Layer {
public: 
   Layer(unsigned int layer_size, unsigned int input_size, float (*act_func)(float), 
         const float* weights, const float* biases);
   Layer(unsigned int layer_size, unsigned int input_size, float (*act_func)(float), 
         const std::vector<float> weights, const std::vector<float> biases);

	virtual ~Layer();
   
   std::shared_ptr<Matrix> compute(const std::shared_ptr<Matrix> input);
   std::shared_ptr<Matrix> output() const;
   std::shared_ptr<Matrix> pre_bias_output() const;
   std::shared_ptr<Matrix> pre_act_output() const;

   std::shared_ptr<Matrix> get_weights() const;
   std::shared_ptr<Matrix> get_biases() const;
   
   unsigned int get_layer_size() const;

private:
   unsigned int layer_size;
   unsigned int input_size;
   float (*act_func)(float);

   std::shared_ptr<Matrix> weights;
   std::shared_ptr<Matrix> biases;
   
   std::shared_ptr<Matrix> output_mat;
   std::shared_ptr<Matrix> pre_bias_output_mat;
   std::shared_ptr<Matrix> pre_act_output_mat;
};


// Multi-Layer Network --------------------------------------------------------
class Network {
public:
   Network(unsigned int input_size, float (*act_func)(float), 
           const std::vector<unsigned int> layer_sizes,
           const std::vector<float*> layer_weights, 
           const std::vector<float*> layer_biases);
   
   Network(unsigned int input_size, float (*act_func)(float), 
           const std::vector<unsigned int> layer_sizes,
           const std::vector<std::vector<float>> layer_weights, 
           const std::vector<std::vector<float>> layer_biases);

	virtual ~Network();
   
   // Computing the Network
   std::shared_ptr<Matrix> compute(const std::vector<float> input);
   std::shared_ptr<Matrix> compute(const std::shared_ptr<Matrix> input);
   
   // Getting Network I/O
   std::shared_ptr<Matrix> input() const;
   std::shared_ptr<Matrix> output() const;
   
   // Getting Layer Info
   std::shared_ptr<std::vector<std::shared_ptr<Matrix>>> layer_outputs(bool include_input = false) const;
   std::shared_ptr<std::vector<std::shared_ptr<Matrix>>> layer_pre_bias_outputs(bool include_input = false) const;
   std::shared_ptr<std::vector<std::shared_ptr<Matrix>>> layer_pre_act_outputs(bool include_input = false) const;
   std::shared_ptr<std::vector<unsigned int>> layer_sizes(bool include_input = false) const;

   std::shared_ptr<Matrix> get_layer_output(unsigned int layer_num) const;
   unsigned int get_layer_size(unsigned int layer_num) const;

   // Getting Layer Parameters
   std::shared_ptr<std::vector<std::shared_ptr<Matrix>>> layer_weights() const;
   std::shared_ptr<std::vector<std::shared_ptr<Matrix>>> layer_biases() const;

   std::shared_ptr<Matrix> get_layer_weights(unsigned int layer_num) const;
   std::shared_ptr<Matrix> get_layer_biases(unsigned int layer_num) const;

   // Get Network Information
   unsigned int get_num_layers() const;
   unsigned int get_input_size() const;

   // Printing Info
   void print_network_state() const;

private:
   unsigned int num_layers;
   unsigned int input_size;
   
   std::vector<Layer> layers;

   std::shared_ptr<Matrix> net_input_mat;
   std::shared_ptr<Matrix> net_output_mat;
};


std::shared_ptr<Network> default_network(NetworkType type);

#endif
