#pragma once
/*Libreria con alcune definizioni utili (costanti, strutture), raggruppate qua per evitare di incasinare il resto del codice*/
#include "Lib.h"

#define ESC 27
#define PI 3.14159265358979323846
#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 800
#define PLAYER_SPEED 7
#define ENEMY_SPEED_1 4
#define ENEMY_SPEED_2 5
#define ENEMY_SPEED_3 3
#define MAX_VITE 3

typedef struct {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	int nTriangles;
	int radius;//TODO rimuovere questo campo se sicuro
	// Vertici
	vector<vec3> vertici;
	vector<vec3> CP;
	vector<vec4> colors;
	vector<int> indici;
	// Numero vertici
	int nv;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
	//Vertici della bounding box: TL sta per top left; BR sta per bottom right
	vec4 tl_original;	//original è prima di subire le trasformazioni di Model,
	vec4 tl_model;	//model è dopo ogni trasformazione
	vec4 br_original;
	vec4 br_model;
	//Larghezza e altezza della figura (e quindi della bounding box)
	float width;
	float height;
} Figura;

typedef struct {
	Figura figura;
	vec2 defaultPosition;
	float posX;
	float posY;
	float dx;
	float dy;
	float speed;
} Entity;

