#version 330 core
out vec4 FragColour;

struct Material 
    {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
    };

struct Light 
    {
        vec3 direction;

        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };

uniform sampler2D inTexture;

in vec3 VertexNormal;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec2 TextureCoords;

uniform Material material;
uniform Light light;

uniform vec3 ViewPos;
uniform float gamma;

vec3 lightingCalculation(float shadow) 
    {
        vec3 normal = normalize(VertexNormal);

        vec3 lightDir = light.direction;
        vec3 viewDir = normalize(ViewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float diff = max(dot(normal, lightDir), 0);       
        float specularScalar = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

        vec3 ambient =  light.ambient * vec3(texture(material.diffuse, TextureCoords));
        vec3 diffuse =  light.diffuse * diff * vec3(texture(material.diffuse, TextureCoords));
        vec3 specular = light.specular * specularScalar * vec3(texture(material.specular, TextureCoords));

        /*float distance = length(light.position - FragPos);
        float attenuation = 1 / pow(light.constant + light.linear * distance + light.quadratic * distance * distance, 2);
        attenuation = 1;*/

        vec3 finalColour = (ambient + (1 - shadow) * (diffuse + specular));

        return finalColour;
    }

void main()
    {             
        vec3 lightColour = lightingCalculation(0);

        // gamma correction
        vec3 finalColour = pow(lightColour, vec3(1 / gamma));

        FragColour = vec4(finalColour, 1);
    }