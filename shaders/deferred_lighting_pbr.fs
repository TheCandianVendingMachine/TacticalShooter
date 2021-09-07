#version 330 core
#define PI 3.1415926535897932384626433832795
out vec4 FragColour;

struct Light
    {
        vec3 ambient;
        vec3 diffuse;
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
uniform sampler2D gAlbedo;
uniform sampler2D gMetallicRoughtnessAO;

uniform vec3 cameraPosition;
uniform vec2 framebufferSize;

uniform Light light;
uniform LightInfo lightSpatialInfo;

uniform vec3 albedo;
uniform float metallic;
uniform float roughness;
uniform float ambientOcclusion;

vec3 fresnelShlick(float cosTheta, vec3 baseReflectivity)
    {
        return baseReflectivity + (1 - baseReflectivity) * pow(clamp(1 - cosTheta, 0, 1), 5);
    }

float distributionGGX(vec3 normal, vec3 halfVector, float surfaceRoughness)
    {
        float a = surfaceRoughness * surfaceRoughness;

        float a2 = a * a;
        float nDotH = max(dot(normal, halfVector), 0);
        float nDotH2 = nDotH * nDotH;

        float denominator = nDotH2 * (a2 - 1) + 1;

        return a2 / (PI * denominator * denominator);
    }

float geometryShlickGGX(vec3 normal, vec3 viewDirection, float surfaceRoughnessMapped)
    {
        float r = surfaceRoughnessMapped + 1;
        float k = (r * r) / 8.0;

        float NdotV = max(dot(normal, viewDirection), 0);

        return NdotV / (NdotV * (1 - k) + k);
    }

float geometryGGX(vec3 normal, vec3 viewDirection, vec3 lightDir, float surfaceRoughnessMapped)
    {
        float ggx1 = geometryShlickGGX(normal, viewDirection, surfaceRoughnessMapped);
        float ggx2 = geometryShlickGGX(normal, lightDir, surfaceRoughnessMapped);

        return ggx1 * ggx2;
    }

vec3 getLightDirection(vec3 FragPos)
    {
        vec3 directions[3];
        directions[0] = normalize(-lightSpatialInfo.direction);
        directions[1] = normalize(lightSpatialInfo.position - FragPos);
        directions[2] = normalize(lightSpatialInfo.position - FragPos);

        return directions[lightSpatialInfo.type];
    }

void main()
    {
        vec2 normalFragCoord = gl_FragCoord.xy / framebufferSize;
        vec3 FragPos = texture(gPosition, normalFragCoord).rgb;
        vec3 Normal = texture(gNormal, normalFragCoord).rgb;

        vec3 normal = normalize(Normal);
        vec3 viewDirection = normalize(cameraPosition - FragPos);

        vec3 lightDir = getLightDirection(FragPos);
        vec3 halfVector = normalize(viewDirection + lightDir);

        float distance = length(lightSpatialInfo.position - FragPos);
        float attenuation = 1.0 / (distance * distance);
        vec3 radiance = light.diffuse * attenuation;

        vec3 baseReflectivity = vec3(0.04);
        baseReflectivity = mix(baseReflectivity, albedo, metallic);
        vec3 f = fresnelShlick(max(dot(halfVector, viewDirection), 0), baseReflectivity);

        // BRDF
        float ndf = distributionGGX(normal, halfVector, roughness);
        float g = geometryGGX(normal, viewDirection, lightDir, roughness);

        vec3 specular = (ndf * g * f)/ (4 * max(dot(normal, viewDirection), 0) * max(dot(normal, lightDir), 0) + 0.00001);

        // final lighting
        float intensities[3];
        intensities[0] = 1.f;
        intensities[1] = 1.f;

        float theta = dot(lightDir, normalize(-lightSpatialInfo.direction));
        float epsilon = lightSpatialInfo.cutoff - lightSpatialInfo.cutoffOuter;
        intensities[2] = clamp((theta - lightSpatialInfo.cutoffOuter) / epsilon, 0, 1);

        float intensity = intensities[lightSpatialInfo.type];

        vec3 ks = f;
        vec3 kd = vec3(1) - ks;
        kd *= 1 - metallic;

        float nDotL = max(dot(normal, lightDir), 0);
        vec3 lightColour = (kd * albedo / PI + specular) * radiance * nDotL; // may have to sum this in a texture and then do final lighting in another pass

        vec3 ambient = light.ambient * albedo * ambientOcclusion;
        FragColour = vec4(ambient + intensity * lightColour, 1);
    }

