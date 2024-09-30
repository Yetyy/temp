#version 330 core
out vec4 FragColor;
in vec2 TexCoord;
//текстурные сэмплеры
uniform sampler2D texture1;
//uniform sampler texture 
void main()
{
    FragColor = texture(texture1, TexCoord);
}