#version 450


in vec3 a_Position; // float 3��
in vec3 a_Velocity; // float �ӵ� 3��
in float a_EmitTime ; // float �ð� 1��
in float a_LifeTime ; //float ������ 1��
in float a_P ; //float �ֱ� 1��
in float a_A ; //float �� 1��



uniform float u_Time; //�����ð�

//const vec3 c_Gravity = vec3(0, -2.8, 0);
const vec3 c_Gravity = vec3(0, 0, 0);
const mat3 c_NV = mat3(0,-1,0,1,0,0,0,0,0);
void main()
{

	float newTime =u_Time - a_EmitTime;
	// �ݺ��ϴ°�

	vec3 newPos = a_Position;
	if(newTime <0.0)
	{
		newPos = vec3(10000,10000,10000);
	}
	else
	{
		/*newTime = mod(newTime, a_LifeTime);
		newPos = newPos + vec3(newTime,0,0);
		newPos.y  =newPos.y + (a_A *newTime)* sin(newTime * 3.14 * 2 * a_P);
		*/
		float t = newTime;
		float tt = newTime * newTime;	
		vec3 currVel = a_Velocity + t *c_Gravity;
		vec3 normalV= normalize(currVel * c_NV);
		newPos = newPos+ t *a_Velocity + 0.5 * c_Gravity * tt;
		newPos =newPos + normalV * a_A*sin(newTime * 2 * 3.14 * a_P);
	}


	gl_Position = vec4(newPos, 1); // GL�����ǰ�
}
