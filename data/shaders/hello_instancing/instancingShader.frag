#version 450 core

layout(location = 0) out vec4 FragColor;

in vec2 out_tex;


uniform sampler2D texture_diffuse1;
//uniform sampler2D texture_specular1;


const float ambient_strengh = 0.1;
const float specular_strength = 0.8;
const float specular_pow = 256;
const vec3 light_color = vec3(1.0, 1.0, 1.0);
const vec3 light_color2 = vec3(1.0, 1.0, 1.0);


void main()
{
   
    FragColor = texture(texture_diffuse1, out_tex);
}