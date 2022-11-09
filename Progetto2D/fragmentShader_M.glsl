#version 330 core
in vec4 ourColor;
out vec4 FragColor;  

uniform int sceltaFS;

uniform float time;
uniform vec2 res;
uniform vec2 mouse;

#define PI 3.1415

const float DENSITY = 0.1;
const vec3 STAR_COLOR = vec3(1.0);

float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void point(vec2 pos) {
    pos *= res;

    float d = distance(gl_FragCoord.xy, pos);
    FragColor.rgb += smoothstep(4.0, 0.0, d) * STAR_COLOR;
}

void stars() {
    for (int i = 1; i < 70; i++) {
        float sx = rand(vec2(-i, i));
        float sy = rand(vec2(sx, -sx));
        point(vec2(sx, sy));
    }
}
void main()
{
    if(sceltaFS==0){
        FragColor = ourColor;    
    }
    if(sceltaFS==1){

	    //vec2 position = ( gl_FragCoord.xy / res.xy )+mouse/800;
	    //FragColor = vec4( 0.1 + mouse.x/1000, 0.1, 0.1 + mouse.y/1000, 1.0 );
	    //FragColor = vec4(0.02, 0.13, 0.29, 1.0);
        FragColor = ourColor;
        vec2 res = 1.0 / res;
        stars();
    }

}