#version 450


in vec3 a_Position; // float 3��
in vec3 a_Velocity; // float �ӵ� 3��
in float a_EmitTime ; // float �ð� 1��
in float a_LifeTime ; //float ������ 1��

uniform float u_Time; //�����ð�

//const vec3 c_Gravity = vec3(0, -2.8, 0);
const vec3 c_Gravity = vec3(0, 0, 0);
void main()
{

	float newTime =u_Time - a_EmitTime;
	// �ݺ��ϴ°�

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


	gl_Position = vec4(nesPos, 1); // GL�����ǰ�
}
