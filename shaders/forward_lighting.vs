#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aTangent;
layout (location = 3) in vec2 aTextureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform mat4 lightSpaceMatrix;

out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;
out vec2 TextureCoords;
out mat3 TBN;

void main()
    {
        vec3 bTangent = cross(aNormal, aTangent);

        vec3 t = normalize(vec3(model * vec4(aTangent, 0)));
        vec3 b = normalize(vec3(model * vec4(bTangent, 0)));
        vec3 n = normalize(vec3(model * vec4(aNormal, 0)));
        TBN = mat3(t, b, n);

        FragPos = vec3(model * vec4(aPosition, 1.0));

        gl_Position = projection * view * vec4(FragPos, 1.0);

        TextureCoords = aTextureCoords;
        Normal = aNormal;
        FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    }  

