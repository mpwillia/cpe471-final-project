
#pragma once
#ifndef KEYBINDINGS_HPP
#define KEYBINDINGS_HPP

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <string>

// Keybindings
enum Keybinds {
   // Camera Movement
   CAM_FORWARD  = GLFW_KEY_W,
   CAM_BACKWARD = GLFW_KEY_S,
   CAM_LEFT     = GLFW_KEY_A,
   CAM_RIGHT    = GLFW_KEY_D,
   CAM_UP       = GLFW_KEY_SPACE,
   CAM_DOWN     = GLFW_KEY_X,

   CAM_FASTER   = GLFW_KEY_F,
   CAM_FAST     = GLFW_KEY_C,

   // Network Input
   NET_CASE_1 = GLFW_KEY_1,
   NET_CASE_2 = GLFW_KEY_2,
   NET_CASE_3 = GLFW_KEY_3,
   NET_CASE_4 = GLFW_KEY_4,

   // Animation Speed
   ANI_FASTER = GLFW_KEY_EQUAL,
   ANI_SLOWER = GLFW_KEY_MINUS,
   ANI_STOP   = GLFW_KEY_0,
   ANI_RESET  = GLFW_KEY_BACKSLASH,

   // Pulse Movement Settings
   MOVE_TYPE_LINEAR = GLFW_KEY_L,
   MOVE_TYPE_COS = GLFW_KEY_K,
   MOVE_TYPE_SIN = GLFW_KEY_J,
  
   MOVE_EXP_STEP_UP       = GLFW_KEY_Y,
   MOVE_EXP_CUBE          = GLFW_KEY_U,
   MOVE_EXP_SQR           = GLFW_KEY_I,
   MOVE_EXP_NONE          = GLFW_KEY_O,
   MOVE_EXP_THREE_QUARTER = GLFW_KEY_P,
   MOVE_EXP_HALF          = GLFW_KEY_LEFT_BRACKET,
   MOVE_EXP_STEP_DOWN     = GLFW_KEY_RIGHT_BRACKET,
};

static string describe(Keybinds bind) {
   switch(bind) {
      case CAM_FORWARD  : return "Forward ";
      case CAM_BACKWARD : return "Backward";
      case CAM_LEFT     : return "Left    ";
      case CAM_RIGHT    : return "Right   ";
      case CAM_UP       : return "Up      ";
      case CAM_DOWN     : return "Down    ";

      case CAM_FASTER : return "Super Speed (x3)";
      case CAM_FAST   : return "Warp Speed  (x9)";
      
      case NET_CASE_1 : return "Netowrk Input 1";
      case NET_CASE_2 : return "Netowrk Input 2";
      case NET_CASE_3 : return "Netowrk Input 3";
      case NET_CASE_4 : return "Netowrk Input 4";

      case ANI_FASTER : return "Speed Up   ";
      case ANI_SLOWER : return "Slow Down  ";
      case ANI_STOP   : return "Stop       ";
      case ANI_RESET  : return "Reset Speed";
      
      case MOVE_TYPE_LINEAR : return "Linear";
      case MOVE_TYPE_COS    : return "Cosine";
      case MOVE_TYPE_SIN    : return "Sin   ";
      
      case MOVE_EXP_STEP_UP       : return "Increase";
      case MOVE_EXP_CUBE          : return "Cubic   ";
      case MOVE_EXP_SQR           : return "Squared ";
      case MOVE_EXP_NONE          : return "None    ";
      case MOVE_EXP_THREE_QUARTER : return "3/4     ";
      case MOVE_EXP_HALF          : return "1/2     ";
      case MOVE_EXP_STEP_DOWN     : return "Decrease";
      default:
         return "TODO";
   } 
}

static string keyname(int key) {
   switch(key) {
      case GLFW_KEY_SPACE: return "Space";
      case GLFW_KEY_EQUAL: return "+";
      case GLFW_KEY_MINUS: return "-";
      default:
         return string(glfwGetKeyName(key,0));
   }  
} 

const char* one_tab_fmt = "   %s\n";
const char* two_tab_fmt = "      %s\n";

static void print_keybind(Keybinds bind, const char* fmt = one_tab_fmt) {
   string bind_info = describe(bind) + " : " + keyname(bind);
   printf(fmt, bind_info.c_str());
} 

static void print_camera_keybindings() {
   printf("Camera Movement\n"); 
   print_keybind(CAM_FORWARD);
   print_keybind(CAM_BACKWARD);
   print_keybind(CAM_LEFT);
   print_keybind(CAM_RIGHT);
   print_keybind(CAM_UP);
   print_keybind(CAM_DOWN);
   printf("\n");
   print_keybind(CAM_FASTER);
   print_keybind(CAM_FAST);
   printf("\n");
} 

static void print_net_input_keybindings() {
   printf("Network Input\n");
   printf(one_tab_fmt, "Change Input : [1-4]");
   printf("\n");
} 

static void print_animation_keybindings() {
   printf("Animation Settings\n");
   printf("   Animation Speed\n");
   print_keybind(ANI_FASTER, two_tab_fmt);
   print_keybind(ANI_SLOWER, two_tab_fmt);
   print_keybind(ANI_STOP, two_tab_fmt);
   print_keybind(ANI_RESET, two_tab_fmt);
   printf("   Movement Type\n");
   print_keybind(MOVE_TYPE_LINEAR, two_tab_fmt);
   print_keybind(MOVE_TYPE_COS, two_tab_fmt);
   print_keybind(MOVE_TYPE_SIN, two_tab_fmt);
   printf("   Movement Exponent\n");
   print_keybind(MOVE_EXP_STEP_UP, two_tab_fmt);
   print_keybind(MOVE_EXP_CUBE, two_tab_fmt);
   print_keybind(MOVE_EXP_SQR, two_tab_fmt);
   print_keybind(MOVE_EXP_NONE, two_tab_fmt);
   print_keybind(MOVE_EXP_THREE_QUARTER, two_tab_fmt);
   print_keybind(MOVE_EXP_HALF, two_tab_fmt);
   print_keybind(MOVE_EXP_STEP_DOWN, two_tab_fmt);
   printf("\n");
} 

static void print_keybindings() {
   printf("--- Controls ---\n");
   print_camera_keybindings();
   print_net_input_keybindings();
   print_animation_keybindings();
   printf("\n");
} 

#endif

