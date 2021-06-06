#version 450

layout(location=0) out vec4 FragColor;

uniform sampler2D u_TexSampler;
uniform float u_Step;

in vec2 v_TexPos;

const float PI = 3.141592;

vec4 P1() // if없이 거꾸로 
{
	vec2 newTex = v_TexPos;
	newTex.y =abs(newTex.y-0.5)*2;
	vec4 retrunColor = texture(u_TexSampler,newTex);

	return retrunColor;
}

vec4 P2()// rgb를 bgr로 세우기
{
	vec2 newTex = v_TexPos;
	newTex.x =fract(newTex.x*3.0);
	newTex.y = floor(v_TexPos.x*3.0)/3.0 + newTex.y/3.0;


	vec4 retrunColor = texture(u_TexSampler,newTex);

	return retrunColor;
}

vec4 SingleTexture()
{
	return texture(u_TexSampler,v_TexPos);
}


vec4 SpriteTexture()
{
	vec2 newTex = vec2(v_TexPos.x,u_Step/6.0+v_TexPos.y/6.0);
	return texture(u_TexSampler,newTex);
}

void main()
{

	FragColor = SingleTexture();
	//FragColor =P2();
	//FragColor = texture(u_TexSampler,v_TexPos);
}


