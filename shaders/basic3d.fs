#version 330 core
out vec4 FragColour;

in vec3 VertexColour;

void main()
    {             
        FragColour = vec4(VertexColour, 1);
    }