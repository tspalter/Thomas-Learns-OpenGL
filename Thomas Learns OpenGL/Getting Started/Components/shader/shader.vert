#version 330 core

struct DirectionLight{
    bool status;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight{
    bool status;
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float constant;
    float linear;
    float quadratic;
};

struct Material{
    vec3 objColor;
    float shininess;
};

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

uniform vec3 cameraPos;

uniform Material mObject;

uniform DirectionLight dirLight;
uniform PointLight pointLight;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 finalColor;

vec3 CalculateDirLight(DirectionLight light, vec3 normal, vec3 viewDir){
    vec3 lightDir = normalize(-light.direction);
    float diffuseParam = max(dot(normal,lightDir),0.0f);

    vec3 halfDir = normalize(lightDir + viewDir);
    float specParam = pow(max(dot(halfDir,normal),0.0f), mObject.shininess);

    vec3 ambient = light.ambient;
    vec3 diffuse = diffuseParam * light.diffuse;
    vec3 specular = specParam * light.specular;
    vec3 res = ambient + diffuse + specular;
    return res;
}

vec3 CalcPointLight(PointLight light, vec3 normal,vec3 fragPos, vec3 viewDir){
    vec3 lightDir = normalize(light.position - fragPos);
    float diffuseParam = max(dot(lightDir,normal),0.0f);

    vec3 halfDir = normalize(lightDir + viewDir);
    float specParam = pow(max(dot(halfDir,normal),0.0f), mObject.shininess);

    float distance = length(light.position - fragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * (distance * distance));
    
    vec3 ambient = attenuation * light.ambient;
    vec3 diffuse = attenuation * diffuseParam * light.diffuse;
    vec3 specular = attenuation * specParam * light.specular;
    
    vec3 res = ambient + diffuse + specular;
    return res;
}

void main(){
    gl_Position = projection * view * model * vec4(position, 1.0f);

    vec3 fragPos = vec3(model * vec4(position, 1.0f));

    vec3 normalV = normalize(normal);
    vec3 viewDir = normalize(cameraPos - fragPos);

    finalColor = 0.1 * mObject.objColor;

    if(dirLight.status){
        vec3 dirLightColor = CalculateDirLight(dirLight,normalV,viewDir);
        finalColor += dirLightColor * mObject.objColor;
    }
    if(pointLight.status){
        vec3 pointLightColor = CalcPointLight(pointLight,normalV,fragPos,viewDir);
        finalColor += pointLightColor * mObject.objColor;
    }
}