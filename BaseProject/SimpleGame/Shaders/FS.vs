#version 330

in vec3 a_Position; 
in vec2 a_TPos;

out vec2 v_TPos;

void main()
{
    vec4 newPosition = vec4(a_Position, 1);

    v_TPos = a_TPos;
    gl_Position = newPosition;
}