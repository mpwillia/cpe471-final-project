/* Lab 6 base code - transforms using matrix stack built on glm 
	CPE 471 Cal Poly Z. Wood + S. Sueda
*/
#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "GLSL.h"
#include "Program.h"
#include "MatrixStack.h"
#include "Shape.h"

// value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;
using namespace glm;

/* to use glee */
#define GLEE_OVERWRITE_GL_FUNCTIONS
#include "glee.hpp"

#include "Materials.hpp"
#include "Lighting.hpp"

#include "Matrix.hpp"
#include "Network.hpp"
#include "NetworkRenderer.hpp"
#include "Keybindings.hpp"

#include <array>
#include <limits>

GLFWwindow *window; // Main application window
string RESOURCE_DIR = ""; // Where the resources are loaded from

// Define our window size
int g_width = 1280;
int g_height = 960;

vector<shared_ptr<NetworkRenderer>> networks;

vector<float> case1 = {0,0};
vector<float> case2 = {0,1};
vector<float> case3 = {1,0};
vector<float> case4 = {1,1};
vector<vector<float>> test_cases = {case1, case2, case3, case4};
unsigned int case_idx = 1;

RenderSettings net_render_settings = default_render_settings;
const float net_ani_step = 0.05;

// Shading and Materials ------------------------------------------------------
// Using Phong as our main shader
shared_ptr<Program> phong;

// Global Lighting Information ------------------------------------------------
const float global_brightness = 1.0;
const float default_ambient_scale = 0.25;

// Define our objects ---------------------------------------------------------
shared_ptr<Shape> bunny;
shared_ptr<Shape> sphere;
shared_ptr<Shape> icosphere;
shared_ptr<Shape> cube;
shared_ptr<Shape> head;
shared_ptr<Shape> connection;

// Virtual Camera -------------------------------------------------------------
float theta = M_PI; // yaw
float phi = -1*M_PI/4; // pitch
vec3 cam_eye = vec3(90,150,-20);
vec3 cam_up = vec3(0,1,0);

vec3 cam_move = vec3(0,0,0);
float cam_speed = 0.5;
const float cam_speed_default = 0.5;
float speed_mult = 3.0;

// Defines how close to 90 degrees the camera can look up or down
const float phi_cap = 0.99; 

// Defines the sensitivity for mouse movements
// Higher values for |sens| mean faster movement
const float sens = 10.0; 
const float scroll_offset_div = 1000.0;
const float mouse_offset_div = 10000.0;

// Defines the maximum velocity the camera can rotate
const double clamp_val = 175.0;


const float far_plane = 300.0;

// Stores the cursor position
double cursor_x = g_width / 2.0;
double cursor_y = g_height / 2.0;


