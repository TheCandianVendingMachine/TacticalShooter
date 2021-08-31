#version 330 core
out vec4 FragColour;

uniform sampler2D inTexture;

in vec3 VertexColour;
in vec2 TextureCoords;

void main()
    {             
        FragColour = vec4(VertexColour * texture(inTexture, TextureCoords).rgb, 1);
    }