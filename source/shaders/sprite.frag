#version 330 core
in vec2 TexCoords;
in vec3 FragPos;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform vec3 lightPos;
uniform float lightCutOff;

void main()
{   
    float distance = length(FragPos - lightPos);

    if(distance < lightCutOff)
        color = vec4( spriteColor, 1.0) * texture(image, TexCoords);
    else
        color = vec4(0.0 * spriteColor, 1.0) * texture(image, TexCoords);
}