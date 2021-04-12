#version 450


in vec3 a_Position; // float 3개

varying vec4 v_Color;



void main()
{
	gl_Position = vec4(a_Position, 1); // GL고유의 출력값

	v_Color = vec4(a_Position.x+0.5f,a_Position.y+0.5f,0.0,0.0);
}