static void error_callback(int error, const char *description)
{
	cerr << description << endl;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

   if(action == GLFW_PRESS) {
      switch(key) {
         case CAM_FORWARD: cam_move.z += -1; break; // forward
         case CAM_BACKWARD: cam_move.z += 1; break; // backward
         case CAM_LEFT: cam_move.x += -1; break; // left
         case CAM_RIGHT: cam_move.x += 1; break; // right
         case CAM_UP: cam_move.y += 1; break; // up
         case CAM_DOWN: cam_move.y += -1; break; // down
         
         case CAM_FASTER: cam_speed *= speed_mult; break;
         case CAM_FAST: cam_speed *= speed_mult * speed_mult; break;

         // Network Inputs
         case NET_CASE_1: case_idx = 0; break;
         case NET_CASE_2: case_idx = 1; break;
         case NET_CASE_3: case_idx = 2; break;
         case NET_CASE_4: case_idx = 3; break;
      
         // Pulse Movement Settings
         case MOVE_TYPE_LINEAR: net_render_settings.move_type = LINEAR; break;
         case MOVE_TYPE_COS: net_render_settings.move_type = COS; break;
         case MOVE_TYPE_SIN: net_render_settings.move_type = SIN; break;
         
         case MOVE_EXP_STEP_UP       : net_render_settings.move_exp += 1.0; break;
         case MOVE_EXP_CUBE          : net_render_settings.move_exp = 3.0; break;
         case MOVE_EXP_SQR           : net_render_settings.move_exp = 2.0; break; 
         case MOVE_EXP_NONE          : net_render_settings.move_exp = 1.0; break; 
         case MOVE_EXP_THREE_QUARTER : net_render_settings.move_exp = 0.75; break;
         case MOVE_EXP_HALF          : net_render_settings.move_exp = 0.50; break; 
         case MOVE_EXP_STEP_DOWN     : net_render_settings.move_exp -= 0.25; break; 

         // Animation Speed
         case ANI_FASTER: net_render_settings.animation_speed += net_ani_step; break;
         case ANI_SLOWER: net_render_settings.animation_speed -= net_ani_step; break;
         case ANI_STOP: net_render_settings.animation_speed = 0.0; break;
         case ANI_RESET: 
            net_render_settings.animation_speed = default_render_settings.animation_speed; 
            break;
      }   
   } else if (action == GLFW_RELEASE) {
       switch(key) {
         case CAM_FORWARD: cam_move.z -= -1; break; // forward
         case CAM_BACKWARD: cam_move.z -= 1; break; // backward
         case CAM_LEFT: cam_move.x -= -1; break; // left
         case CAM_RIGHT: cam_move.x -= 1; break; // right
         case CAM_UP: cam_move.y -= 1; break; // up
         case CAM_DOWN: cam_move.y -= -1; break; // down

         case CAM_FASTER: cam_speed = cam_speed_default; break;
         case CAM_FAST: cam_speed = cam_speed_default; break;
      }   
   } else if (action == GLFW_REPEAT) {
      switch(key) {
         case ANI_FASTER: net_render_settings.animation_speed += net_ani_step; break;
         case ANI_SLOWER: net_render_settings.animation_speed -= net_ani_step; break;
      } 
   } 
}


static void mouse_callback(GLFWwindow *window, int button, int action, int mods)
{
   double posX, posY;
   if (action == GLFW_PRESS) {
      glfwGetCursorPos(window, &posX, &posY);
      cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
	}
}

static void update_camera_rotation(double xoffset, double yoffset, double offset_div) {
   xoffset = std::max(std::min(xoffset, clamp_val), -clamp_val);
   yoffset = std::max(std::min(yoffset, clamp_val), -clamp_val);

   theta += (xoffset / offset_div) * sens;
   phi += (yoffset / offset_div) * sens;
   
   if(phi > M_PI/2.0 * phi_cap) {
      phi = M_PI/2.0 * phi_cap; 
   } else if(phi < -M_PI/2.0 * phi_cap) {
      phi = -M_PI/2.0 * phi_cap; 
   } 
} 

static void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
   update_camera_rotation(xpos - cursor_x, -(ypos - cursor_y), mouse_offset_div);

   cursor_x = xpos;
   cursor_y = ypos;
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
   update_camera_rotation(-xoffset, yoffset, scroll_offset_div);
}

static void resize_callback(GLFWwindow *window, int width, int height) {
   g_width = width;
   g_height = height;
   glViewport(0, 0, width, height);
}

static shared_ptr<Shape> load_shape(const std::string &mesh_name) {
   string obj_dir = RESOURCE_DIR + "objs/";
   shared_ptr<Shape> shape = make_shared<Shape>();
   shape->loadMesh(obj_dir + mesh_name);
   shape->resize();
   shape->init();
   return shape;
} 

