#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTex;
layout(location = 5) in vec3 aTangent;
layout(location = 6) in vec3 instanceMatrix;

out vec3 out_position;
out vec3 out_normal;
out vec2 out_tex;
out vec3 pointLight_Position;
out vec3 tangentLightDirection;
out vec3 tangentLightPos;
out vec3 tangentViewPos;
out vec3 tangentFragPos;
out vec4 fragPosLightSpace;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 inv_model;
uniform vec3 cameraPosition;
uniform vec3 pointLightPosition;
uniform mat4 lightSpaceMatrix;
uniform vec3 lightDirection;

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
    out_position = vec3(model * vec4(aPos, 1.0));
    pointLight_Position = pointLightPosition;


    //tangent TBN matrix
    mat3 normalMatrix = mat3(inv_model);
    vec3 T = normalize(normalMatrix * aTangent);
    vec3 N = normalize(normalMatrix * aNormal);
    //calculate bitangent
    T = normalize(T-dot(T,N)* N);
    vec3 B = normalize(cross(N,T));
    mat3 TBN = transpose(mat3(T,B,N));

    tangentFragPos = TBN * out_position;
    tangentViewPos = TBN * cameraPosition;
    tangentLightDirection = TBN * lightDirection;
    tangentLightPos = TBN * pointLightPosition;

    mat4 pvm = projection * view * model;
    gl_Position = pvm * translate(instanceMatrix) * vec4(aPos, 1.0);
    out_tex = aTex;
    out_normal = vec3(inv_model * vec4(N, 1.0));
    fragPosLightSpace = lightSpaceMatrix * vec4(out_position, 1.0);
}