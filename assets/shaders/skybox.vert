#version 330 core
layout(location = 0) in vec3 aPos;
out vec3 TexCoords;
uniform mat4 u_View;
uniform mat4 u_Projection;
void main() {
    TexCoords = aPos;
    gl_Position = u_Projection * mat4(mat3(u_View)) * vec4(aPos, 1.0);
}