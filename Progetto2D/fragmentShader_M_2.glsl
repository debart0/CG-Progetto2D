#version 330 core
in vec4 ourColor;
out vec4 FragColor;  

precision highp float;

uniform int sceltaFS;

uniform float time;
uniform vec2 res;
uniform vec2 mouse;

const float SIZE_MULTIPLIER = 1.2;
const int STAR_COUNT = 200;
const vec3 STAR_COLOR = vec3(1.0);

//Funzione random che tutti consigliano se cerchi "random glsl", boh funziona
float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}

void point(vec2 pos, float size) {
    pos *= res;
    float d = distance(gl_FragCoord.xy, pos);
	//Uso il mouse anche per i colori
    FragColor.rgb += smoothstep(size * SIZE_MULTIPLIER, 0.0, d) * vec3(0.2 + mouse.x/res.x , abs(sin(time)),0.2 + mouse.y /res.y);
}

void stars() {
    for (int i = 1; i < STAR_COUNT; i++) {
        float sx = rand(vec2(i, -i));
        float sy = rand(vec2(-sx, sx));
        float size = rand(vec2(sx, sy)) ;
		//Uso il mouse per spostare le stelle
        point(vec2(sx * 2.0 - 1.0 + mouse.x/res.x, sy * 2.0 - 1.0 + abs(mouse.y/res.y)), size*size*8);	
	}
}
void main()
{
    if(sceltaFS==0){
        FragColor = ourColor;    
    }
    if(sceltaFS==1){
        FragColor = ourColor;

		vec2 res = 1.0 / res;
		
		stars();
    }

}