#version 330 core
flat in float vState;

out vec4 FragColor;

void main()
{
    float s = (vState > 0.5) ? 1.0 : 0.0;
    vec3 c = vec3(1.0 - s);
    FragColor = vec4(c, 1.0);
}
