// Progetto2D.cpp : Questo file contiene la funzione 'main', in cui inizia e termina l'esecuzione del programma.
//

#include <iostream>
#include "ShaderMaker.h"
#include "Lib.h"
#include "BoundingBox.h"
#include "Definizioni.h"
#include "GestioneEventi.h"


static unsigned int programId, MatModel, MatProj;
mat4 Projection;
float angolo = 0.0, s = 1, dx = 0, dy = 0, player_dx = 0, player_dy = 0;
int player_x = width / 2, player_y = height / 4;
bool isPaused = false;
Bounds boundingBox;

vector<Figura> Scena;

Figura  Farf = {};
Figura Cuore = {};
Figura Auto1 = {}, Auto2 = {};
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

//TODO: fare che ritorni un qualcosa di tipo boundingbox
//andrebbe anche passata attraverso le varie matrici di trasformazioni
BoundingBox costruisciBoundingBox(Figura* fig) {
	vec3 min = fig->vertici.at(0);
	vec3 max = fig->vertici.at(0);
	vec3 topLeftCorner, bottomRightCorner;
	for (vec3 vertice : fig->vertici) {
		if (min.x > vertice.x) min.x = vertice.x;
		if (min.y > vertice.y) min.y = vertice.y;
		if (max.x < vertice.x) max.x = vertice.x;
		if (max.y < vertice.y) max.y = vertice.y;

	}
	topLeftCorner.x = min.x;
	topLeftCorner.y = max.y;
	bottomRightCorner.x = max.x;
	bottomRightCorner.y = min.y;
	BoundingBox bb = {};
	bb.bottomRightCorner = bottomRightCorner;
	bb.topLeftCorner = topLeftCorner;
	printf("Bounding Box\nTOP LEFT: %f, %f\nBOT RIGHT: %f, %f\n", topLeftCorner.x, topLeftCorner.y, bottomRightCorner.x, bottomRightCorner.y);
	return bb;
}

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_S.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);
}

void INIT_VAO(void) {
	Cuore.nTriangles = 30;
	costruisci_cuore(0, 0, 1, 1, &Cuore);
	player.figura = Cuore;
	player.boundingBox = costruisciBoundingBox(&Cuore);
	printf("PLAYER Bounding Box\nTOP LEFT: %f, %f\nBOT RIGHT: %f, %f\n", player.boundingBox.topLeftCorner.x, player.boundingBox.topLeftCorner.y, player.boundingBox.bottomRightCorner.x, player.boundingBox.bottomRightCorner.y);

	crea_VAO_Vector(&player.figura);
	Scena.push_back(player.figura);

	Auto1.nTriangles = 30;
	costruisci_cuore(0, 0, 1, 1, &Auto1);
	crea_VAO_Vector(&Auto1);
	Scena.push_back(Auto1);

	Auto2.nTriangles = 30;
	costruisci_cuore(0, 0, 1, 1, &Auto2);
	crea_VAO_Vector(&Auto2);
	Scena.push_back(Auto2);

	Projection = ortho(0.0f, float(width), 0.0f, float(height));
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
			//Scena[k].Model = rotate(Scena[k].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
			printf("%f; %f\n", player.posX, player.posY);
			player.boundingBox.Model = mat4(1.0);
			player.boundingBox.Model = translate(Scena[k].Model, vec3(player.posX, player.posY, 0.0));
			player.boundingBox.Model = scale(Scena[k].Model, vec3(50, 50, 1.0));
			printf("%d; %d\n", player.boundingBox.topLeftCorner, player.boundingBox.bottomRightCorner);
			
		}

		if (k == 1) {
			Scena[k].Model = mat4(1.0); //Inizializzo con l'identità
			Scena[k].Model = translate(Scena[k].Model, vec3(200 + dx, height/6 + dy, 0.0));
			Scena[k].Model = scale(Scena[k].Model, vec3(50, 50, 1.0));
			Scena[k].Model = rotate(Scena[k].Model, radians(angolo), vec3(0.0, 0.0, 1.0));
		}

		if (k == 2) {
			Scena[k].Model = mat4(1.0); //Inizializzo con l'identità
			Scena[k].Model = translate(Scena[k].Model, vec3(600 + dx, height / 2 + dy, 0.0));
			Scena[k].Model = scale(Scena[k].Model, vec3(50 * s, 50 * s, 1.0));
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
			vec2 angleVec = vec2(width / 2, height / 2) - asteroid;
			angleVec = normalize(angleVec);
			asteroid = vec2((asteroid.x + angleVec.x * asteroidSpeed), (asteroid.y + angleVec.y * asteroidSpeed));
			if (((asteroid.x > width / 2 - width / 10 && asteroid.x < width / 2) || (asteroid.x < width / 2 + width / 10 && asteroid.x > width / 2)) &&
				((asteroid.y > height / 2 - height / 10 && asteroid.y < height / 2) || (asteroid.y < height / 2 + height / 10 && asteroid.y > height / 2))) {
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


int main(int argc, char* argv[])
{
	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Progetto 2D");
	glutDisplayFunc(drawScene);
	glutKeyboardFunc(myKeyboard);

	player.posX = player_x; player.posY = player_y; player.speed = PLAYER_SPEED;

	glutTimerFunc(50, updateAngolo, 0);
	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();
}