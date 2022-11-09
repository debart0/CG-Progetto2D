// Progetto2D.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"
#include "Definizioni.h"
#include "GestioneEventi.h"
#include "Utils.h"
#include "GestioneTesto.h"
#include <ft2build.h>
#include FT_FREETYPE_H

////////////////////////////////////// Dichiarazione variabili //////////////////////////////////////

static unsigned int programId, programId_text, programId_fx, MatModel, MatProj;
unsigned int VAO_Text, VBO_Text;

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
string vite_string, score_string, gameover_string = "GAME OVER";
bool isPaused = false, gameOver = false, drawBB = false;
//Booleani posti a true se si usa il tasto a (spostamento a sinistra) e b (spostamento a destra)
bool pressing_left = false;
bool pressing_right = false;
vec4 asteroide_col_bot = { 0.181, 0.181, 0.185, 1.0 }, asteroide_col_top = { 0.076, 0.076, 0.084, 1.0 };
vec4 pod_col_primario = vec4(0.80, 0.82, 0.84, 1.0), pod_col_secondario = vec4(0.09, 0.10, 0.10, 1.0), pod_col_accenti = vec4(0.91, 0.58, 0.11, 1.0);
vec4 fondale_col_top = vec4(0.02, 0.13, 0.29, 1.0), fondale_col_bottom = vec4(0.26, 0.05, 0.29, 1.0);
vec4 colore_testo = vec4(0.82, 0.61, 0.18, 1.0), colore_testo_alt = vec4(0.80, 0.14, 0.05, 1.0);
unsigned int locres, locmouse, loctime, lsceltavs, lsceltafs;
vec2 mouse, res;

vector<Figura> Scena;

Figura Pod = {}, Porta = {}, Antenna1 = {}, Antenna2 = {};
Figura Cuore = {}, Fondale = {};
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
/*void costruisci_cuore(float cx, float cy, float raggiox, float raggioy, Figura* fig) {

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
	fig->TL_original = boundingBoxPair.first;
	fig->BR_original = boundingBoxPair.second;
	//printf("Bounding Box\nTOP LEFT: %f, %f\nBOT RIGHT: %f, %f\n", topLeftCorner.x, topLeftCorner.y, bottomRightCorner.x, bottomRightCorner.y);
}*/

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





void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_M.glsl";
	//char* fragmentShader = (char*)"FS_mare.glsl";


	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	//Generazione del program shader per la gestione del testo
	vertexShader = (char*)"VertexShader_Text.glsl";
	fragmentShader = (char*)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);

	vertexShader = (char*)"vertexShader_M.glsl";
	fragmentShader = (char*)"FS_speciale.glsl";
	programId_fx = ShaderMaker::createProgram(vertexShader, fragmentShader);

}

void INIT_VAO(void) {
	Pod.sceltaFS = 0;
	Pod.sceltaVS = 0;
	costruisci_pod(pod_col_primario, pod_col_secondario, pod_col_accenti, &Pod);
	player.figura = Pod;
	//printf("\nPLAYER Bounding Box\t|\tTOP LEFT: %f, %f\tBOT RIGHT: %f, %f\n", player.figura.tl_original.x, player.figura.tl_original.y, player.figura.br_original.x, player.figura.br_original.y);

	crea_VAO_Vector(&player.figura);
	Scena.push_back(player.figura);

	//Gli asteroidi che fanno da "nemico"
	Asteroide1.sceltaFS = 0;
	Asteroide1.sceltaVS = 0;
	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide1);
	nemico1.figura = Asteroide1;
	crea_VAO_Vector(&Asteroide1);
	Scena.push_back(Asteroide1);
	//printf("\nENEMY Bounding Box\t|\tTOP LEFT: %f, %f\tBOT RIGHT: %f, %f\n", nemico1.figura.tl_original.x, nemico1.figura.tl_original.y, nemico1.figura.br_original.x, nemico1.figura.br_original.y);

	Asteroide2.sceltaFS = 0;
	Asteroide2.sceltaVS = 0;
	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide2);
	nemico2.figura = Asteroide2;
	crea_VAO_Vector(&Asteroide2);
	Scena.push_back(Asteroide2);

	Asteroide3.sceltaFS = 0;
	Asteroide3.sceltaVS = 0;
	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide3);
	nemico3.figura = Asteroide3;
	crea_VAO_Vector(&Asteroide3);
	Scena.push_back(Asteroide3);

	Fondale.sceltaFS = 1;
	Fondale.sceltaVS = 0;
	costruisci_fondale(fondale_col_top, fondale_col_bottom, &Fondale);
	crea_VAO_Vector(&Fondale);
	Scena.push_back(Fondale);

	//Qua metto gli "extra", ovvero dettagli in più 
	/*costruisci_antenne(pod_col_accenti, &Antenna1, &Antenna2);
	crea_VAO_Vector(&player.figura);
	Scena.push_back(player.figura);*/

	Projection = ortho(0.0f, float(WINDOW_WIDTH), 0.0f, float(WINDOW_HEIGHT));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");

	lsceltavs = glGetUniformLocation(programId, "sceltaVS");
	lsceltafs = glGetUniformLocation(programId, "sceltaFS");
	locres = glGetUniformLocation(programId, "res");
	loctime = glGetUniformLocation(programId, "time");
	locmouse = glGetUniformLocation(programId, "mouse");
}

