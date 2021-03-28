#version 330 core
in vec2 TexCoords;
in vec3 FragPos;
out vec4 color;

uniform sampler2D image;
uniform vec3 spriteColor;
uniform vec3 lightColor;
uniform vec3 lightPos;
// uniform vec3 spotDir;
uniform float lightCutOff;

void main()
{   
    // vec3 lightDir = normalize(FragPos - lightPos);
    float distance = length(FragPos - lightPos);
    // float theta = dot(lightDir, normalize(-spotDir));
    if(distance < lightCutOff)
        color = vec4(lightColor * spriteColor, 1.0) * texture(image, TexCoords);
    else
        color = vec4(0.0 * lightColor * spriteColor, 1.0) * texture(image, TexCoords);
}