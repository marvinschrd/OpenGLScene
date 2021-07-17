#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 5) in vec3 aTangent;

out vec3 out_position;
out vec3 out_normal;
out vec2 out_tex;
out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 inv_model;
uniform vec3 view_pos;


void main()
{
    
    vec3 light_position = vec3(0.0,7.0,3.0);
    out_position = vec3(model * vec4(aPos, 1.0));


    //tangent TBN matrix
    mat3 normalMatrix = mat3(inv_model);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    //calculate bitangent
    T = normalize(T-dot(T,N)* N);
    vec3 B = normalize(cross(N,T));

    mat3 TBN = transpose(mat3(T,B,N));

   // tangentLightPos = TBN * light_position;
    tangentFragPos = TBN * out_position;
    tangentViewPos = TBN * view_pos;
    vec3 lightDirection = vec3(0.0,0.0,1.0);
    tangentLightPos = TBN * lightDirection;


    mat4 pvm = projection * view * model;
    gl_Position = pvm * vec4(aPos, 1.0);
    out_tex = aTex;
    out_normal = vec3(inv_model * vec4(N, 1.0));
    
}