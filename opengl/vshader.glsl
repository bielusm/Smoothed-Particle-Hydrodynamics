#version 150

in vec4 vData;
in vec4 vPos;
uniform float Size;
uniform mat4 ModelView, Projection;

void main()
{
	vec4 loc= vPos+vData;
    gl_Position =  Projection* ModelView*loc;
}

