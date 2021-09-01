#version 330 core

out vec4 FragColour;
in vec2 TextureCoords;

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

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gColourSpecular;

uniform Light light;
uniform LightInfo lightInfo;

uniform vec3 ViewPos;
uniform vec2 FramebufferSize;

vec3 getLightDirection(vec3 FragPos)
    {
        vec3 directions[3];
        directions[0] = normalize(-lightInfo.direction);
        directions[1] = normalize(lightInfo.position - FragPos);
        directions[2] = normalize(lightInfo.position - FragPos);

        return directions[lightInfo.type];
    }

vec3 lightingCalculation(vec3 VertexNormal, vec3 FragPos, vec3 Albedo, vec3 Specular, float shadow) 
    {
        vec3 normal = normalize(VertexNormal);

        vec3 lightDir = getLightDirection(FragPos);
        vec3 viewDir = normalize(ViewPos - FragPos);
        vec3 halfwayDir = normalize(lightDir + viewDir);

        float diff = max(dot(normal, lightDir), 0);       
        float specularScalar = pow(max(dot(normal, halfwayDir), 0.0), 128.f);

        vec3 ambient =  light.ambient * Albedo;
        vec3 diffuse =  light.diffuse * diff * Albedo;
        vec3 specular = light.specular * specularScalar * Specular;

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
        vec2 normalFragCoord = gl_FragCoord.xy / FramebufferSize;

        vec3 FragPos = texture(gPosition, normalFragCoord).rgb;
        vec3 Normal = texture(gNormal, normalFragCoord).rgb;
        vec3 Albedo = texture(gColourSpecular, normalFragCoord).rgb;
        float Specular = texture(gColourSpecular, normalFragCoord).a;

        vec3 lightColour = lightingCalculation(Normal, FragPos, Albedo, vec3(Specular), 0);

        FragColour = vec4(lightColour, 1);
    }
