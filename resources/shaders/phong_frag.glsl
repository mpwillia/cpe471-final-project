#version 330 core 
in vec3 frag_nor_in;
in vec3 frag_pos;

uniform mat4 V;
uniform vec3 MatAmb;
uniform vec3 MatDif;
uniform vec3 MatSpec;
uniform vec3 MatEmis;
uniform float shine;

uniform int num_lights;
uniform vec3 light_positions[10];
uniform vec3 light_colors[10];
uniform vec3 light_falloffs[10];
uniform float light_brightness[10];

uniform float ambient_scale;
uniform float global_brightness;

uniform float size;

out vec4 color;

void main()
{
   bool atten_debug = false;
   bool num_lights_debug = false;
   float debug_step = 1.0 / num_lights;

   // Normalize the interpolated Normals
   vec3 frag_nor = normalize(frag_nor_in);
   
   // Extract the camera position from the view transformation matrix
   vec3 eye = vec3(V[3][0], V[3][1], V[3][2]);

   // Compute our view vector
   vec3 view_vec = normalize(frag_pos - eye);


   // Begin computing our reflected color
   vec3 refl_color = MatAmb * ambient_scale;

   for(int i = 0; i < num_lights && i < 10; i++) {

      if(num_lights_debug)
         refl_color += vec3(0,0,debug_step);

      vec3 light_color = light_colors[i];

      vec3 light_pos = light_positions[i];
      vec3 falloff = light_falloffs[i];
      float brightness = light_brightness[i];

      // Distance Attenuation
      float d = distance(frag_pos, light_pos);

      float dist_atten = 1.0 / (falloff[0] + falloff[1]*d + falloff[2]*d*d);

      // Skip lights that will have basically no impact on the lighting
      // No idea if this will really "optimize" anything but hey
      if(dist_atten < 0.01) {
         if(atten_debug)
            refl_color += vec3(0.0,debug_step,0);
         continue;
      } else {
         if(atten_debug)
            refl_color += vec3(debug_step,0.0,0);
      } 

      vec3 light_vec = normalize(light_pos - frag_pos);
      float l_dot = dot(frag_nor, light_vec);
      
      // Diffuse
      vec3 diffuse = MatDif * max(l_dot, 0) * light_color;

      // Specular
      //r=d−2(d⋅n)n
      //vec3 refl_vec = normalize(light_vec - 2*(max(dot(light_vec, frag_nor), 0))*frag_nor);
      vec3 refl_vec = reflect(light_vec, frag_nor);
      vec3 specular = MatSpec * pow(max(dot(refl_vec, view_vec), 0), shine) * light_color;
      
      //float emis_atten = 1.0 / max(pow(d-light_inside_dist, 2),1.0);

      //vec3 emissive = MatEmis * max(-l_dot,0.5) * emis_atten;
      
      vec3 emissive = vec3(0);
      if(d < size * 2.0) {
         
         float ed = max(d-(size+0.5),0.0);
         float emis_atten = 1.0 / (1.0 + 0.5 *ed + 0.5 *ed*ed + 1.0 *ed*ed*ed);
         
         /*
         if(emis_atten < 0.001) {
            emis_atten = 0; 
         }*/ 
         emissive = MatEmis * emis_atten;
      } 

      /*
      if(d >= light_inside_dist) {
         emissive *= 0.0;
      }
      */
      refl_color += dist_atten * brightness * (diffuse + specular + emissive);
   } 
   
   refl_color *= global_brightness;
   color = vec4(refl_color, 1.0);
}


