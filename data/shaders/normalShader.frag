#version 450 core

layout(location = 0) out vec4 FragColor;

in vec2 out_tex;
in vec3 tangentFragPos;
in vec3 tangentLightPos;
in vec3 tangentViewPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

const float ambient_strengh = 0.1;
const float specular_strength = 0.8;
const float specular_pow = 256;
const vec3 light_color = vec3(1.0, 1.0, 1.0);
const vec3 light_color2 = vec3(1.0, 1.0, 1.0);


void main()
{
    // Compute ambiant light.
    vec3 ambient = ambient_strengh * light_color;

    // Compute diffuse light
    vec3 normal = texture(texture_normal1, out_tex).rgb;
    normal = normalize(normal*2.0-1.0);

   // vec3 light_direction = normalize(tangentLightPos - tangentFragPos);
    vec3 light_direction = tangentLightPos;
    vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;
    
    // Compute specular light.
    vec3 reflection_direction = reflect(-light_direction,normal);
    vec3 halfWayDir = normalize(light_direction + viewDir);
    float spec = pow(max(dot(viewDir, reflection_direction), 0.0), specular_pow);
    vec3 specular = specular_strength * spec * light_color2;

    // Final lighting.
    vec3 result_diffuse_ambient = 
        (ambient + diffuse + specular) * texture(texture_diffuse1, out_tex).rgb; 
    // vec3 result_diffuse_ambient = 
    //        (ambient + diffuse + specular) * mix(texture(textureDiffuse, out_tex).rgb,texture(textureDiffuse2, out_tex).rgb,0.7);
    vec3 result_specular =
        specular * texture(texture_specular1, out_tex).r;
    FragColor = vec4(result_diffuse_ambient + result_specular, texture(texture_diffuse1, out_tex).a);
}