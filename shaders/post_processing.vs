#version 330 core
layout(location = 0) in vec3 aVertexCoordinate;
layout(location = 3) in vec2 aTextureCoordinate;

out vec2 TextureCoordinate;

void main()
    {
        TextureCoordinate = aTextureCoordinate;
        gl_Position = vec4(aVertexCoordinate.z, aVertexCoordinate.x, 0, 1);
    }
