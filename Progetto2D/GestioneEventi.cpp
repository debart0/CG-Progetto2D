/*Implementazione di GestioneEventi.h*/

#include "Definizioni.h"
#include "Utils.h"
#include "GestioneEventi.h"

extern Entity player, nemico1, nemico2, nemico3;
extern bool isPaused, gameOver, drawBB;
extern bool pressing_left, pressing_right, moving;
extern int drift_orizzontale, gravity, score, vite, width, height;
extern float angolo, w_update, h_update, dy_asteroide, player_dx, player_dy, attrito;
extern vec2 mouse;
extern mat4 Projection;

void keyboardPressedEvent(unsigned char key, int x, int y) {
	//x e y che passo alla funzione sono le coordinate del mouse nella finestra quindi non mi servono
	switch (key)
	{
	case 'a':
		pressing_left = true;
		break;
	case 'd':
		pressing_right = true;
		break;
	case 'b':
		drawBB = TRUE;
		break;
	case 'p':
		printf("\n----------------------PAUSA------------------------\n");
		isPaused = !isPaused;
		break;
	case ESC:
		printf("\n----------------------USCITA------------------------\n");
		exit(0);
		break;
	}
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
	case 'b':
		drawBB = FALSE;
		break;
	default:
		break;
	}

}

void specialKeyPressedEvent(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		pressing_left = true;
		break;
	case GLUT_KEY_RIGHT:
		pressing_right = true;
	default:
		break;
	}
}

void specialKeyReleasedEvent(int key, int x, int y) {
	switch (key)
	{
	case GLUT_KEY_LEFT:
		pressing_left = false;
		break;
	case GLUT_KEY_RIGHT:
		pressing_right = false;
		break;
	default:
		break;
	}
}
void reshape(int w, int h) {
	Projection = ortho(0.0f, (float)width, 0.0f, (float)height);

	float AspectRatio_mondo = (float)(width) / (float)(height); //Rapporto larghezza altezza di tutto ciò che è nel mondo
	 //Se l'aspect ratio del mondo è diversa da quella della finestra devo mappare in modo diverso 
	 //per evitare distorsioni del disegno
	if (AspectRatio_mondo > (float)w / (float)h)   //Se ridimensioniamo la larghezza della Viewport
	{
		glViewport(0, 0, w, w / AspectRatio_mondo);
		w_update = (float)w;
		h_update = w / AspectRatio_mondo;

	}
	else {  //Se ridimensioniamo la larghezza della viewport oppure se l'aspect ratio tra la finestra del mondo 
			//e la finestra sullo schermo sono uguali
		glViewport(0, 0, h * AspectRatio_mondo, h);
		w_update = h * AspectRatio_mondo;
		h_update = (float)h;
	}

}

void mouseClick(int x, int y)
{

	mouse.x = (float)x;
	mouse.y = (float)y;

	glutPostRedisplay();
}

bool checkCollision(vec4 tl1, vec4 br1, vec4 tl2, vec4 br2) {

	bool collisionX = br1.x >=tl2.x &&
		tl1.x <= br2.x;

	bool collisionY = br1.y <= tl2.y &&
		tl1.y >= br2.y;

	return collisionX && collisionY;
}

bool checkCollision(Figura fig1, Figura fig2) {
	bool collisionX = fig1.BR_model.x >= fig2.TL_model.x &&
		fig1.TL_model.x <= fig2.BR_model.x;

	bool collisionY = fig1.BR_model.y <= fig2.TL_model.y &&
		fig1.TL_model.y >= fig2.BR_model.y;
	//Si ha collisione se c'è collisione sia nella direzione x che nella direzione y
	if (collisionX && collisionY) {
		printf("\n----------------------CHECK COLLISION------------------------\n");
		printf("FIG 1  : %f, %f --- %f, %f\n", fig1.TL_model.x, fig1.TL_model.y, fig1.BR_model.x, fig1.BR_model.y);
		printf("FIG 2  : %f, %f --- %f, %f\n", fig2.TL_model.x, fig2.TL_model.y, fig2.BR_model.x, fig2.BR_model.y);
	}
	return collisionX && collisionY;
}

void update(int a) {
	
	if (!isPaused && !gameOver) {
		bool moving = false;
		float width = player.figura.BR_model.x - player.figura.TL_model.x;
		float height = player.figura.TL_model.y - player.figura.BR_model.y;
		player_dy = -gravity;
		//Se sto premendo i tasti per muovermi, sposto la X di una quantità 
		if (pressing_left){
			player_dx = -drift_orizzontale;
			moving = true;
		}
		if (pressing_right){
			player_dx = drift_orizzontale;
			moving = true;
		}
		//Se non sto premendo i tasti per muovermi, lentamente freno il movimento del giocatore.
		if (!moving) {
			if (player_dx >= attrito)
			{
				player_dx -= attrito;

				if (player_dx < attrito)
					player_dx = attrito;
			}
			if (player_dx <= -attrito)
			{
				player_dx += attrito;
				if (player_dx > -attrito)
					player_dx = -attrito;
			}
		}

		//Aggiornamento della posizione del player
		player.posX += player_dx;
		player.posY += player_dy;

		//Controllo che il giocatore non vada "Out of bounds", ovvero fuori dai lati della finestra
		if (player.posX <= width / 2) {
			player.posX = width / 2	;
			player_dx = 0;
		}
		if (player.posX > WINDOW_WIDTH - width / 2) {
			player.posX = WINDOW_WIDTH - width / 2;
			player_dx = 0;
		}
		//Se il giocatore esce totalmente dalla parte inferiore della finestra,
		//allora aggiungo un punto e lo faccio ripartire
		if (player.posY < 0 - height /2) {
			resettaPosizionePlayer();
			score += 1;
			printf("Score: %d\n", score);
		}
	}
	glutPostRedisplay();
	glutTimerFunc(24, update, 0);
}

void updateAsteroidi(int a) {
	if (!isPaused) {
		//Posso anche non usare width per la larghezza, mi dovrebbero bastare le coordinate dei corner
		nemico1.posX += nemico1.speed;
		nemico2.posX -= nemico2.speed;
		nemico3.posX += nemico3.speed;
		dy_asteroide = dy_asteroide + 0.1;
		//Se il nemico esce da un lato della finestra, allora lo faccio apparire nel lato opposto
		if (nemico1.figura.TL_model.x >= WINDOW_WIDTH) {
			float width_nemico1 = nemico1.figura.BR_model.x - nemico1.posX;
			nemico1.posX = -width_nemico1;
		}
		if (nemico2.figura.BR_model.x <= 0) {
			float width_nemico2 = nemico2.figura.BR_model.x - nemico2.posX;
			nemico2.posX = WINDOW_WIDTH + width_nemico2;
		}
		if (nemico3.figura.TL_model.x >= WINDOW_WIDTH) {
			float width_nemico3 = nemico3.figura.BR_model.x - nemico3.posX;
			nemico3.posX = -width_nemico3;
		}
	}
	
	glutTimerFunc(24, updateAsteroidi, 0);
}

void updateAngolo(int value) {
	if (!isPaused) {
		angolo += 1;
	}
	glutTimerFunc(60, updateAngolo, 0);
}