
#include <memory>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include "Matrix.hpp"
#include "Network.hpp"

using namespace std;

// Activation Functions -------------------------------------------------------
float relu(float x) {
   return std::max(0.0f, x);
}
float sigmoid(float x) {
   return 1.0 / (1.0 + std::pow(M_E, -x));
}


// Network Single Layer -------------------------------------------------------
Layer::Layer(unsigned int layer_size, unsigned int input_size, 
             float (*act_func)(float), 
             const float* weights, const float* biases) { 
   
   this->layer_size = layer_size;
   this->input_size = input_size;
   this->act_func = act_func;
   
   this->weights = make_shared<Matrix>(input_size, layer_size, weights);
   this->biases = make_shared<Matrix>(1, layer_size, biases);

   this->output_mat = nullptr;
} 

Layer::Layer(unsigned int layer_size, unsigned int input_size, 
             float (*act_func)(float), 
             const vector<float> weights, const vector<float> biases) {
   
   this->layer_size = layer_size;
   this->input_size = input_size;
   this->act_func = act_func;

   this->weights = make_shared<Matrix>(input_size, layer_size, weights);
   this->biases = make_shared<Matrix>(1, layer_size, biases);
   
   this->output_mat = nullptr;
   this->pre_bias_output_mat = nullptr;
   this->pre_act_output_mat = nullptr;
} 

Layer::~Layer() {} 

shared_ptr<Matrix> Layer::compute(const std::shared_ptr<Matrix> input) {
   this->pre_bias_output_mat = input->dot(this->weights);
   this->pre_act_output_mat = pre_bias_output_mat->add(this->biases);
   this->output_mat = pre_act_output_mat->apply(this->act_func);
   //this->output_mat = pre_bias_output_mat->add(this->biases)->apply(this->act_func);
   return this->output_mat;
} 

shared_ptr<Matrix> Layer::output() const {
   return this->output_mat;
} 

shared_ptr<Matrix> Layer::pre_bias_output() const {
   return this->pre_bias_output_mat;
} 

shared_ptr<Matrix> Layer::pre_act_output() const {
   return this->pre_act_output_mat; 
} 

shared_ptr<Matrix> Layer::get_weights() const {
   return this->weights; 
} 

shared_ptr<Matrix> Layer::get_biases() const {
   return this->biases;
} 

unsigned int Layer::get_layer_size() const {
   return this->layer_size; 
} 


// Multi-Layer Network --------------------------------------------------------
Network::Network(unsigned int input_size, float (*act_func)(float), 
                 const vector<unsigned int> layer_sizes,
                 const vector<float*> layer_weights, 
                 const vector<float*> layer_biases) {
   this->input_size = input_size;
   this->num_layers = static_cast<unsigned int>(layer_sizes.size());
   this->net_input_mat = nullptr;
   this->net_output_mat = nullptr;
   
   //this->layers = make_shared<vector<shared_ptr<Layer>>>();
   
   // Initialize Layers
   unsigned int prev_output_size = input_size;
   for(int i = 0; i < this->num_layers; i++) {
      auto layer_size = layer_sizes[i];
      auto weights = layer_weights[i];
      auto biases = layer_biases[i];

      //auto layer = make_shared<Layer>(layer_size, prev_output_size, act_func, weights, biases);
      auto layer = Layer(layer_size, prev_output_size, act_func, weights, biases);
      this->layers.push_back(layer);

      prev_output_size = layer_size;
   }  
} 

Network::Network(unsigned int input_size, float (*act_func)(float), 
                 const vector<unsigned int> layer_sizes,
                 const vector<vector<float>> layer_weights, 
                 const vector<vector<float>> layer_biases) {
   this->input_size = input_size;
   this->num_layers = static_cast<unsigned int>(layer_sizes.size());
   this->net_input_mat = nullptr;
   this->net_output_mat = nullptr;
   
   //this->
   //this->layers = make_shared<vector<shared_ptr<Layer>>>();

   unsigned int prev_output_size = input_size;
   for(int i = 0; i < this->num_layers; i++) {
      auto layer_size = layer_sizes[i];
      auto weights = layer_weights[i];
      auto biases = layer_biases[i];

      //auto layer = make_shared<Layer>(layer_size, prev_output_size, act_func, weights, biases);
      auto layer = Layer(layer_size, prev_output_size, act_func, weights, biases);
      this->layers.push_back(layer);

      prev_output_size = layer_size;
   }  
} 

Network::~Network() {} 

// Getting Network Info -------------------------------------------------------
unsigned int Network::get_num_layers() const {
   return this->num_layers; 
} 

