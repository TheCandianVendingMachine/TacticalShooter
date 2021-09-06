#version 330 core
out vec4 FragColour;

in vec2 TextureCoordinate;

uniform sampler2D Image;

uniform float exposure;
uniform float gamma;

void main()
    {
        vec3 colour = texture(Image, TextureCoordinate).rgb;

        // HDR Tone mapping
        colour = vec3(1.0) - exp(-colour * exposure);
        
        // Gamma correction
        colour = pow(colour, vec3(1.0 / gamma));

        FragColour = vec4(colour, 1);
    }