void INIT_VAO_Text(void)
{

	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenVertexArrays(1, &VAO_Text);
	glGenBuffers(1, &VBO_Text);
	glBindVertexArray(VAO_Text);
	glBindBuffer(GL_ARRAY_BUFFER, VBO_Text);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}


void drawScene(void)
{
	vite_string = "VITE: " + to_string(vite);
	score_string = "PUNTI: " + to_string(score);
	int k;
	res.x = (float)WINDOW_WIDTH;
	res.y = (float)WINDOW_HEIGHT;
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;
	

	glUniform1f(loctime, time);
	glUniform2f(locres, res.x, res.y);
	glUniform2f(locmouse, mouse.x, mouse.y);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection)); //Questa può andare fuori dal ciclo xk nn cambia
	
	//FONDALE, prima disegno il fondale perché deve stare dietro alla scena
	/*glUseProgram(programId_fx);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));*/
	glUniform1i(lsceltavs, Scena[4].sceltaVS);
	glUniform1i(lsceltafs, Scena[4].sceltaFS);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[4].Model));
	glBindVertexArray(Scena[4].VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, Scena[4].nv);
	//glUseProgram(programId);
	if (!gameOver)
	{
		//PLAYER
		Scena[0].Model = mat4(1.0); //Inizializzo con l'identità
		Scena[0].Model = translate(Scena[0].Model, vec3(player.posX, player.posY, 0.0));
		//Scena[0].Model = translate(Scena[0].Model, vec3(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2, 0.0)); 
		//Scommenta per testare la figura

		Scena[0].Model = scale(Scena[0].Model, vec3(5, 5, 1.0));
		//Scena[k].Model = rotate(Scena[0].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
		//printf("PLAYER POS : %f; %f\n", player.posX, player.posY);

		vec4 br = player.figura.BR_original; vec4 tl = player.figura.TL_original;
		vec4 bl = player.figura.BL_original; vec4 tr = player.figura.TR_original;
		br = Scena[0].Model * br; tl = Scena[0].Model * tl;

		player.figura.BR_model = br;
		player.figura.TL_model = tl;

		/*NEMICI*/
		//NEMICO 1
		Scena[1].Model = mat4(1.0); //Inizializzo con l'identità
		Scena[1].Model = translate(Scena[1].Model, vec3(nemico1.posX, nemico1.posY, 0.0));
		Scena[1].Model = scale(Scena[1].Model, vec3(5, 5, 1.0));

		br = nemico1.figura.BR_original;  tl = nemico1.figura.TL_original; 
		br = Scena[1].Model * br; tl = Scena[1].Model * tl; 

		Scena[1].Model = rotate(Scena[1].Model, radians(angolo * enemy_rotation_1), vec3(0.0, 0.0, 1.0));

		nemico1.figura.BR_model = br;
		nemico1.figura.TL_model = tl;

		/*printf("\n\nENEMY BB : %f, %f --- %f, %f\n", nemico1.figura.TL_model.x, nemico1.figura.TL_model.y, nemico1.figura.BR_model.x, nemico1.figura.BR_model.y);
		printf("         : %f, %f --- %f, %f\n", nemico1.figura.BL_model.x, nemico1.figura.BL_model.y, nemico1.figura.TR_model.x, nemico1.figura.TR_model.y);*/
		/*assestaRotazioneBoundingBox(&nemico1.figura);
		printf("ROTATED  : %f, %f --- %f, %f\n", nemico1.figura.TL_model.x, nemico1.figura.TL_model.y, nemico1.figura.BR_model.x, nemico1.figura.BR_model.y);
		printf("         : %f, %f --- %f, %f", nemico1.figura.BL_model.x, nemico1.figura.BL_model.y, nemico1.figura.TR_model.x, nemico1.figura.TR_model.y);*/
	

		//NEMICO 2
		Scena[2].Model = mat4(1.0); //Inizializzo con l'identità
		Scena[2].Model = translate(Scena[2].Model, vec3(nemico2.posX, nemico2.posY, 0.0));
		Scena[2].Model = scale(Scena[2].Model, vec3(6, 6, 1.0));

		br = nemico2.figura.BR_original;  tl = nemico2.figura.TL_original;
		br = Scena[2].Model * br; tl = Scena[2].Model * tl;

		Scena[2].Model = rotate(Scena[2].Model, radians(angolo*enemy_rotation_2), vec3(0.0, 0.0, 1.0));

		nemico2.figura.BR_model = br;
		nemico2.figura.TL_model = tl;
		//assestaRotazioneBoundingBox(&nemico2.figura);

		//NEMICO 3
		Scena[3].Model = mat4(1.0); //Inizializzo con l'identità
		Scena[3].Model = translate(Scena[3].Model, vec3(nemico3.posX, nemico3.posY, 0.0));
		Scena[3].Model = scale(Scena[3].Model, vec3(4, 6, 1.0));

		br = nemico3.figura.BR_original;  tl = nemico3.figura.TL_original;
		br = Scena[3].Model * br; tl = Scena[3].Model * tl;

		Scena[3].Model = rotate(Scena[3].Model, radians(angolo*enemy_rotation_3), vec3(0.0, 0.0, 1.0));

		nemico3.figura.BR_model = br;
		nemico3.figura.TL_model = tl;
		//assestaRotazioneBoundingBox(&nemico3.figura);

		//Devo passare allo shader le info su dove andare a prendere le informazioni sulle variabili uniformi
		glUniform1i(lsceltavs, Scena[0].sceltaVS);
		glUniform1i(lsceltafs, Scena[0].sceltaFS);
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
			glUniform1i(lsceltavs, Scena[k].sceltaVS);
			glUniform1i(lsceltafs, Scena[k].sceltaFS);
			glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[k].Model));
			glBindVertexArray(Scena[k].VAO);
			glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[k].nv - 6);
		}

		
		//Disegno Bounding Box
		if (drawBB == TRUE)
		{
			for (int j = 0; j <= 3; j++) {
				glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[j].Model));
				glBindVertexArray(Scena[1].VAO);
				glDrawArrays(GL_LINE_STRIP, Scena[1].nv - 6, 6);
				glBindVertexArray(0);

			}
		}

		if (checkCollision(player.figura, nemico1.figura) || checkCollision(player.figura, nemico2.figura)
			|| checkCollision(player.figura, nemico3.figura)) {
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
		RenderText(programId_text, Projection, vite_string, VAO_Text, VBO_Text, 10, WINDOW_HEIGHT - 50, 0.7f, colore_testo);
		RenderText(programId_text, Projection, score_string, VAO_Text, VBO_Text, 10, WINDOW_HEIGHT - 100, 0.7f, colore_testo);
	}
	
	else {
		RenderText(programId_text, Projection, gameover_string, VAO_Text, VBO_Text, 180, WINDOW_HEIGHT/2, 1.0f, colore_testo_alt);
		RenderText(programId_text, Projection, score_string, VAO_Text, VBO_Text, 240, WINDOW_HEIGHT/2 - 50, 0.7f,colore_testo);
	}
	
	
	glUseProgram(programId);
	glutSwapBuffers();

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
	glutSpecialFunc(specialKeyPressedEvent);
	glutSpecialUpFunc(specialKeyReleasedEvent);
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

	INIT_VAO_Text();
	Init_Freetype();

	buildOpenGLMenu();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();
}