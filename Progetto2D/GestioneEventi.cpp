/*Implementazione di GestioneEventi.h*/

#include "Definizioni.h"
#include "GestioneEventi.h"

extern Entity player;
extern bool isPaused;
//devo aggiungere questi movimenti alla matrice di traslazione (chiamiamo gli spostamenti dx e dy)
void myKeyboard(unsigned char key, int x, int y) {
	//x e y che passo alla funzione sono le coordinate del mouse nella finestra quindi non mi servono
	switch (key)
	{
	case 'a':
		player.posX -= player.speed;
		player.figura.tl_model.x -= player.speed;
		player.figura.br_model.x -= player.speed;
		break;
	case 'd':
		player.posX += player.speed;
		player.figura.tl_model.x += player.speed;
		player.figura.br_model.x += player.speed;
		break;
	case 'w':
		player.posY += player.speed;
		player.figura.tl_model.y += player.speed;
		player.figura.br_model.y += player.speed;
		break;
	case 's':
		player.posY -= player.speed;
		player.figura.tl_model.y -= player.speed;
		player.figura.br_model.y -= player.speed;
		break;
	case 'p':
		printf("Prova pausa");
		isPaused = !isPaused;
	case ESC:
		printf("Prova uscita");
		exit(0);
	}
	//printf("Speed: %d\n", player.speed);
	//printf("PLAYER BB : %f; %f\n", player.boundingBox.topLeftCorner, player.boundingBox.bottomRightCorner);

	glutPostRedisplay();
}
//TODO fix
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

bool checkCollision(vec4 tl1, vec4 br1, vec4 tl2, vec4 br2) {

	bool collisionX = br1.x >=tl2.x &&
		tl1.x <= br2.x;

	bool collisionY = br1.y <= tl2.y &&
		tl1.y >= br2.y;

	if (collisionX && collisionY)
		return true;
	else
		return false;
}