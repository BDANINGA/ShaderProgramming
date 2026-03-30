#version 330

uniform float u_Time;

in vec3 a_Position;
in float a_Mass;
in vec2 a_Vel;
in float a_RV;
in float a_Size;

out float v_Grey;

const float c_PI = 3.141592;
const float c_G = -9.8;


float hash(float n) {
    return fract(sin(n) * 43758.5453123);
}

// 하나의 시드(a_RV)에서 서로 다른 위치의 랜덤값을 뽑아내는 함수
float getNextRandom(float seed, float offset) {
    return hash(seed + offset);
}

void sin0()
{
    float rv1 = getNextRandom(a_RV, 12.345);
    float rv2 = getNextRandom(a_RV, 67.890);
    float rv3 = getNextRandom(a_RV, 45.678);
    float rv4 = getNextRandom(a_RV, 23.584);
    
    float lifeScale = 2.0;
    float lifeTime = 0.5 * rv2 * lifeScale;
    

    float startTime = rv3;
    float newTime = u_Time - startTime;

    if (newTime>0)
    {
        float t = mod(newTime, 1.0f);
        float amp = (1-t) * 0.4 * (rv1 - 0.5)*2;
        float period = rv4;

        vec4 newPosition;
        newPosition.x = a_Position.x + t;
        newPosition.y = a_Position.y + amp * sin(t * 2 * c_PI * period);
        newPosition.z = 0.0;
        newPosition.w = 1.0;
    
        gl_Position = newPosition;
        v_Grey = 1 - t;
    }
    else
    {
        gl_Position = vec4(4400, 50000, 0, 1);
        v_Grey = 0;
    }
}

void Falling()
{
    float rv1 = getNextRandom(a_RV, 12.345);
    float rv2 = getNextRandom(a_RV, 67.890);
    float rv3 = getNextRandom(a_RV, 45.678);
    float rv4 = getNextRandom(a_RV, 23.584);

    float startTime = rv2;
    float newTime = u_Time - startTime;

    if(newTime > 0)
    {
        float lifeScale = 2.0f;
        float lifeTime = 0.5 + rv4 * lifeScale;
        float t = lifeTime * fract(newTime / lifeTime);         // 0 ~ lifeTime
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

void sin1()
{
    // 1. 시드값 추출
    float rv1 = getNextRandom(a_RV, 12.345); // 진폭용
    float rv2 = getNextRandom(a_RV, 67.890); // 회전 속도용
    float rv3 = getNextRandom(a_RV, 45.678); // 시작 시간용
    float rv4 = getNextRandom(a_RV, 23.584); // 주기(Period)용

    // 느린 u_Time을 보정하기 위해 배수 적용 (원하는 속도에 따라 조절)
    float t_scaled = u_Time * 10.0; 
    float startTime = rv3 * 5.0; // 등장 시점 분산
    float newTime = t_scaled - startTime;

    if (newTime > 0.0)
    {
        // 2. 시간 파라미터 (0.0 ~ 1.0 반복)
        float t = mod(newTime, 1.0);
        
        // 3. 움직임 계산 (기존 로직 유지 + 강화)
        // 뒤로 갈수록 감쇠하는 진폭
        float amp = (1.0 - t) * 0.5 * (rv1 - 0.5) * 2.0;
        float period = 1.0 + rv4 * 3.0; // 주기를 조금 더 다양하게

        // 4. 사각형을 위한 화려한 효과 추가
        // (1) 자체 회전: 시간에 따라 사각형 파편이 뱅글뱅글 돕니다.
        float rotation = t * 10.0 * (rv2 + 0.5);
        float cosR = cos(rotation);
        float sinR = sin(rotation);
        
        // 사각형의 각 정점을 회전시킴
        vec2 rotatedPos;
        rotatedPos.x = a_Position.x * cosR - a_Position.y * sinR;
        rotatedPos.y = a_Position.x * sinR + a_Position.y * cosR;

        // (2) 크기 변화: 나타날 때 커졌다가 사라질 때 작아짐
        float sizePulse = sin(t * c_PI) * 1.5; 

        vec4 newPosition;
        // X축: 오른쪽으로 이동하면서 약간의 가속도 추가
        newPosition.x = (rotatedPos.x * sizePulse) + (t * 2.5 - 1.25); 
        // Y축: 사인파 운동
        newPosition.y = (rotatedPos.y * sizePulse) + (amp * sin(t * 2.0 * c_PI * period));
        newPosition.z = 0.0;
        newPosition.w = 1.0;

        gl_Position = newPosition;
    }
    else
    {
        // 화면 밖으로 (현재 렌더러 범위에 맞게 조정)
        gl_Position = vec4(-2.0, -2.0, 0.0, 1.0);
    }
}

void main()
{
    sin0();
}