#version 330 core


layout (location = 0) in vec3 aPos;   // the position variable has attribute position 0
layout (location = 1) in vec4 aColor; // the color variable has attribute position 1

uniform mat4 Projection;
uniform mat4 Model;
out vec4 ourColor; // output a color to the fragment shader
uniform float time;
uniform vec2 res;
uniform vec2 mouse;

uniform int sceltaVS;

void main(){	
	if (sceltaVS==1) {
		vec4	v =  vec4(aPos, 1.0);
		float	offset = 0.1*sin(0.01*time+5.0*v.x);
		v.y=v.y+offset;

		gl_Position = Projection*Model*v;
	}  
	if (sceltaVS==0)
		 gl_Position = Projection*Model*vec4(aPos, 1.0);

	ourColor=aColor;
	
}