static void init()
{
	GLSL::checkVersion();

   // Define OpenGL Parameters ------------------------------------------------
   //vec3 clear_color = vec3(0.12f, 0.34f, 0.56f);
   vec3 clear_color = vec3(0.13f, 0.13f, 0.14f);
   float bg_brightness = 0.2 * global_brightness;

	// Set background color.
   clear_color *= bg_brightness;
	glClearColor(clear_color.x, clear_color.y, clear_color.z, 1.0f);

	// Enable z-buffer test.
	glEnable(GL_DEPTH_TEST);

   // Enable Backface Culling.
   glEnable(GL_CULL_FACE);  


   // Load Object Meshes ------------------------------------------------------
	// Initialize meshs.
   bunny = load_shape("bunny.obj");
   sphere = load_shape("sphere.obj");
   icosphere = load_shape("IcoSphere.obj");
   cube = load_shape("cube.obj");
   head = load_shape("Nefertiti-10K.obj");
   connection = load_shape("connection.obj");

   // Load Shaders ------------------------------------------------------------
   string shader_dir = RESOURCE_DIR + "shaders/";
	// Initialize the GLSL program.
   phong = make_shared<Program>();
	phong->setVerbose(true);
	phong->setShaderNames(shader_dir + "phong_vert.glsl", shader_dir + "phong_frag.glsl");
	phong->init();
	phong->addAttribute("vertPos");
	phong->addAttribute("vertNor");
   // Transformation Matrices
	phong->addUniform("P");
	phong->addUniform("V");
	phong->addUniform("M");
   // Material Properties
   phong->addUniform("MatAmb");
   phong->addUniform("MatDif");
   phong->addUniform("MatSpec");
   phong->addUniform("MatEmis");
   phong->addUniform("shine");
   // Lighting Info
   phong->addUniform("num_lights");
   phong->addUniform("light_positions");
   phong->addUniform("light_colors");
   phong->addUniform("light_falloffs");
   phong->addUniform("light_brightness");

   phong->addUniform("ambient_scale");
   phong->addUniform("global_brightness");
   
   phong->addUniform("size");

   
   // Create Network
   auto make_net = [](NetworkType type) {
      return make_shared<NetworkRenderer>(default_network(type), sphere, connection, phong);
   };
   
   networks.push_back(make_net(XOR));
   //networks.push_back(make_net(RAND_4X4));
   //networks.push_back(make_net(FULL_4X4));

}

static void render()
{
	// Get current frame buffer size.
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Clear framebuffer.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//Use the matrix stack for Lab 6
   float aspect = width/(float)height;

   // Create the matrix stacks - please leave these alone for now
   auto P = make_shared<MatrixStack>();
   auto M = make_shared<MatrixStack>();
   auto V = make_shared<MatrixStack>();
   // Apply perspective projection.
   P->pushMatrix();
   P->perspective(45.0f, aspect, 0.01f, far_plane);
   
   // Setup our View
   V->pushMatrix();
   V->loadIdentity();
   
   float r = 1.0;
   vec3 look_at_point = vec3(r * cos(phi) * cos(theta),
                             r * sin(phi),
                             r * cos(phi) * cos(M_PI/2.0 - theta));
   vec3 gaze = look_at_point;
   look_at_point += cam_eye;
   
   // compute camera coordinate frame
   vec3 view_w = normalize(-gaze);
   vec3 view_u = normalize(cross(cam_up, view_w));
   vec3 view_v = normalize(cross(view_w, view_u));
   
   vec3 move = cam_speed * (cam_move.x * view_u + cam_move.z * view_w + cam_move.y * view_v);
   look_at_point += move;
   cam_eye += move; 

   V->lookAt(cam_eye, look_at_point, cam_up);
   
   shared_ptr<Program> prog = phong;
   
   vec3 net_spacing = vec3(0, 0, 100);
   vec3 net_base_pos = vec3(-20, -5, 15);

   auto global_lighting = make_shared<Lighting>();
   global_lighting->set_global_brightness(global_brightness);

   // Place Objects Into World
   M->pushMatrix();
      M->loadIdentity();

      // Draw Sample Networks
      auto test_case = test_cases[case_idx % test_cases.size()];
      
      vec3 pos = net_base_pos - (net_spacing * (float)(networks.size() / 2.0));
      for(auto &net : networks) {
         net->set_input(test_case);
         net->set_render_settings(net_render_settings);
         net->render(pos, default_ambient_scale, global_brightness, P,V,M);
         global_lighting->add_lights(net->get_lighting());
         pos += net_spacing;
      } 

      //case_idx = (unsigned int)(glfwGetTime() / 5.0);

      // Draw a Ground Plane
      M->pushMatrix();
         M->translate(vec3(0,net_base_pos.y-10,0));
         M->scale(vec3(1000,0.1,1000));
         prog->bind();

         glUniform1f(prog->getUniform("ambient_scale"), default_ambient_scale);
         glUniform1f(prog->getUniform("size"), 0);

         global_lighting->load_lights(prog);
         load_material(prog, flat_grey_no_spec);

         glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
         glUniformMatrix4fv(prog->getUniform("V"), 1, GL_FALSE, value_ptr(V->topMatrix()));
         glUniformMatrix4fv(prog->getUniform("P"), 1, GL_FALSE, value_ptr(P->topMatrix()));  

         cube->draw(prog);
         prog->unbind();
      M->popMatrix();

   M->popMatrix();
   V->popMatrix();
   P->popMatrix();
}

