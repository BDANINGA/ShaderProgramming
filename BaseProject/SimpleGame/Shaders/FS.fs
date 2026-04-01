#version 330

layout(location=0) out vec4 FragColor;

in vec2 v_TPos;

float c_PI = 3.141592;

uniform float u_Time;

void Simple()
{
	if (v_TPos.x + v_TPos.y > 0.5)
	{
		FragColor = vec4(0);
	}
	else
	{
		FragColor = vec4(v_TPos, 0, 1);
	}
}

void linePattern()
{
	float lineCountH = 10;
	float lineCountV = 5;
	float lineWidth = 1;

	lineCountH = lineCountH / 2;
	lineCountV = lineCountV / 2;
	lineWidth = 100 / lineWidth;
	float per = - 0.5*c_PI;

	float grey = pow(abs(sin((v_TPos.y * 2 * c_PI + per) * lineCountH)), lineWidth);
	float grey1 = pow(abs(sin((v_TPos.x * 2 * c_PI + per) * lineCountV)), lineWidth);
	FragColor = vec4(grey+grey1);
}

void Circle()
{
	vec2 center = vec2(0.5, 0.5);
	vec2 currPos = v_TPos.xy;
	float d = distance(center, currPos);
	float lineWidth = 0.01;
	float radius = 0.5;	

	if (d > radius - lineWidth && d < radius)
	{
		FragColor = vec4(1);
	}
	else
	{
		FragColor = vec4(0);
	}
}

void CircleSin()
{
	vec2 center = vec2(0.5, 0.5);
	vec2 currPos = v_TPos.xy;
	float d = distance(center, currPos);
	float value = abs(sin(d * c_PI * 16 - u_Time * 5));
	
	FragColor = vec4(pow((value), 16));
}
void PsychedelicFlower()
{
    // 1. 좌표계를 중앙 중심으로 변경 (-1.0 ~ 1.0)
    vec2 p = v_TPos.xy * 2.0 - 1.0;
    
    // 2. 극좌표계 변환 (거리 r, 각도 a)
    float r = length(p);
    float a = atan(p.y, p.x);
    
    // 3. 꽃잎 모양 계산 (시간에 따라 춤추는 곡선)
    float petalCount = 6.0;
    float t = u_Time * 2.0;
    // r 값에 변이를 주어 꽃잎 모양 생성
    float d = r - (0.5 + 0.3 * sin(a * petalCount + t));
     d = abs(d); // 선으로 표현
    
    // 4. 네온 조명 효과 (Glow)
    float glow = 0.01 / (d + 0.001); // d가 0에 가까울수록 값이 커짐
    
    // 5. 시간에 따라 변하는 무지개 색상
    vec3 col = 0.5 + 0.5 * cos(u_Time + vec3(0, 2, 4) + a);
    
    FragColor = vec4(col * glow, 1.0);
}
void RainbowGrid()
{
    float t = u_Time;
    
    // 1. 격자 밀도 조절
    vec2 gridScale = vec2(20.0, 10.0);
    vec2 p = v_TPos.xy * gridScale;
    
    // 2. 사선 방향의 물결 추가
    float wave = sin(v_TPos.x * 5.0 + v_TPos.y * 5.0 + t * 4.0);
    
    // 3. 격자 선 계산
    vec2 grid = abs(fract(p - 0.5 + wave * 0.2) - 0.5);
    float line = min(grid.x, grid.y);
    
    // 4. 네온 선 효과 (두께 조절)
    float lineEffect = smoothstep(0.0, 0.05, line);
    lineEffect = 1.0 - lineEffect; // 선 부분만 1.0으로 반전
    
    // 5. 그라데이션 색상 (TPos 위치에 기반)
    vec3 col = 0.5 + 0.5 * cos(t + v_TPos.xyx * vec3(1.0, 2.0, 3.0) + vec3(0, 2, 4));
    
    FragColor = vec4(col * lineEffect * 2.0, 1.0); // 밝기 강조
}
void main()
{
	RainbowGrid();
}
