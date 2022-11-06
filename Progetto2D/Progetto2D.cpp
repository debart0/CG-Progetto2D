// Progetto2D.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"
#include "Definizioni.h"
#include "GestioneEventi.h"
#include "Utils.h"

////////////////////////////////////// Dichiarazione variabili //////////////////////////////////////

static unsigned int programId, MatModel, MatProj;
mat4 Projection;
float angolo = 0.0, s = 1, delta_x = 0, delta_y = 0, player_dx = 0, player_dy = 0;
float enemy_rotation_1 = 0.5, enemy_rotation_2 = -0.4, enemy_rotation_3 = 0.7;
vec2 player_default_pos = { WINDOW_WIDTH / 2, WINDOW_HEIGHT };
vec2 nemico1_default_pos = { 100.0, WINDOW_HEIGHT / 1.5 };
vec2 nemico2_default_pos = { 600.0 , WINDOW_HEIGHT/ 3};
vec2 nemico3_default_pos = {27.0,  WINDOW_HEIGHT / 4.5};

int drift_orizzontale = 1, gravity = 2;
int vite = MAX_VITE; int score = 0;
int pval = 140;
bool isPaused = false, gameOver = false, drawBB = false;
//Booleani posti a true se si usa il tasto a (spostamento a sinistra) e b (spostamento a destra)
bool pressing_left = false;
bool pressing_right = false;
vec4 asteroide_col_bot = { 0.181, 0.181, 0.185, 1.0 }, asteroide_col_top = { 0.076, 0.076, 0.084, 1.0 };
vec4 pod_col_primario = vec4(0.80, 0.82, 0.84, 1.0), pod_col_secondario = vec4(0.09, 0.10, 0.10, 1.0), pod_col_accenti = vec4(0.91, 0.58, 0.11, 1.0);
unsigned int loc_res, loc_mouse;
vec2 mouse, res;

vector<Figura> Scena;

Figura Pod = {}, Porta = {}, Antenna1 = {}, Antenna2 = {};
Figura Cuore = {};
Figura Asteroide = {}, Asteroide1 = {}, Asteroide2 = {}, Asteroide3 = {};
Entity player = {}, nemico1 = {}, nemico2 = {}, nemico3 = {};

void crea_VAO_Vector(Figura* fig)
{

	glGenVertexArrays(1, &fig->VAO);
	glBindVertexArray(fig->VAO);
	//Genero , rendo attivo, riempio il VBO della geometria dei vertici
	glGenBuffers(1, &fig->VBO_G);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_G);
	glBufferData(GL_ARRAY_BUFFER, fig->vertici.size() * sizeof(vec3), fig->vertici.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(0);

	//Genero , rendo attivo, riempio il VBO dei colori
	glGenBuffers(1, &fig->VBO_C);
	glBindBuffer(GL_ARRAY_BUFFER, fig->VBO_C);
	glBufferData(GL_ARRAY_BUFFER, fig->colors.size() * sizeof(vec4), fig->colors.data(), GL_STATIC_DRAW);
	//Adesso carico il VBO dei colori nel layer 2
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
	glEnableVertexAttribArray(1);

}

void costruisci_cuore(float cx, float cy, float raggiox, float raggioy, Figura* fig) {

	int i;
	float stepA = (2 * PI) / fig->nTriangles;
	float t;


	fig->vertici.push_back(vec3(cx, cy, 0.0));

	fig->colors.push_back(vec4(255.0 / 255.0, 75.0 / 255.0, 0.0, 1.0));

	for (i = 0; i <= fig->nTriangles; i++)
	{
		t = (float)i * stepA;
		fig->vertici.push_back(vec3(cx + raggiox * (16 * pow(sin(t), 3)) / 16, cy + raggioy * ((13 * cos(t) - 5 * cos(2 * t) - 2 * cos(3 * t) - cos(4 * t)) / 16), 0.0));
		//Colore 
		fig->colors.push_back(vec4(1.0, 204.0 / 255.0, 0.0, 1.0));


	}
	fig->nv = fig->vertici.size();

	pair<vec4, vec4> boundingBoxPair = calcolaBoundingBox(fig);
	fig->tl_original = boundingBoxPair.first;
	fig->br_original = boundingBoxPair.second;
	//printf("Bounding Box\nTOP LEFT: %f, %f\nBOT RIGHT: %f, %f\n", topLeftCorner.x, topLeftCorner.y, bottomRightCorner.x, bottomRightCorner.y);
}

