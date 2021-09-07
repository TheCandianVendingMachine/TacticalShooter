#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMetallicRoughnessAO;

in vec3 FragPos;
in vec2 TextureCoords;
in mat3 TBN;

struct Material 
    {
        sampler2D albedoMap;
        sampler2D normalMap;
        sampler2D metallicMap;
        sampler2D roughnessMap;
        sampler2D ambientOcclusionMap;
    };
uniform Material material;

vec3 calculateNormal()
    {
        vec3 colour = texture(material.normalMap, TextureCoords).rgb;
        vec3 normal = colour * 2.0 - 1.0;
        normal = normalize(TBN * normal);
        return normal;
    }

void main()
    {
        gPosition = FragPos;
        
        gNormal = calculateNormal();
        gAlbedo = texture(material.albedoMap, TextureCoords).rgb;
        gMetallicRoughnessAO.r = texture(material.metallicMap, TextureCoords).r;
        gMetallicRoughnessAO.g = texture(material.roughnessMap, TextureCoords).r;
        gMetallicRoughnessAO.b = texture(material.ambientOcclusionMap, TextureCoords).r;
    }
