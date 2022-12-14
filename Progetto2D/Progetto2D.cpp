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

static unsigned int programId, programId_text, MatModel, MatProj;
unsigned int VAO_Text, VBO_Text;

mat4 Projection;
float angolo = 0.0, s = 1, player_dx = 0, player_dy = 0, dy_asteroide=0, attrito = 0.3;
float enemy_rotation_1 = 0.5, enemy_rotation_2 = -0.4, enemy_rotation_3 = 0.7;
vec2 player_default_pos = { WINDOW_WIDTH / 2, WINDOW_HEIGHT };
vec2 nemico1_default_pos = { 100.0, WINDOW_HEIGHT / 1.5 };
vec2 nemico2_default_pos = { 600.0 , WINDOW_HEIGHT/ 2.8};
vec2 nemico3_default_pos = {27.0, WINDOW_HEIGHT / 15};

int drift_orizzontale = 3, gravity = 3;
int vite = MAX_VITE; int score = 0;
int pval = 140;
int width = WINDOW_WIDTH, height = WINDOW_HEIGHT;
float w_update, h_update;
string vite_string, score_string, gameover_string = "GAME OVER";
bool isPaused = false, gameOver = false, drawBB = false;
//Booleani posti a true se si usa il tasto a (spostamento a sinistra) e b (spostamento a destra)
bool pressing_left = false;
bool pressing_right = false;
vec4 asteroide_col_bot = { 0.181, 0.181, 0.185, 1.0 }, asteroide_col_top = { 0.076, 0.076, 0.084, 1.0 };
vec4 pod_col_primario = vec4(0.80, 0.82, 0.84, 1.0), pod_col_secondario = vec4(0.09, 0.10, 0.10, 1.0), pod_col_accenti = vec4(0.91, 0.58, 0.11, 1.0), pod_col_ter = vec4(0.08, 0.31, 0.58, 1.0);
vec4 fondale_col_top = vec4(0.02, 0.13, 0.29, 1.0), fondale_col_bottom = vec4(0.19, 0.03, 0.11, 1.0), fondale_col_mid = vec4(0.32, 0.15, 0.46, 1.0), fondale_col_alt = vec4(0.21, 0.16, 0.29, 1.0);
vec4 colore_testo = vec4(0.82, 0.61, 0.18, 1.0), colore_testo_alt = vec4(0.80, 0.14, 0.05, 1.0);
unsigned int locres, locmouse, loctime, lsceltavs, lsceltafs;
vec2 mouse, res;

vector<Figura> Scena;

Figura Pod = {}, Fondale = {}, Asteroide = {}, Asteroide1 = {}, Asteroide2 = {}, Asteroide3 = {};
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

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_M_2.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	//Generazione del program shader per la gestione del testo
	vertexShader = (char*)"VertexShader_Text.glsl";
	fragmentShader = (char*)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);

}

