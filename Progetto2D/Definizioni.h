#pragma once
/*Libreria con alcune definizioni utili (costanti, strutture), raggruppate qua per evitare di incasinare il resto del codice*/
#include "Lib.h"

#define ESC 27
#define PI 3.14159265358979323846
#define width 800
#define height 800
#define PLAYER_SPEED 6

typedef struct {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	int nTriangles;
	int radius;//TODO rimuovere questo campo se sicuro
	// Vertici
	vector<vec3> vertici;
	vector<vec4> colors;
	// Numero vertici
	int nv;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
	//Vertici della bounding box: TL sta per top left; BR sta per bottom right
	vec4 tl_original;	//original è prima di subire le trasformazioni di Model,
	vec4 tl_model;	//model è dopo ogni trasformazione
	vec4 br_original;
	vec4 br_model;
} Figura;

typedef struct {
	Figura figura;
	float posX;
	float posY;
	int speed;
} Entity;

