#version 450 core

layout(location = 0) out vec4 FragColor;

in vec3 Normal;
in vec3 Position;

uniform vec3 cameraPosition;
uniform samplerCube skybox;

void main()
{    
    vec3 I = normalize(Position - cameraPosition);
    vec3 reflection = reflect(I, normalize(Normal));


    FragColor = vec4(texture(skybox, reflection).rgb, 1.0f);
}