void costruisci_cerchio(vec4 color_center, vec4 color_edges, Figura* fig) {

	int i;
	int triangles = fig->nTriangles;
	int radius = fig->radius;
	GLfloat twicePi = 2.0f * PI;


	fig->vertici.push_back(vec3(0.0, 0.0, 0.0));

	fig->colors.push_back(color_center);

	for (i = 0; i <= fig->nTriangles; i++)
	{
		fig->vertici.push_back(vec3(radius * cos(i * twicePi / triangles), radius * sin(i * twicePi / triangles), 0.0));
		//Colore 
		fig->colors.push_back(color_edges);


	}
	fig->nv = fig->vertici.size();

}

void costruisci_asteroide(vec4 color_top, vec4 color_bot, Figura* forma) {
	float* t;

	//forma->CP.push_back(vec3(0.0, 0.0, 0.0));
	forma->CP.push_back(vec3(-10, 0, 0.0));
	forma->CP.push_back(vec3(-9.5, 3, 0.0));
	forma->CP.push_back(vec3(-7, 6, 0.0));
	forma->CP.push_back(vec3(-6, 9, 0.0));
	forma->CP.push_back(vec3(-4,10, 0.0));
	forma->CP.push_back(vec3(-2, 9, 0.0));
	forma->CP.push_back(vec3(0, 7, 0.0));
	forma->CP.push_back(vec3(2, 6, 0.0));	

	forma->CP.push_back(vec3(5,7, 0.0));	//Questo punto sembra problematico
	forma->CP.push_back(vec3(9, 4, 0.0));
	forma->CP.push_back(vec3(10,2, 0.0));
	forma->CP.push_back(vec3(8, -1, 0.0));
	forma->CP.push_back(vec3(6.5, -2.5, 0.0));
	forma->CP.push_back(vec3(7, -7, 0.0));
	forma->CP.push_back(vec3(3,-10, 0.0));
	forma->CP.push_back(vec3(-3, -6, 0.0));
	forma->CP.push_back(vec3(-6, -5, 0.0));
	forma->CP.push_back(vec3(-9, -3, 0.0));

	forma->CP.push_back(vec3(-10, 0, 0.0));

	t = new float[forma->CP.size()];
	int i;
	float step = 1.0 / (float)(forma->CP.size() - 1);

	for (i = 0; i < forma->CP.size(); i++)
		t[i] = i * step;


	InterpolazioneHermite(t, forma, color_top, color_bot);
	forma->vertici.push_back(vec3(-10.0, 0.0, 0.0));
	//forma->vertici.push_back(vec3(0, 0, 0.0));
	forma->vertici.at(0) = vec3(0.0, 0.0, 0.0);	//QUA CAMBIO IL CENTRO DELLA MESH DI HERMITE PER IL MIO SCOPO
	forma->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	forma->nv = forma->vertici.size();

	pair<vec4, vec4> boundingBoxPair = calcolaBoundingBox(forma);
	forma->tl_original = boundingBoxPair.first;
	forma->br_original = boundingBoxPair.second;

}



void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_M.glsl";
	//char* fragmentShader = (char*)"fragmentShader_M_2.glsl";


	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void INIT_VAO(void) {
	//Cuore.nTriangles = 30;
	//costruisci_cuore(0, 0, 1, 1, &Cuore);
	costruisci_pod(pod_col_primario, pod_col_secondario, pod_col_accenti, &Pod);
	player.figura = Pod;
	//printf("\nPLAYER Bounding Box\t|\tTOP LEFT: %f, %f\tBOT RIGHT: %f, %f\n", player.figura.tl_original.x, player.figura.tl_original.y, player.figura.br_original.x, player.figura.br_original.y);

	crea_VAO_Vector(&player.figura);
	Scena.push_back(player.figura);

	//Gli asteroidi che fanno da "nemico"
	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide1);
	nemico1.figura = Asteroide1;
	crea_VAO_Vector(&Asteroide1);
	Scena.push_back(Asteroide1);
	//printf("\nENEMY Bounding Box\t|\tTOP LEFT: %f, %f\tBOT RIGHT: %f, %f\n", nemico1.figura.tl_original.x, nemico1.figura.tl_original.y, nemico1.figura.br_original.x, nemico1.figura.br_original.y);

	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide2);
	nemico2.figura = Asteroide2;
	crea_VAO_Vector(&Asteroide2);
	Scena.push_back(Asteroide2);

	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide3);
	nemico3.figura = Asteroide3;
	crea_VAO_Vector(&Asteroide3);
	Scena.push_back(Asteroide3);

	//Qua metto gli "extra", ovvero dettagli in più 
	/*costruisci_antenne(pod_col_accenti, &Antenna1, &Antenna2);
	crea_VAO_Vector(&player.figura);
	Scena.push_back(player.figura);*/

	Projection = ortho(0.0f, float(WINDOW_WIDTH), 0.0f, float(WINDOW_HEIGHT));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");

	loc_res = glGetUniformLocation(programId, "res");

	loc_mouse = glGetUniformLocation(programId, "mouse");
}

