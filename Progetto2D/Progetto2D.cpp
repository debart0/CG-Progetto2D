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
int player_x = WINDOW_WIDTH / 2, player_y = WINDOW_HEIGHT - 50;
int drift_orizzontale = 1, gravity = 2;
int vite = MAX_VITE; int score = 0;
int pval = 140;
bool isPaused = false;
//Booleani posti a true se si usa il tasto a (spostamento a sinistra) e b (spostamento a destra)
bool pressing_left = false;
bool pressing_right = false;
vec4 asteroide_col_bot = { 0.181, 0.181, 0.185, 1.0 }; vec4 asteroide_col_top = { 0.076, 0.076, 0.084, 1.0 };
vec2 mouse;

vector<Figura> Scena;

Figura Farf = {};
Figura Cuore = {};
Figura Asteroide = {}; Figura Asteroide1 = {}; Figura Asteroide2 = {};
Entity player = {};

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

/// /////////////////////////////////// Disegna geometria //////////////////////////////////////
//Per Curve di hermite
#define PHI0(t)  (2.0*t*t*t-3.0*t*t+1)
#define PHI1(t)  (t*t*t-2.0*t*t+t)
#define PSI0(t)  (-2.0*t*t*t+3.0*t*t)
#define PSI1(t)  (t*t*t-t*t)
float dx(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig)
{
	if (i == 0)
		return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP[i + 1].x - Fig->CP[i].x) / (t[i + 1] - t[i]);
	if (i == Fig->CP.size() - 1)
		return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP[i].x - Fig->CP[i - 1].x) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (Fig->CP.at(i).x - Fig->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).x - Fig->CP.at(i).x) / (t[i + 1] - t[i]);
	else
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (Fig->CP.at(i).x - Fig->CP.at(i - 1).x) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (Fig->CP.at(i + 1).x - Fig->CP.at(i).x) / (t[i + 1] - t[i]);
}
float dy(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig)
{
	if (i == 0)
		return 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
	if (i == Fig->CP.size() - 1)
		return  0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]);

	if (i % 2 == 0)
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 + Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 - Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
	else
		return  0.5 * (1 - Tens) * (1 + Bias) * (1 - Cont) * (Fig->CP.at(i).y - Fig->CP.at(i - 1).y) / (t[i] - t[i - 1]) + 0.5 * (1 - Tens) * (1 - Bias) * (1 + Cont) * (Fig->CP.at(i + 1).y - Fig->CP.at(i).y) / (t[i + 1] - t[i]);
}

void InterpolazioneHermite(float* t, Figura* Fig, vec4 color_top, vec4 color_bot)
{
	float p_t = 0, p_b = 0, p_c = 0, x, y;
	float passotg = 1.0 / (float)(pval - 1);
	float passotg0 = 1.0 / 10.0;
	float tg = 0, tgmapp, ampiezza;
	int i = 0;
	int is = 0; //indice dell'estremo sinistro dell'intervallo [t(i),t(i+1)] a cui il punto tg
				//appartiene

	Fig->vertici.push_back(vec3(-1.0, 5.0, 0.0));
	//Fig->colors.push_back(vec4(1.0, 1.0, 0.0, 1.0));
	Fig->colors.push_back(color_bot);




	for (tg = 0; tg <= 1; tg += passotg)
	{
		if (tg > t[is + 1]) is++;

		ampiezza = (t[is + 1] - t[is]);
		tgmapp = (tg - t[is]) / ampiezza;

		x = Fig->CP[is].x * PHI0(tgmapp) + dx(is, t, p_t, p_b, p_c, Fig) * PHI1(tgmapp) * ampiezza + Fig->CP[is + 1].x * PSI0(tgmapp) + dx(is + 1, t, p_t, p_b, p_c, Fig) * PSI1(tgmapp) * ampiezza;
		y = Fig->CP[is].y * PHI0(tgmapp) + dy(is, t, p_t, p_b, p_c, Fig) * PHI1(tgmapp) * ampiezza + Fig->CP[is + 1].y * PSI0(tgmapp) + dy(is + 1, t, p_t, p_b, p_c, Fig) * PSI1(tgmapp) * ampiezza;
		Fig->vertici.push_back(vec3(x, y, 0.0));
		Fig->colors.push_back(color_top);

	}


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
}

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_M.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void INIT_VAO(void) {
	Cuore.nTriangles = 30;
	costruisci_cuore(0, 0, 1, 1, &Cuore);
	player.figura = Cuore;
	/*printf("PLAYER Bounding Box\nTOP LEFT: %f, %f\nBOT RIGHT: %f, %f\n", player.boundingBox.topLeftCorner.x, player.boundingBox.topLeftCorner.y, player.boundingBox.bottomRightCorner.x, player.boundingBox.bottomRightCorner.y);*/
	printf("\nPLAYER Bounding Box\t|\tTOP LEFT: %f, %f\tBOT RIGHT: %f, %f\n", player.figura.tl_original.x, player.figura.tl_original.y, player.figura.br_original.x, player.figura.br_original.y);

	crea_VAO_Vector(&player.figura);
	Scena.push_back(player.figura);

	//Asteroide1.nTriangles = 30;
	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide1);
	crea_VAO_Vector(&Asteroide1);
	Scena.push_back(Asteroide1);

	//Asteroide2.nTriangles = 30;
	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide2);
	crea_VAO_Vector(&Asteroide2);
	Scena.push_back(Asteroide2);

	//vec4 col_bottom = vec4{ 0.5451, 0.2706, 0.0745, 1.0000 };

	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide);
	crea_VAO_Vector(&Asteroide);
	Scena.push_back(Asteroide);
	Projection = ortho(0.0f, float(WINDOW_WIDTH), 0.0f, float(WINDOW_HEIGHT));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");
}

