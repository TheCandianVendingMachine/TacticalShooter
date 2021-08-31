#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gColourSpecular;

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
        gColourSpecular.rgb = texture(material.diffuse, TextureCoords).rgb;
        gColourSpecular.a = texture(material.specular, TextureCoords).r;
    }
