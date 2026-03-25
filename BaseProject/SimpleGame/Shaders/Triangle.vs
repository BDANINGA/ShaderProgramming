#version 330

uniform float u_Time;

in vec3 a_Position;
in float a_Mass;
in vec2 a_Vel;
in float a_RV;
in float a_Size;

const float c_PI = 3.141592;
const float c_G = -9.8;


float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

// 하나의 시드(a_RV)에서 서로 다른 위치의 랜덤값을 뽑아내는 함수
float getNextRandom(float seed, float offset) {
    return hash(seed + offset);
}

void Falling()
{
    float rv1 = getNextRandom(a_RV, 12.345);
    float rv2 = getNextRandom(a_RV, 67.890);
    float rv3 = getNextRandom(a_RV, 45.678);

    float startTime = rv2;
    float newTime = u_Time - startTime;

    if(newTime > 0)
    {
        float t = mod(newTime, 1.0f);
        float tt = t*t;

        float randomScale = a_Size * (0.5 + rv3);

        float vx = a_Vel.x/3;
        float vy = a_Vel.y/3;

        float sx = (a_Position.x * randomScale) + sin(rv1 * 2.0 * c_PI);
        float sy = (a_Position.y * randomScale) + cos(rv1 * 2.0 * c_PI);

        vec4 newPos;
        newPos.x = sx + (vx * t);
        newPos.y = sy + (vy * t) + (0.5 * c_G * tt);
        newPos.z = 0;
        newPos.w = 1;

        gl_Position = newPos;
    }
    else
    {
        gl_Position = vec4(-1000, 0, 0, 1);
    }
}

void main()
{
    Falling();
}