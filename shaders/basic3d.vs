#version 330 core
layout (location = 0) in vec3 aPosition;
layout (location = 4) in vec3 aColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 VertexColour;

void main()
    {
        gl_Position = projection * view * model * vec4(aPosition, 1); 
        VertexColour = vec3(aColour);
    }  

