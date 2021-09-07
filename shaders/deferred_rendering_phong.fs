#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;
layout (location = 3) out vec3 gMetallicRoughnessAO;

in vec3 VertexNormal;
in vec3 FragPos;
in vec4 FragPosLightSpace;
in vec2 TextureCoords;

struct Material 
    {
        sampler2D diffuse;
        sampler2D specular;
        float shininess;
    };
uniform Material material;

void main()
    {
        gPosition = FragPos;
        gNormal = VertexNormal;
        //gAlbedo = texture(material.diffuse, TextureCoords).rgb;
        
        gAlbedo = vec3(0.61, 0, 0);
        
        gMetallicRoughnessAO.r = 0.3;
        gMetallicRoughnessAO.g = 0.1;
        gMetallicRoughnessAO.b = 1;
    }
