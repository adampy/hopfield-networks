#version 330 core
in vec2 vLocal;
flat in vec3 vColor;
uniform float uAspect;
out vec4 FragColor;
void main()
{
    vec2 p = vLocal;
    p.x *= uAspect;
    float r = length(p);
    if (r > 0.5) discard;
    FragColor = vec4(vColor, 1.0);
}
