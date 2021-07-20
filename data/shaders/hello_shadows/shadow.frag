#version 450 core

layout(location = 0) out vec4 FragColor;

in vec2 out_tex;
in vec3 out_normal;
in vec3 out_position;
in vec3 pointLight_Position;
in vec3 tangentFragPos;
in vec3 tangentLightDirection;
in vec3 tangentLightPos;
in vec3 tangentViewPos;
in vec4 fragPosLightSpace;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;
uniform sampler2D texture_emission1;
uniform sampler2D shadowMap;

uniform float constant;
uniform float linear;
uniform float quadratic;

const float ambient_strengh = 0.1f;
const float specular_strength = 0.8;
const float specular_pow = 256;
const vec3 light_color = vec3(1,1, 1);
const vec3 light_color2 = vec3(500,128,0);

float ShadowCalculations(vec4 fragPosLightSpace)
{
    float bias = max(0.001f * (1.0 - dot(out_normal, tangentLightDirection)), 0.0005f);
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // check whether current frag pos is in shadow

    //PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;

    if(projCoords.z > 1.0)
        shadow = 0.0;

    return shadow;
}

vec3 calculateDiractionalLight(vec3 normal, vec3 viewDir, float shadow)
{
    vec3 color = texture(texture_diffuse1,out_tex).rgb;
    vec3 ambient = ambient_strengh * color;

    vec3 light_direction = tangentLightDirection;
    viewDir = normalize(tangentViewPos - tangentFragPos);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color;

     // Compute specular light.
    vec3 reflection_direction = reflect(-light_direction,normal);
    vec3 halfWayDir = normalize(light_direction + viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), specular_pow);
    vec3 specular = specular_strength * spec * light_color;

    vec3 result = (ambient +(1.0-shadow) * (diffuse + specular)) * color;

    vec3 result_specular =
       specular * texture(texture_specular1, out_tex).r;

    result +=result_specular;

    return result;
}

vec3 CalculatePointLight(vec3 normal, vec3 out_position, vec3 viewDir, float shadow)
{
    vec3 color = texture(texture_diffuse1,out_tex).rgb;
     // Compute ambient light.
    vec3 ambient = ambient_strengh * color * light_color2;

    vec3 light_direction = normalize(tangentLightPos - tangentFragPos);
    float diff = max(dot(normal, light_direction), 0.0);
    vec3 diffuse = diff * light_color2;


    vec3 reflection_direction = reflect(-light_direction,normal);
    vec3 halfWayDir = normalize(light_direction + viewDir);
    float spec = pow(max(dot(normal, halfWayDir), 0.0), specular_pow);
    vec3 specular = specular_strength * spec * light_color2;

    // atenuation
    float distance    = length(pointLight_Position - out_position);
    float attenuation = 1.0 / (constant + linear * distance + 
    		    quadratic * (distance * distance));

    ambient  *= attenuation;  
    diffuse   *= attenuation;
    specular *= attenuation; 

    //emission light
    vec3 emission = texture(texture_emission1, out_tex).rgb;

    vec3 lighting = (ambient +(1.0-shadow) * (diffuse + specular) + emission) * color;

    vec3 result_specular =
       specular * texture(texture_specular1, out_tex).r;

     lighting +=result_specular;

    return lighting;
}

void main()
{
    vec3 normal = texture(texture_normal1, out_tex).rgb;
    normal = normalize(normal*2.0-1.0);
    vec3 viewDir = normalize(tangentViewPos - tangentFragPos);
    float shadow  = ShadowCalculations(fragPosLightSpace);

    vec3 result = calculateDiractionalLight(normal,viewDir,shadow);
    result += CalculatePointLight(normal,out_position,viewDir,shadow);

   // Final lighting.
   FragColor = vec4(result, texture(texture_diffuse1, out_tex).a);
}