#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 3) in vec2 aTextureCoords;

out vec2 TextureCoords;

void main()
    {
        gl_Position = vec4(aPosition.z, aPosition.x, 0, 1);
        TextureCoords = aTextureCoords;
    }
