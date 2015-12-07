#version 330 core

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
}; 

struct Light {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};


out vec4 color;

in vec3 Normal;
in vec3 FragPos;

uniform Material material;
uniform Light light;
uniform vec3 lightColor;
uniform Light light2;
uniform vec3 light2Color;
uniform vec3 objectColor;
uniform vec3 viewPos;

void main()
{
    // Ambient
    vec3 ambient = light.ambient * material.ambient;

    // Diffuse 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff + material.diffuse);

    vec3 light2Dir = normalize(light2.position - FragPos);
    float diff2 = max(dot(norm, light2Dir), 0.0);
    vec3 diffuse2 = light2.diffuse * (diff2 + material.diffuse);
    
    //Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = light.specular * (spec * material.specular);

    vec3 reflect2Dir = reflect(-light2Dir, norm);
    float spec2 = pow(max(dot(viewDir, reflect2Dir), 0.0), 32);
    vec3 specular2 = light2.specular * (spec2 * material.specular);

    vec3 result = (ambient + diffuse + diffuse2 + specular + specular2); //* objectColor;
    color = vec4(result, 1.0f);
}
