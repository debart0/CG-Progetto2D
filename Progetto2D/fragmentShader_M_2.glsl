#version 330 core
in vec4 ourColor;
out vec4 FragColor;  
 
uniform vec2 res;
uniform vec2 mouse;
#define PI 3.1415


float circle(vec2 c, float r, vec2 p) {
	
	if (length(c-p) < r) { return 1.0; }
	return 0.0;
}

void main()
{

    vec2 coord = gl_FragCoord.xy /res;

    FragColor=vec4(1-sin(coord.x),0.0,1-cos(coord.y ),1 );

 
}