unsigned int Network::get_input_size() const {
   return this->input_size; 
} 

// Sending data through the network -------------------------------------------
shared_ptr<Matrix> Network::compute(const vector<float> input) {
   int input_size = static_cast<int>(input.size());
   auto input_mat = make_shared<Matrix>(1, input_size, input);
   return this->compute(input_mat);
} 

shared_ptr<Matrix> Network::compute(const shared_ptr<Matrix> input) {
   this->net_input_mat = input;
   auto output = this->layers[0].compute(input);
   //auto output = this->layers->at(0)->compute(input);
   for(int layer_num = 1; layer_num < this->num_layers; layer_num++) {
      output = this->layers[layer_num].compute(output);
      //output = this->layers->at(layer_num)->compute(output);
   } 
   this->net_output_mat = output;
   return this->net_output_mat;
} 

// Getting Network I/O --------------------------------------------------------
shared_ptr<Matrix> Network::input() const {
   return this->net_input_mat; 
} 

shared_ptr<Matrix> Network::output() const {
   return this->net_output_mat; 
} 


// Getting Layer Info ---------------------------------------------------------
shared_ptr<vector<shared_ptr<Matrix>>> Network::layer_outputs(bool include_input) const {
   
   auto outputs = make_shared<vector<shared_ptr<Matrix>>>();
   
   if(include_input) {
      outputs->push_back(this->net_input_mat);
   } 

   for(int i = 0; i < this->num_layers; i++) {
      outputs->push_back(this->layers[i].output());
   } 
   
   return outputs;
}   

shared_ptr<vector<shared_ptr<Matrix>>> Network::layer_pre_bias_outputs(bool include_input) const {
   
   auto outputs = make_shared<vector<shared_ptr<Matrix>>>();
   
   if(include_input) {
      outputs->push_back(this->net_input_mat);
   } 

   for(int i = 0; i < this->num_layers; i++) {
      outputs->push_back(this->layers[i].pre_bias_output());
   } 
   
   return outputs;
}   

shared_ptr<vector<shared_ptr<Matrix>>> Network::layer_pre_act_outputs(bool include_input) const {
   
   auto outputs = make_shared<vector<shared_ptr<Matrix>>>();
   
   if(include_input) {
      outputs->push_back(this->net_input_mat);
   } 

   for(int i = 0; i < this->num_layers; i++) {
      outputs->push_back(this->layers[i].pre_act_output());
   } 
   
   return outputs;
}   



shared_ptr<vector<unsigned int>> Network::layer_sizes(bool include_input) const {
    
   auto outputs = make_shared<vector<unsigned int>>();
   
   if(include_input) {
      outputs->push_back(this->get_input_size());
   } 

   for(int i = 0; i < this->num_layers; i++) {
      outputs->push_back(this->layers[i].get_layer_size());
   } 
   
   return outputs;   
} 


shared_ptr<Matrix> Network::get_layer_output(unsigned int layer_num) const {
   return this->layers[layer_num].output();
} 
   
unsigned int Network::get_layer_size(unsigned int layer_num) const {
   return this->layers[layer_num].get_layer_size();
} 

// Getting Layer Parameters ---------------------------------------------------
shared_ptr<vector<shared_ptr<Matrix>>> Network::layer_weights() const {
   auto weights = make_shared<vector<shared_ptr<Matrix>>>();
   for(int i = 0; i < this->num_layers; i++) {
      //weights->push_back(this->layers->at(i)->get_weights());
      weights->push_back(this->layers[i].get_weights());
   } 
   return weights;
}   

shared_ptr<vector<shared_ptr<Matrix>>> Network::layer_biases() const {
   auto biases = make_shared<vector<shared_ptr<Matrix>>>();
   for(int i = 0; i < this->num_layers; i++) {
      //biases->push_back(this->layers->at(i)->get_biases());
      biases->push_back(this->layers[i].get_biases());
   } 
   return biases;
}   


shared_ptr<Matrix> Network::get_layer_weights(unsigned int layer_num) const {
   return this->layers[layer_num].get_weights();
} 
   
shared_ptr<Matrix> Network::get_layer_biases(unsigned int layer_num) const {
   return this->layers[layer_num].get_biases();
} 


void Network::print_network_state() const {
   printf("Current Network State\n");
   this->net_input_mat->print("Input");
   
   for(int i = 0; i < this->num_layers; i++) {
      string layer_name = "Layer " + to_string(i) + " Output";
      this->layers[i].output()->print(layer_name.c_str());
   }
} 






