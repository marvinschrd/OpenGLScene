#version 450 core

layout(location = 0) out vec4 FragColor;

in vec2 out_tex;
in vec3 out_position;
in vec3 light_position;
in vec3 viewPos;

in vec3 tangentFragPos;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec3 tangentLightDir;


//struct Light {
//    vec3 position;
//  
//    vec3 ambient;
//    vec3 diffuse;
//    vec3 specular;
//	
//    float constant;
//    float linear;
//    float quadratic;
//};

uniform float constant;
uniform float linear;
uniform float quadratic;
uniform float far_plane;


uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform samplerCube depthMap;
//uniform Light light;

const float ambient_strengh = 0.5;
const float specular_strength = 0.8;
const float specular_pow = 256;
const vec3 light_color = vec3(50.0, 3.0, 5.0);
const vec3 light_color2 = vec3(1.0, 1.0, 1.0);

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float ShadowCaclulations(vec3 fragPos)
{
    vec3 fragToLight = fragPos - light_position;
    float currentDepth = length(fragToLight);

    float shadow = 0.0;
    float bias = 0.05;
    int samples = 20;
    float viewDistance = length(viewPos - fragPos);
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(depthMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    return shadow;
}


void main()
{
    vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
    vec3 color = texture(texture_diffuse1,out_tex).rgb;
    // Compute ambient light.
    vec3 ambient = ambient_strengh * color * light_color2;

    // Compute diffuse light
    vec3 normal = texture(texture_normal1, out_tex).rgb;
    normal = normalize(normal*2.0-1.0);

    vec3 light_direction = normalize(tangentLightPos - tangentFragPos);
    //vec3 light_direction = tangentLightPos;
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;
    
    // Compute specular light.
   // vec3 reflection_direction = reflect(-light_direction,normal);
    vec3 halfWayDir = normalize(light_direction + viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), specular_pow);
    vec3 specular = specular_strength * spec * light_color2;

    //atenuation
    float distances = length(light_position - out_position);
    float attenuation = 1.0 / (constant + linear * distances + 
    		    quadratic * (distances * distances));

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation; 

    float shadow = ShadowCaclulations(out_position);
    vec3 lighting = (ambient + (1.0-shadow) * (diffuse + specular)) * color;
   
    vec3 result_specular =
        specular * texture(texture_specular1, out_tex).r;


    FragColor = vec4(lighting + result_specular, texture(texture_diffuse1, out_tex).a);
}