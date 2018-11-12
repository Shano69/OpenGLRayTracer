#version 430
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture samplers
uniform sampler2D tex;

void main()
{
	FragColor = texture(tex, TexCoord);
}