#version 450


in vec3 a_Position; // float 3개
in vec3 a_Velocity; // float 속도 3개
in float a_EmitTime ; // float 시간 1개
in float a_LifeTime ; //float 라이프 1개

uniform float u_Time; //누적시간

//const vec3 c_Gravity = vec3(0, -2.8, 0);
const vec3 c_Gravity = vec3(0, 0, 0);
void main()
{

	float newTime =u_Time - a_EmitTime;
	// 반복하는거

	vec3 nesPos = a_Position;
	if(newTime <0.0)
	{
		nesPos = vec3(10000,10000,10000);
	}
	else
	{
		newTime = mod(newTime, a_LifeTime);
		nesPos = nesPos + vec3(newTime,0,0);
		nesPos.y  =nesPos.y + sin(newTime*3.14*2);
		//float t = newTime;
		//float tt = newTime *newTime;	
		//nesPos = nesPos+ t *a_Velocity + 0.5 * c_Gravity * tt;
	}


	gl_Position = vec4(nesPos, 1); // GL고유의값
}
