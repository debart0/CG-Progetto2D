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
	int radius;
	// Vertici
	vector<vec3> vertici;
	vector<vec4> colors;
	// Numero vertici
	int nv;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
} Figura;

typedef struct {
	vec3 topLeftCorner;
	vec3 bottomRightCorner;
	mat4 Model;
} BoundingBox;

typedef struct {
	Figura figura;
	BoundingBox boundingBox;
	float posX;
	float posY;
	int speed;
} Entity;

