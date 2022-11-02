/*Implementazione di GestioneEventi.h*/

#include "Definizioni.h"
#include "Utils.h"
#include "GestioneEventi.h"

extern Entity player, nemico1, nemico2, nemico3;
extern bool isPaused, gameOver;
extern bool pressing_left, pressing_right, moving;
extern int player_x, player_y, drift_orizzontale, gravity, score, vite;
extern vec2 mouse;
//devo aggiungere questi movimenti alla matrice di traslazione (chiamiamo gli spostamenti dx e dy)
void keyboardPressedEvent(unsigned char key, int x, int y) {
	//x e y che passo alla funzione sono le coordinate del mouse nella finestra quindi non mi servono
	switch (key)
	{
	case 'a':
		pressing_left = true;
		player.posX -= player.speed;
		/*player.figura.tl_model.x -= player.speed;
		player.figura.br_model.x -= player.speed;*/
		break;
	case 'd':
		pressing_right = true;
		player.posX += player.speed;
		/*player.figura.tl_model.x += player.speed;
		player.figura.br_model.x += player.speed;*/
		break;
	case 'p':
		printf("Prova pausa");
		isPaused = !isPaused;
		break;
	case ESC:
		printf("Prova uscita");
		exit(0);
		break;
	}
	//printf("Speed: %d\n", player.speed);
	//printf("PLAYER BB : %f; %f\n", player.boundingBox.topLeftCorner, player.boundingBox.bottomRightCorner);

}

void keyboardReleasedEvent(unsigned char key, int x, int y) {
	switch (key)
	{
	case 'a':
		pressing_left = false;
		break;
	case 'd':
		pressing_right = false;
		break;
	default:
		break;
	}

}


void mouseClick(int x, int y)
{

	mouse.x = (float)x;
	mouse.y = (float)y;

}

bool checkCollision(vec4 tl1, vec4 br1, vec4 tl2, vec4 br2) {

	bool collisionX = br1.x >=tl2.x &&
		tl1.x <= br2.x;

	bool collisionY = br1.y <= tl2.y &&
		tl1.y >= br2.y;

	return collisionX && collisionY;
}

bool checkCollision(Figura fig1, Figura fig2) {
	bool collisionX = fig1.br_model.x >= fig2.tl_model.x &&
		fig1.tl_model.x <= fig2.br_model.x;

	bool collisionY = fig1.br_model.y <= fig2.tl_model.y &&
		fig1.tl_model.y >= fig2.br_model.y;
	//Si ha collisione se c'� collisione sia nella direzione x che nella direzione y

	return collisionX && collisionY;
}

void update(int a) {
	
	if (!isPaused) {
		if (checkCollision(player.figura, nemico1.figura) || checkCollision(player.figura, nemico2.figura) || checkCollision(player.figura, nemico3.figura)) {
			printf("COLLISIONE\n");
			//resetta posizione
			//togli una vita
			//controlla che le vite non siano finite
			resettaPosizione(player);
			resettaPosizione(nemico1);
			resettaPosizione(nemico2);
			resettaPosizione(nemico3);
			vite -= 1;
			if (vite <= 0) {
				gameOver = true;
			}
		}
		bool moving = false;
		float width = player.figura.br_model.x - player.figura.tl_model.x;
		float height = player.figura.tl_model.y - player.figura.br_model.y;
		//printf("posizione player: %f\n",player.posX);
		player.posY -= gravity;
		if (pressing_left){
			//dx -= accelerazione;
			player.posX -= drift_orizzontale;
			moving = true;
		}

		if (pressing_right){
			//dx += accelerazione;
			player.posX += drift_orizzontale;
			moving = true;
		}

		if (!moving) {   //Se non mi sto muovendo con i tasti a e d decremento od incremento la velocita' iniziale fino a portarla
					 // a zero e la palla continua a rimbalzare sul posto
			/*
			if (dx > 0)
			{
				dx -= 1;
				if (dx < 0)
					dx = 0;
			}

			if (dx < 0)
			{
				dx += 1;
				if (dx > 0)
					dx = 0;
			}*/

		}

		if (player.posX <= width / 2) {
			player.posX = width / 2	;
		}
		if (player.posX > WINDOW_WIDTH - width / 2) {
			player.posX = WINDOW_WIDTH - width / 2;
		}
		if (player.posY < 0 - height /2) {
			player.posY = player_y;
			player.posX = player_x;
			score += 1;
			printf("Score: %d\n", score);
		
	}
	//TODO FARE QUALCOSA PER AUMENTARE LO SCORE
	}
	glutPostRedisplay();
	glutTimerFunc(24, update, 0);
}