void xor_test() {
   // 'Network'
   vector<float> w1_data = {1,-1,-1,1};
   auto w1 = make_shared<Matrix>(2,2,w1_data);
   float b1_data[2] = {0,0};
   auto b1 = make_shared<Matrix>(1,2,b1_data);

   float wo_data[2] = {1,1};
   auto wo = make_shared<Matrix>(2,1,wo_data);
   float bo_data[1] = {0};
   auto bo = make_shared<Matrix>(1,1,bo_data);
   
   auto act_func = relu;
   auto run_net = [=] (shared_ptr<Matrix> input) {
      return input->dot(w1)->add(b1)->apply(act_func)->dot(wo)->add(bo)->apply(act_func);
   };

   // Tests
   float case1[2] = {0,0};
   auto input = make_shared<Matrix>(1,2,case1);
   auto result = run_net(input);
   result->print("XOR test on (0,0) | expecting 0");

   float case2[2] = {0,1};
   input = make_shared<Matrix>(1,2,case2);
   result = run_net(input);
   result->print("XOR test on (0,1) | expecting 1");

   float case3[2] = {1,0};
   input = make_shared<Matrix>(1,2,case3);
   result = run_net(input);
   result->print("XOR test on (1,0) | expecting 1");

   float case4[2] = {1,1};
   input = make_shared<Matrix>(1,2,case4);
   result = run_net(input);
   result->print("XOR test on (1,1) | expecting 0");

} 

void xor_layer_test() {
   
   printf("Running XOR Layer Test\n");

   vector<float> w1_data = {1,-1,-1,1};
   vector<float> b1_data = {0,0};
   auto hidden_layer = make_shared<Layer>(2, 2, relu, w1_data, b1_data);

   vector<float> wo_data = {1,1};
   vector<float> bo_data = {0};
   auto output_layer = make_shared<Layer>(1, 2, relu, wo_data, bo_data);
    
   auto run_net = [=] (vector<float> vec_input) {
      int input_size = static_cast<int>(vec_input.size());
      auto input_mat = make_shared<Matrix>(1, input_size, vec_input);
      return output_layer->compute(hidden_layer->compute(input_mat));
   };

   vector<float> case1 = {0,0};
   vector<float> case2 = {0,1};
   vector<float> case3 = {1,0};
   vector<float> case4 = {1,1};
   
   run_net(case1)->print("XOR test on (0,0) | expecting 0");
   run_net(case2)->print("XOR test on (0,1) | expecting 1");
   run_net(case3)->print("XOR test on (1,0) | expecting 1");
   run_net(case4)->print("XOR test on (1,1) | expecting 0");

} 

