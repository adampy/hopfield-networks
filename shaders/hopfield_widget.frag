#version 330 core
in vec2 vLocal;
flat in vec3 vColor;
out vec4 FragColor;
void main()
{
    float r = length(vLocal);
    if (r > 0.5) discard;
    FragColor = vec4(vColor, 1.0);
}