void drawScene(void)
{
	int k;
	res.x = (float)WINDOW_WIDTH;
	res.y = (float)WINDOW_HEIGHT;
	glUniform2f(loc_res, res.x, res.y);
	glUniform2f(loc_mouse, mouse.x, mouse.y);
	glClearColor(0.0, 1.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection)); //Questa può andare fuori dal ciclo xk nn cambia
	if (!gameOver)
	{
		Scena[0].Model = mat4(1.0); //Inizializzo con l'identità
		Scena[0].Model = translate(Scena[0].Model, vec3(player.posX, player.posY, 0.0));
		//Scena[0].Model = translate(Scena[0].Model, vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.0)); 
		//Scommenta per testare la figura

		Scena[0].Model = scale(Scena[0].Model, vec3(5, 5, 1.0));
		//Scena[k].Model = rotate(Scena[0].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
		//printf("PLAYER POS : %f; %f\n", player.posX, player.posY);

		vec4 br = player.figura.br_original; vec4 tl = player.figura.tl_original;
		br = Scena[0].Model * br; tl = Scena[0].Model * tl;

		player.figura.br_model = br;
		player.figura.tl_model = tl;
		//printf("PLAYER BB : %f, %f --- %f, %f\n\n", player.figura.tl_model.x, player.figura.tl_model.y, player.figura.br_model.x, player.figura.br_model.y);
	
	
		Scena[1].Model = mat4(1.0); //Inizializzo con l'identità
		Scena[1].Model = translate(Scena[1].Model, vec3(nemico1.posX, nemico1.posY, 0.0));
		Scena[1].Model = scale(Scena[1].Model, vec3(5, 5, 1.0));

		br = nemico1.figura.br_original;  tl = nemico1.figura.tl_original;
		br = Scena[1].Model * br; tl = Scena[1].Model * tl;

		Scena[1].Model = rotate(Scena[1].Model, radians(angolo * enemy_rotation_1), vec3(0.0, 0.0, 1.0));

		nemico1.figura.br_model = br;
		nemico1.figura.tl_model = tl;
		//printf("dx del nemico 1: %f\n", nemico1.dx);
		//printf("ENEMY BB : %f, %f --- %f, %f\n\n", nemico1.figura.tl_model.x, nemico1.figura.tl_model.y, nemico1.figura.br_model.x, nemico1.figura.br_model.y);
	

	
		Scena[2].Model = mat4(1.0); //Inizializzo con l'identità
		Scena[2].Model = translate(Scena[2].Model, vec3(nemico2.posX, nemico2.posY, 0.0));
		Scena[2].Model = scale(Scena[2].Model, vec3(7, 7, 1.0));

		 br = nemico2.figura.br_original;  tl = nemico2.figura.tl_original;
		br = Scena[2].Model * br; tl = Scena[2].Model * tl;

		Scena[2].Model = rotate(Scena[2].Model, radians(angolo*enemy_rotation_2), vec3(0.0, 0.0, 1.0));

		nemico2.figura.br_model = br;
		nemico2.figura.tl_model = tl;
		//printf("posx del nemico 2: %f\n", nemico2.posX);

	


	
		Scena[3].Model = mat4(1.0); //Inizializzo con l'identità
		Scena[3].Model = translate(Scena[3].Model, vec3(nemico3.posX, nemico3.posY, 0.0));
		Scena[3].Model = scale(Scena[3].Model, vec3(4, 6, 1.0));

		 br = nemico3.figura.br_original;  tl = nemico3.figura.tl_original;
		br = Scena[3].Model * br; tl = Scena[3].Model * tl;

		Scena[3].Model = rotate(Scena[3].Model, radians(angolo*enemy_rotation_3), vec3(0.0, 0.0, 1.0));

		nemico3.figura.br_model = br;
		nemico3.figura.tl_model = tl;
		//printf("posx del nemico 2: %f\n", nemico2.posX);

	

	//Devo passare allo shader le info su dove andare a prendere le informazioni sulle variabili uniformi
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[0].Model));
		glBindVertexArray(Scena[0].VAO);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		/*glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[0].nv - 18 - 30);
		glDrawArrays(GL_TRIANGLES, Scena[0].nv - 18 - 30,12);
		glDrawArrays(GL_TRIANGLE_FAN, Scena[0].nv - 6 - 30, 30);*/
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[0].nv - 24);
		glDrawArrays(GL_TRIANGLES, Scena[0].nv - 24, 12);
		glDrawArrays(GL_TRIANGLES, Scena[0].nv - 12, 3);
		glDrawArrays(GL_TRIANGLES, Scena[0].nv - 9, 3);
		//glDrawArrays(GL_TRIANGLE_FAN, Scena[0].nv - 12, 6);
		
		for (int k = 1; k <= 3; k++) {
			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
			glBindVertexArray(Scena[k].VAO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[k].nv - 6);
		}
		
		//Disegno Bounding Box
		if (drawBB == TRUE)
		{
			for (int j = 0; j <= 3; j++) {
				glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[j].Model));
				glBindVertexArray(Scena[j].VAO);
				glDrawArrays(GL_LINE_STRIP, Scena[j].nv - 6, 6);
				glBindVertexArray(0);

			}
		}

		if (checkCollision(player.figura, nemico1.figura) || checkCollision(player.figura, nemico2.figura)) {
			printf("COLLISIONE\n");
			//resetta posizione
			//togli una vita
			//controlla che le vite non siano finite
			resettaPosizionePlayer();
			vite -= 1;
			if (vite <= 0) {
				gameOver = true;
			}
		}
	}
	
	else {
		printf("GAME OVER\n");
	}


	glutSwapBuffers();

}

