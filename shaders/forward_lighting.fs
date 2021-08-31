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
        vec3 ambient;
        vec3 diffuse;
        vec3 specular;

        float constant;
        float linear;
        float quadratic;
    };

struct LightInfo
    {
        vec3 position;
        vec3 direction;
        float cutoff;
        float cutoffOuter;
        int type;
    };

uniform sampler2D inTexture;

in vec3 VertexNormal;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec2 TextureCoords;

uniform Material material;

uniform Light light;
uniform LightInfo lightInfo;

uniform vec3 ViewPos;
uniform float gamma;

vec3 getLightDirection()
    {
        vec3 directions[3];
        directions[0] = normalize(-lightInfo.direction);
        directions[1] = normalize(lightInfo.position - FragPos);
        directions[2] = normalize(lightInfo.position - FragPos);

        return directions[lightInfo.type];
    }

vec3 lightingCalculation(float shadow) 
    {
        vec3 normal = normalize(VertexNormal);

        vec3 lightDir = getLightDirection();
        vec3 viewDir = normalize(ViewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float diff = max(dot(normal, lightDir), 0);       
        float specularScalar = pow(max(dot(normal, halfwayDir), 0.0), material.shininess);

        vec3 ambient =  light.ambient * vec3(texture(material.diffuse, TextureCoords));
        vec3 diffuse =  light.diffuse * diff * vec3(texture(material.diffuse, TextureCoords));
        vec3 specular = light.specular * specularScalar * vec3(texture(material.specular, TextureCoords));

        float intensities[3];
        intensities[0] = 1.f;
        intensities[1] = 1.f;

        float theta = dot(lightDir, normalize(-lightInfo.direction));
        float epsilon = lightInfo.cutoff - lightInfo.cutoffOuter;
        intensities[2] = clamp((theta - lightInfo.cutoffOuter) / epsilon, 0, 1);

        float intensity = intensities[lightInfo.type];

        /*float distance = length(light.position - FragPos);
        float attenuation = 1 / pow(light.constant + light.linear * distance + light.quadratic * distance * distance, 2);
        attenuation = 1;*/

        vec3 finalColour = (ambient + intensity * (1 - shadow) * (diffuse + specular));

        return finalColour;
    }

void main()
    {             
        vec3 lightColour = lightingCalculation(0);

        // gamma correction
        vec3 finalColour = pow(lightColour, vec3(1 / gamma));

        FragColour = vec4(finalColour, 1);
    }