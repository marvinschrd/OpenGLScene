#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 6) in vec3 instanceMatrix;
//layout(location = 5) in vec3 aTangent;

out vec3 out_position;
out vec3 out_normal;
out vec2 out_tex;



uniform mat4 view;
uniform mat4 projection;
uniform mat4 inv_model;
uniform vec3 view_pos;

mat4 translate(vec3 v)
{
    return mat4(
        vec4(1.0,0.0,0.0,0.0),
        vec4(0.0,1.0,0.0,0.0),
        vec4(0.0,0.0,1.0,0.0),
        vec4(v, 1.0)
    );
}

void main()
{
    
    vec3 light_position = vec3(0.0,7.0,3.0);
    vec4 pos = projection * view * translate(instanceMatrix) * vec4(aPos, 1.0f);
   gl_Position = pos;
   out_tex = aTex;
}