void updateAngolo(int value) {
	angolo += 1;
	glutTimerFunc(60, updateAngolo, 0);

	glutPostRedisplay();
}

void updateCar(int a)
{
	/*if (!paused && !ded) {
		if (asteroidMoving) {
			vec2 angleVec = vec2(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2) - asteroid;
			angleVec = normalize(angleVec);
			asteroid = vec2((asteroid.x + angleVec.x * asteroidSpeed), (asteroid.y + angleVec.y * asteroidSpeed));
			if (((asteroid.x > WINDOW_WIDTH / 2 - WINDOW_WIDTH / 10 && asteroid.x < WINDOW_WIDTH / 2) || (asteroid.x < WINDOW_WIDTH / 2 + WINDOW_WIDTH / 10 && asteroid.x > WINDOW_WIDTH / 2)) &&
				((asteroid.y > WINDOW_HEIGHT / 2 - WINDOW_HEIGHT / 10 && asteroid.y < WINDOW_HEIGHT / 2) || (asteroid.y < WINDOW_HEIGHT / 2 + WINDOW_HEIGHT / 10 && asteroid.y > WINDOW_HEIGHT / 2))) {
				ded = true;
			}
		}
		else {
			float newX = 0.0f, newY = 0.0f;
			//int axisRand = linearRand(0, 3);
			switch (axisRand) {
				//Parte da sotto
			case 0:
				newY = 0;
				newX = linearRand(0, 1280);
				break;
			case 1:
				newY = 720;
				newX = linearRand(0, 1280);
				break;
			case 2:
				newX = 0;
				newY = linearRand(0, 720);
				break;
			case 3:
				newX = 1280;
				newY = linearRand(0, 720);
				break;
			}
			asteroid = vec2(newX, newY);
			asteroidMoving = true;
		}
	}
	//glutSetWindow(idfg);
	glutPostRedisplay();
	//glutSetWindow(idfi);
	glutPostRedisplay();
	//glutTimerFunc(24, update_asteroid, 0);*/
}

void main_menu_func(int option)
{
	switch (option)
	{
	case 1: glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		break;
	case 2: glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		break;

	default:
		break;
	}
}

void buildOpenGLMenu()
{

	glutCreateMenu(main_menu_func); // richiama main_menu_func() alla selezione di una voce menu
	glutAddMenuEntry("Opzioni", -1); //-1 significa che non si vuole gestire questa riga

	glutAddMenuEntry("Wireframe", 1);
	glutAddMenuEntry("Face fill", 2);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Progetto 2D");
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(keyboardPressedEvent);
	glutKeyboardUpFunc(keyboardReleasedEvent);
	glutPassiveMotionFunc(mouseClick);

	//TODO 
	inizializzaEntity();
	glutTimerFunc(25, update, 0);
	glutTimerFunc(25, updateAngolo, 0);
	glutTimerFunc(60, updateAsteroidi, 0);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	buildOpenGLMenu();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();
}