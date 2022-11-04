/*Implementazione di Utils.h*/

#include "Utils.h"
#include "Definizioni.h"

extern Figura Asteroide1, Asteroide2, Asteroide3;
extern Entity player, nemico1, nemico2, nemico3;
extern vec2 player_default_pos, nemico1_default_pos, nemico2_default_pos, nemico3_default_pos;

pair<vec4, vec4> calcolaBoundingBox(Figura* fig) {
	vec3 min = fig->vertici.at(0);
	vec3 max = fig->vertici.at(0);
	vec3 topLeftCorner, bottomRightCorner;
	for (vec3 vertice : fig->vertici) {
		if (min.x > vertice.x) min.x = vertice.x;
		if (min.y > vertice.y) min.y = vertice.y;
		if (max.x < vertice.x) max.x = vertice.x;
		if (max.y < vertice.y) max.y = vertice.y;

	}
	topLeftCorner.x = min.x;
	topLeftCorner.y = max.y;
	bottomRightCorner.x = max.x;
	bottomRightCorner.y = min.y;

	//Aggiungo i vertici della spezzata per costruire il bounding box
	fig->vertici.push_back(vec3(min.x, min.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(max.x, min.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(max.x, max.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(min.x, min.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(min.x, max.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(max.x, max.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->nv = fig->vertici.size();

	pair<vec4, vec4> pair = make_pair(vec4(topLeftCorner, 1.0), vec4(bottomRightCorner, 1.0));
	return pair;
}
/*
pair<float, float> calcolaDimensioniFigura(Figura* fig) {
	pair<float,float> pair = make_pair(fig->tl_original)
}*/

void inizializzaEntity() {
	player.posX = player_default_pos.x; player.posY = player_default_pos.y; player.speed = PLAYER_SPEED;

	nemico1.posX = nemico1_default_pos.x; nemico1.posY = nemico1_default_pos.y;
	nemico1.speed = ENEMY_SPEED_1;
	nemico1.dx = (float) nemico1.speed;

	nemico2.posX = nemico2_default_pos.x; nemico2.posY = nemico2_default_pos.y;
	nemico2.speed = ENEMY_SPEED_2;
	nemico2.dx = (float) nemico2.speed;
	nemico3.speed = ENEMY_SPEED_3;
	nemico3.dx = (float) nemico3.speed;


}

void resettaPosizionePlayer() {
	player.posX = player_default_pos.x;
	player.posY = player_default_pos.y;
	printf("Posizione resettata\n");
}