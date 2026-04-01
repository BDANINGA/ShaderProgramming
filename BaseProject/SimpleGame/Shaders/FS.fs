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

void main()
{
	CircleSin();
}
