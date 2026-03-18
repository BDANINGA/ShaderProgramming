#version 330

uniform float u_Time;

in vec3 a_Position;
in float a_Mass;
in vec2 a_Vel;

const float c_PI = 3.141592;
const float c_G = -9.8;

void sin1()
{
	float t = u_Time * 5;
	vec4 newPosition;
	newPosition.x = a_Position.x + sin(t * 3.141592);
	newPosition.y = a_Position.y + cos(t * 3.141592);
	newPosition.z = 0;
	newPosition.w = 1;
	gl_Position = newPosition;
}

void mobiusShard()
{
    float t = u_Time * 1.2;
    
    // 1. 극좌표계(Polar Coordinates) 개념 도입
    // 중심으로부터의 거리(r)와 각도(theta)를 계산합니다.
    float r = length(a_Position.xy);
    float theta = atan(a_Position.y, a_Position.x);
    
    // 2. 시간과 공간의 꼬임 (The Twist)
    // 외곽으로 갈수록 더 많이 회전하고, 시간에 따라 꼬임의 강도가 변합니다.
    float twist = sin(t + r * 5.0) * 2.0;
    float newTheta = theta + twist;
    
    // 3. 동적 두께감 (Fake Volume)
    // 원거리에서 수축하는 효과를 주어 마치 종이가 얇게 회전하는 느낌을 줍니다.
    float pulse = cos(t * 2.0 + r);
    float fakeVolume = 0.8 + 0.4 * pulse;

    vec4 newPosition;
    
    // 회전 변환 행렬 없이 직접 삼각함수로 위치 재계산
    // x축은 꼬인 각도의 cos, y축은 sin을 사용하여 '안팎이 뒤집히는' 회전 구현
    newPosition.x = r * cos(newTheta) * fakeVolume;
    newPosition.y = r * sin(newTheta) * fakeVolume;
    
    // 4. 관성 이동 (Inertial Drift)
    // 전체적인 위치를 8자 모양(Lissajous)으로 유영하게 만듭니다.
    newPosition.x += sin(t * 0.5) * 0.3;
    newPosition.y += cos(t * 0.7) * 0.3;
    
    newPosition.z = 0.0; // Z축 사용 금지 규칙 준수
    newPosition.w = 1.0;
    
    gl_Position = newPosition;
}

void Falling()
{
    float t = mod(u_Time, 1.0f);
    float tt = t*t;
    float vx, vy;
    vx = a_Vel.x;
    vy = a_Vel.y;
    vec4 newPos;
    newPos.x = a_Position.x + (vx * t);
    newPos.y = a_Position.y + (vy * t) + (0.5 * c_G * tt);
    newPos.z = 0;
    newPos.w = 1;

    gl_Position = newPos;
}

void main()
{
    Falling();
}