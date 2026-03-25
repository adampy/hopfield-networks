#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 iCenter;
layout(location = 2) in vec3 iColor;
uniform float uScale;
out vec2 vLocal;
flat out vec3 vColor;
void main()
{
    vLocal = aPos;
    vColor = iColor;
    vec2 pos = iCenter + aPos * uScale;
    gl_Position = vec4(pos, 0.0, 1.0);
}