void drawScene(void)
{
	int k;

	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection)); //Questa può andare fuori dal ciclo xk nn cambia
	for (k = 0; k < Scena.size(); k++)
	{
		if (k == 0) {//Player
			Scena[k].Model = mat4(1.0); //Inizializzo con l'identità
			Scena[k].Model = translate(Scena[k].Model, vec3(player.posX, player.posY, 0.0));	
			Scena[k].Model = scale(Scena[k].Model, vec3(50, 50, 1.0));
			//vec4 bb1 = Scena[k].Model * vec4(player.boundingBox.bottomRightCorner.x, player.boundingBox.bottomRightCorner.y, player.boundingBox.bottomRightCorner.y, 1);
			//vec4 bb2 = Scena[k].Model * vec4(player.boundingBox.topLeftCorner.x, player.boundingBox.topLeftCorner.y, player.boundingBox.topLeftCorner.y, 1);
			//player.boundingBox.bottomRightCorner = vec3(bb1.x, bb1.y, bb1.z);
			//player.boundingBox.topLeftCorner = vec3(bb2.x, bb2.y, bb2.z);
			
			
			//Scena[k].Model = rotate(Scena[k].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
			//printf("PLAYER POS : %f; %f\n", player.posX, player.posY);

			vec4 br = player.figura.br_original; vec4 tl = player.figura.tl_original;
			//tl = vec3(tl.x * 0.5 + player.posX, tl.y * 0.5 + player.posY, 1.0);
			//br = vec3(br.x * 0.5 + player.posX, br.y * 0.5 + player.posY, 1.0);
			br = Scena[k].Model * br; tl = Scena[k].Model * tl;

			player.figura.br_model = br;
			player.figura.tl_model = tl;
			printf("PLAYER BB : %f, %f --- %f, %f\n\n", player.figura.tl_model.x, player.figura.tl_model.y, player.figura.br_model.x, player.figura.br_model.y);
			
		}

		if (k == 1) {
			Scena[k].Model = mat4(1.0); //Inizializzo con l'identità
			Scena[k].Model = translate(Scena[k].Model, vec3(200 + delta_x, WINDOW_HEIGHT/6 + delta_y, 0.0));
			Scena[k].Model = scale(Scena[k].Model, vec3(50, 50, 1.0));
			Scena[k].Model = rotate(Scena[k].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
		}

		if (k == 2) {
			Scena[k].Model = mat4(1.0); //Inizializzo con l'identità
			Scena[k].Model = translate(Scena[k].Model, vec3(600 + delta_x, WINDOW_HEIGHT / 2 + delta_y, 0.0));
			Scena[k].Model = scale(Scena[k].Model, vec3(50 * s, 50 * s, 1.0));
			Scena[k].Model = rotate(Scena[k].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
		}


		if (k == 1) {
			Scena[k].Model = mat4(1.0); //Inizializzo con l'identità
			Scena[k].Model = translate(Scena[k].Model, vec3(300 + delta_x, WINDOW_HEIGHT / 2 + delta_y, 0.0));
			Scena[k].Model = scale(Scena[k].Model, vec3(20 * s, 20 * s, 1.0));
			Scena[k].Model = rotate(Scena[k].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
		}

		//Devo passare allo shader le info su dove andare a prendere le informazioni sulle variabili uniformi
		glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
		glBindVertexArray(Scena[k].VAO);
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[k].nv);
	}

	glutSwapBuffers();

}

void updateAngolo(int value) {
	angolo += 1;
	s = s * 1.1;
	if (s > 2) {
		s = 1;
	}
	glutTimerFunc(50, updateAngolo, 0);

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

	player.posX = player_x; player.posY = player_y; player.speed = PLAYER_SPEED;

	glutTimerFunc(66, update, 0);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	buildOpenGLMenu();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();
}