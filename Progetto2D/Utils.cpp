/*Implementazione di Utils.h*/

#include "Utils.h"
#include "Definizioni.h"

extern Figura Asteroide1, Asteroide2, Asteroide3;
extern Entity player, nemico1, nemico2, nemico3;

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
	pair<vec4, vec4> pair = make_pair(vec4(topLeftCorner, 1.0), vec4(bottomRightCorner, 1.0));
	return pair;
}
/*
pair<float, float> calcolaDimensioniFigura(Figura* fig) {
	pair<float,float> pair = make_pair(fig->tl_original)
}*/

void inizializzaEntity() {

}

void resettaPosizione(Entity entity) {
	entity.posX = entity.defaultPosition.x;
	entity.posY = entity.defaultPosition.x;
}