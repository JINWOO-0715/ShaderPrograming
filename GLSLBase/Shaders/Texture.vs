#version 450


in vec3 a_Position; // float 3��
in vec2 a_TexPos;

out vec2 v_TexPos;

void main()
{

	gl_Position = vec4(a_Position, 1); // GL�����ǰ�
	v_TexPos = a_TexPos;
}
