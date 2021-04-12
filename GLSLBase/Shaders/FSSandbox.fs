#version 450

layout(location=0) out vec4 FragColor;

varying vec4 v_Color;

const vec3 Circle = vec3(0.5f,0.5f,0.0f);

void main()
{

	vec4 color = vec4(0);

	float tempLength = length(v_Color.rgb - Circle);
	if(tempLength <0.5f && tempLength >0.4f )
	{
		color = vec4(1,1,1,1);
	}
	else
	{
		color = vec4(0,0,0,0);
	}
	FragColor = color;
	
}
