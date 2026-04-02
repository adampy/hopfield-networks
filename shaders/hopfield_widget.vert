#version 330 core
layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 iCell;
layout(location = 2) in float iState;

uniform vec2 uGrid;

flat out float vState;

void main()
{
    vState = iState;

    vec2 cellSize = vec2(2.0 / uGrid.x, 2.0 / uGrid.y);
    vec2 origin = vec2(-1.0, -1.0);

    vec2 pos = origin + iCell * cellSize + aPos * cellSize;

    gl_Position = vec4(pos, 0.0, 1.0);
}
