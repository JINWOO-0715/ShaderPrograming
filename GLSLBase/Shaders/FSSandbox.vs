#version 450


in vec3 a_Position; // float 3��

varying vec4 v_Color;



void main()
{
	gl_Position = vec4(a_Position, 1); // GL������ ��°�

	v_Color = vec4(a_Position.x,a_Position.y,0.0,0.0);
}