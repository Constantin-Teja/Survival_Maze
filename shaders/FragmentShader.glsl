#version 330
in vec3 fColor;
layout(location = 0) out vec4 out_color;


void main()
{
    out_color = vec4(fColor * 0.5f + 0.5f, 1);
}