void INIT_VAO(void) {
	Pod.sceltaFS = 0;
	Pod.sceltaVS = 0;
	Pod.nTriangles = 50;
	costruisci_pod(pod_col_primario, pod_col_secondario, pod_col_ter, pod_col_accenti, &Pod);
	player.figura = Pod;
	crea_VAO_Vector(&player.figura);
	Scena.push_back(player.figura);

	//Gli asteroidi che fanno da "nemico"
	Asteroide1.sceltaFS = 0;
	Asteroide1.sceltaVS = 0;
	costruisci_asteroide(asteroide_col_top, asteroide_col_bot, &Asteroide1);
	nemico1.figura = Asteroide1;
	crea_VAO_Vector(&Asteroide1);
	Scena.push_back(Asteroide1);

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
	costruisci_fondale(fondale_col_top, fondale_col_bottom, fondale_col_mid, fondale_col_alt, &Fondale);
	crea_VAO_Vector(&Fondale);
	Scena.push_back(Fondale);

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
	res.x = (float)width;
	res.y = (float)height;
	float time = glutGet(GLUT_ELAPSED_TIME) / 1000.0f;

	glUniform1f(loctime, time);
	
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection)); //Questa pu? andare fuori dal ciclo xk nn cambia
	glUniform2f(locres, res.x, res.y);
	glUniform2f(locmouse, mouse.x, mouse.y);
	//FONDALE, prima disegno il fondale perch? deve stare dietro alla scena
	glUniform1i(lsceltavs, Scena[4].sceltaVS);
	glUniform1i(lsceltafs, Scena[4].sceltaFS);
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Scena[4].Model));
	glBindVertexArray(Scena[4].VAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, Scena[4].nv);
	//glUseProgram(programId);
	if (!gameOver)
	{
		//PLAYER
		Scena[0].Model = mat4(1.0); //Inizializzo con l'identit?
		Scena[0].Model = translate(Scena[0].Model, vec3(player.posX, player.posY, 0.0));
		Scena[0].Model = scale(Scena[0].Model, vec3(5, 5, 1.0));

		vec4 br = player.figura.BR_original; vec4 tl = player.figura.TL_original;
		vec4 bl = player.figura.BL_original; vec4 tr = player.figura.TR_original;
		br = Scena[0].Model * br; tl = Scena[0].Model * tl;

		player.figura.BR_model = br;
		player.figura.TL_model = tl;

		/*NEMICI*/
		//NEMICO 1
		Scena[1].Model = mat4(1.0); //Inizializzo con l'identit?
		Scena[1].Model = translate(Scena[1].Model, vec3(nemico1.posX, nemico1.posY + 5*sin(dy_asteroide), 0.0));
		Scena[1].Model = scale(Scena[1].Model, vec3(5, 5, 1.0));

		br = nemico1.figura.BR_original;  tl = nemico1.figura.TL_original; 
		br = Scena[1].Model * br; tl = Scena[1].Model * tl; 

		Scena[1].Model = rotate(Scena[1].Model, radians(angolo * enemy_rotation_1), vec3(0.0, 0.0, 1.0));

		nemico1.figura.BR_model = br;
		nemico1.figura.TL_model = tl;

		//NEMICO 2
		Scena[2].Model = mat4(1.0); //Inizializzo con l'identit?
		Scena[2].Model = translate(Scena[2].Model, vec3(nemico2.posX, nemico2.posY + 4 * cos(dy_asteroide), 0.0));
		Scena[2].Model = scale(Scena[2].Model, vec3(6, 6, 1.0));

		br = nemico2.figura.BR_original;  tl = nemico2.figura.TL_original;
		br = Scena[2].Model * br; tl = Scena[2].Model * tl;

		Scena[2].Model = rotate(Scena[2].Model, radians(angolo*enemy_rotation_2), vec3(0.0, 0.0, 1.0));

		nemico2.figura.BR_model = br;
		nemico2.figura.TL_model = tl;
		//assestaRotazioneBoundingBox(&nemico2.figura);

		//NEMICO 3
		Scena[3].Model = mat4(1.0); //Inizializzo con l'identit?
		Scena[3].Model = translate(Scena[3].Model, vec3(nemico3.posX, nemico3.posY + 7 * cos(dy_asteroide), 0.0));
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

		/* VECCHIO MODO DI COSTRUIRE IL POD
		glDrawArrays(GL_TRIANGLE_FAN, 0, Scena[0].nv - 24);
		glDrawArrays(GL_TRIANGLES, Scena[0].nv - 24, 12);
		glDrawArrays(GL_TRIANGLES, Scena[0].nv - 12, 3);
		glDrawArrays(GL_TRIANGLES, Scena[0].nv - 9, 3);*/
		//NUOVO MODO DI COSTRUIRE IL POD DI ATTERRAGGIO
		glDrawArrays(GL_TRIANGLES, 0,9);
		glDrawArrays(GL_TRIANGLES,9,9);
		glDrawArrays(GL_TRIANGLE_FAN, 18, Scena[0].nv -30);

		glDrawArrays(GL_TRIANGLES, Scena[0].nv - 30, 24);

		//DISEGNO I 3 ASTEROIDI
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
	glutReshapeFunc(reshape);

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