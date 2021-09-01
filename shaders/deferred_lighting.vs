#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 3) in vec2 aTextureCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TextureCoords;

void main()
    {
        gl_Position = projection * view * model * vec4(aPosition, 1);
        TextureCoords = aTextureCoords;
    }
