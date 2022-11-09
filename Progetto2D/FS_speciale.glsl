#version 330 core
in vec4 ourColor;
out vec4 FragColor;

uniform int sceltaFS;

uniform float time;
uniform vec2 res;
uniform vec2 mouse;

#define PI 3.1415


void main()
{

    vec2 coord = gl_FragCoord.xy /res;
	if (gl_FragCoord.xy.x==0){
	FragColor=vec4(1,0,0,1);
	}else{
		vec2 pos = ( gl_FragCoord.xy - res  / 2.0) / res.y;
		//if (intensity > 0.99)
	
		float angle = acos(dot(vec2(0.0, 1.0), normalize(pos)))* sign(pos.x) / PI * 1.0 + time / 1.0;
		float koef = (sin(time + length(pos)) * 0.5 + 0.5) * 0.5 + 0.5;
		float intensity = 1.0 - mod(length(pos) * koef * 10.0 + angle, 1.0);
	
		vec3 col = mix(vec3(1.0,0.0,0.0), vec3(0.0,0.0,1.0), length(pos));
		//	col = vec3(0.0,1.0,9.0);
		FragColor = vec4( col * intensity, 1.0 );
	}
 
}