void xor_network_test() {
   printf("Running XOR Network Test\n");
   auto network = default_network(XOR);
   auto run_net = [=] (vector<float> vec_input) {
      return network->compute(vec_input);
   };

   vector<float> case1 = {0,0};
   vector<float> case2 = {0,1};
   vector<float> case3 = {1,0};
   vector<float> case4 = {1,1};
   
   run_net(case1)->print("XOR test on (0,0) | expecting 0");
   run_net(case2)->print("XOR test on (0,1) | expecting 1");
   run_net(case3)->print("XOR test on (1,0) | expecting 1");
   run_net(case4)->print("XOR test on (1,1) | expecting 0");
} 

void matrix_tests() {
   //xor_test();
   //xor_layer_test();
   xor_network_test();
} 

int main(int argc, char **argv)
{
   
   //matrix_tests();

	if(argc < 2) {
		cout << "Please specify the resource directory." << endl;
		return 0;
	}
	RESOURCE_DIR = argv[1] + string("/");

	// Set error callback.
	glfwSetErrorCallback(error_callback);
	// Initialize the library.
	if(!glfwInit()) {
		return -1;
	}
   //request the highest possible version of OGL - important for mac
   glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
   glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
   glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// Create a windowed mode window and its OpenGL context.
	window = glfwCreateWindow(g_width, g_height, "Michael Williams", NULL, NULL);
	if(!window) {
		glfwTerminate();
		return -1;
	}
   
   int xpos, ypos;
   glfwGetWindowPos(window, &xpos, &ypos);
   glfwSetWindowPos(window, xpos + 100, ypos);

	// Make the window's context current.
	glfwMakeContextCurrent(window);
	// Initialize GLEW.
	glewExperimental = true;
	if(glewInit() != GLEW_OK) {
		cerr << "Failed to initialize GLEW" << endl;
		return -1;
	}
	//weird bootstrap of glGetError
   glGetError();
	cout << "OpenGL version: " << glGetString(GL_VERSION) << endl;
   cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;

	// Set vsync.
	glfwSwapInterval(1);
	// Set keyboard callback.
	glfwSetKeyCallback(window, key_callback);
   //set the mouse call back
   //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
   glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
   glfwGetCursorPos(window, &cursor_x, &cursor_y);
   glfwSetCursorPosCallback(window, cursor_position_callback);
   glfwSetMouseButtonCallback(window, mouse_callback);
   //set the window resize call back
   glfwSetFramebufferSizeCallback(window, resize_callback);

   glfwSetScrollCallback(window, scroll_callback);

	// Initialize scene. Note geometry initialized in init now
	init();

   print_keybindings();

   double prev_time = glfwGetTime();
   const double poll_rate_s = 1.0;
   int num_frames = 0;

   // fps stat tracking
   double min_fps = numeric_limits<double>::infinity();
   double max_fps = -min_fps;

	// Loop until the user closes the window.
	while(!glfwWindowShouldClose(window)) {
		// Render scene.
		render();
		// Swap front and back buffers.
		glfwSwapBuffers(window);
		// Poll for and process events.
		glfwPollEvents();

      double cur_time = glfwGetTime();
      num_frames++; // count frames rendered

      if ( cur_time - prev_time >= poll_rate_s) { 
         // Print Info Every Second
         // Report FPS and other info every second
         
         printf("\n\n");
         print_keybindings();
         print_render_settings(net_render_settings);
         printf("\n");
         
         double mspf = 1000.0 / double(num_frames);
         double fps = double(num_frames) / poll_rate_s;
         
         if(fps < min_fps) min_fps = fps;
         if(fps > max_fps) max_fps = fps;
         
         // frames / second  ==> second / frames ==> milliseconds / frame

         double min_mspf = 1000.0 / min_fps;
         double max_mspf = 1000.0 / max_fps;
         printf("Min %7.3f ms/frame [%3d fps]\n", min_mspf, (int)min_fps);
         printf("Max %7.3f ms/frame [%3d fps]\n", max_mspf, (int)max_fps);

         printf("Framerate %7.3f ms/frame [%3d fps]\n", mspf, (int)fps);

         num_frames = 0;
         prev_time += 1.0;
      }
	}

	// Quit program.
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}
