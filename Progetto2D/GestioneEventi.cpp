/*Implementazione di GestioneEventi.h*/

/*void Player::buildColliders() {
	vec2 min = vertices.at(0);
	vec2 max = vertices.at(0); // minimun and maximum coordinates
	for (vec3 vert : vertices){
		if (min.x > vert.x) min.x = vert.x;
		if (min.y > vert.y) min.y = vert.y;
		if (max.x < vert.x) max.x = vert.x;
		if (max.y < vert.y) max.y = vert.y;
	}
	topLeftCorner.x = min.x;
	topLeftCorner.y = max.y;
	bottomRightCorner.x = max.x;
	bottomRightCorner.y = min.y;
}

*/
#include "Definizioni.h"

extern Entity player;
extern bool isPaused;
//devo aggiungere questi movimenti alla matrice di traslazione (chiamiamo gli spostamenti dx e dy)
void myKeyboard(unsigned char key, int x, int y) {
	//x e y che passo alla funzione sono le coordinate del mouse nella finestra quindi non mi servono
	switch (key)
	{
	case 'a':
		player.posX -= player.speed;
		break;
	case 'd':
		player.posX += player.speed;
		break;
	case 'w':
		player.posY += player.speed;
		break;
	case 's':
		player.posY -= player.speed;
		break;
	case 'p':
		printf("Prova pausa");
		isPaused = !isPaused;
	case ESC:
		printf("Prova uscita");
		exit(0);
	}

	glutPostRedisplay();
}

bool checkCollision(BoundingBox boundingBox1, BoundingBox boundingBox2) {
	
	bool collisionX = boundingBox1.bottomRightCorner.x >= boundingBox2.topLeftCorner.x &&
		boundingBox1.topLeftCorner.x <= boundingBox2.bottomRightCorner.x;

	bool collisionY = boundingBox1.bottomRightCorner.y <= boundingBox2.topLeftCorner.y &&
		boundingBox1.topLeftCorner.y >= boundingBox2.bottomRightCorner.y;

	if (collisionX && collisionY) 
		return true; 
	else 
		return false;
}