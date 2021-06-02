#version 450

layout(location=0) out vec4 FragColor;

uniform sampler2D u_TexSampler;

in vec2 v_TexPos;

const float PI = 3.141592;

void main()
{
	//FragColor = vec4(1,0,0,1);
	vec2 xy = v_TexPos;
	float masky = xy.y * 2* PI *6; //0~2PI;
	float sinValue = sin(masky);

	FragColor = vec4(sinValue);

}