// Network Creation Functions -------------------------------------------------
shared_ptr<Network> make_full_network(unsigned int input_size, unsigned int num_layers,
                                        unsigned int layer_size) {
   vector<unsigned int> layer_sizes;
   vector<vector<float>> weights;
   vector<vector<float>> biases;

   for(int i = 0; i < num_layers; i++) {
      vector<float> layer_weights;
      vector<float> layer_biases;

      for(int j = 0; j < layer_size; j++) {
         
         if(i == 0) {
            for(int k = 0; k < input_size; k++) {
               layer_weights.push_back(1);
            } 
         } else {
            for(int k = 0; k < layer_size; k++) {
               layer_weights.push_back(1);
            } 
         }
         layer_biases.push_back(0);
      }

      weights.push_back(layer_weights);
      biases.push_back(layer_biases);
      layer_sizes.push_back(layer_size);
   } 

   return make_shared<Network>(input_size, sigmoid, layer_sizes, weights, biases);

} 

float rand_between(float min, float max) {
   return min + static_cast<float>(rand()) /(static_cast<float>(RAND_MAX/(max-min)));
} 

shared_ptr<Network> make_random_network(unsigned int input_size, unsigned int num_layers,
                                        unsigned int layer_size, unsigned int seed, bool sparse = false) {

   srand(seed);

   vector<unsigned int> layer_sizes;
   vector<vector<float>> weights;
   vector<vector<float>> biases;

   for(int i = 0; i < num_layers; i++) {
      vector<float> layer_weights;
      vector<float> layer_biases;

      for(int j = 0; j < layer_size; j++) {
         
         if(i == 0) {
            for(int k = 0; k < input_size; k++) {
               float w = rand_between(-1,1);
               if(sparse) w = round(w);
               layer_weights.push_back(w);
            } 
         } else {
            for(int k = 0; k < layer_size; k++) {
               float w = rand_between(-1,1);
               if(sparse) w = round(w);
               layer_weights.push_back(w);
            } 
         }
         float b = rand_between(-1,1);
         if(sparse) b = round(b);
         layer_biases.push_back(b);
      }

      weights.push_back(layer_weights);
      biases.push_back(layer_biases);
      layer_sizes.push_back(layer_size);
   } 

   return make_shared<Network>(input_size, sigmoid, layer_sizes, weights, biases);
};


shared_ptr<Network> default_network(NetworkType type) {
   switch(type) {
      case XOR: {
         vector<float> w1_data = {1,-1,-1,1};
         vector<float> b1_data = {0,0};

         vector<float> wo_data = {1,1};
         vector<float> bo_data = {0};

         vector<unsigned int> layer_sizes = {2,1};
         vector<vector<float>> weights = {w1_data, wo_data};
         vector<vector<float>> biases = {b1_data, bo_data};

         return make_shared<Network>(2, relu, layer_sizes, weights, biases);
      break;}  
      
      case OR: {
         vector<float> w1_data = {1,1,1,1};
         vector<float> b1_data = {0,-1};

         vector<float> wo_data = {1,-1};
         vector<float> bo_data = {0};

         vector<unsigned int> layer_sizes = {2,1};
         vector<vector<float>> weights = {w1_data, wo_data};
         vector<vector<float>> biases = {b1_data, bo_data};

         return make_shared<Network>(2, relu, layer_sizes, weights, biases);
      break;}  
      
      case AND: {
         vector<float> wo_data = {1,1};
         vector<float> bo_data = {-1};

         vector<unsigned int> layer_sizes = {1};
         vector<vector<float>> weights = {wo_data};
         vector<vector<float>> biases = {bo_data};

         return make_shared<Network>(2, relu, layer_sizes, weights, biases);
      break;}  
      
      case NOT: {
         vector<float> wo_data = {-1};
         vector<float> bo_data = {1};

         vector<unsigned int> layer_sizes = {1};
         vector<vector<float>> weights = {wo_data};
         vector<vector<float>> biases = {bo_data};

         return make_shared<Network>(1, relu, layer_sizes, weights, biases);
      break;}  

      case RAND_4X4: return  make_random_network(2, 4, 4, time(NULL));
      case SPARSE_RAND_4X4: return  make_random_network(2, 4, 4, time(NULL), true);

      case SEEDED_4X4: return  make_random_network(2, 4, 4, 5);
      case SEEDED_8X8: return  make_random_network(2, 8, 8, 5);
      case SEEDED_LARGE: return  make_random_network(2, 6, 50, 5);
      case SEEDED_HUGE: return  make_random_network(2, 8, 500, 5);
      
      case FULL_4X4: return make_full_network(2, 4, 4);
   } 
} 

