#version 450

layout(location=0) out vec4 FragColor;

uniform sampler2D u_TexSampler;

in vec2 v_TexPos;

const float PI = 3.141592;

vec4 P1()
{
	vec2 newTex = v_TexPos;
	newTex.y =abs(newTex.y-0.5)*2;
	vec4 retrunColor = texture(u_TexSampler,newTex);

	return retrunColor;
}

void main()
{


	FragColor =P1();
	//FragColor = texture(u_TexSampler,v_TexPos);
}


