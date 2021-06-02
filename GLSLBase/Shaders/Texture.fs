#version 450

layout(location=0) out vec4 FragColor;

uniform sampler2D u_TexSampler;

in vec2 v_TexPos;

const float PI = 3.141592;

void main()
{
	//FragColor = vec4(1,0,0,1);
	vec2 xy = v_TexPos;
	float maskY = xy.y * 2* PI *6; //0~2PI;
	float sinValueY = sin(maskY);
	sinValueY = pow (sinValueY,10);

	float maskX = xy.x * 2* PI *6; //0~2PI;
	float sinValueX = sin(maskX);
	sinValueX = pow (sinValueX,10);
	//ceil ÂðÇÏ°Ô ¿Ã¸² FragColor = vec4(ceil(sinValueX+sinValueY));
	//max ¹æÃæ¸Á
	FragColor = vec4(max(sinValueX,sinValueY));

}
