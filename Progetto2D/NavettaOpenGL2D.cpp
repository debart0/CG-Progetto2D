//

#include <iostream>
#include "ShaderMaker.h"
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <math.h>
#include "GestioneTesto.h"
#include <ft2build.h>
#include FT_FREETYPE_H
static unsigned int programId, programId_text, MatrixProj, MatrixProj_txt, MatView;
unsigned int VAO_Text, VBO_Text;
#define  PI   3.14159265358979323846

int idfg, idfi;
int w_up = 350; 
int h_up = 350;
mat4 Projection_text;

int width = 1280;
int height = 720;
mat4 Projection;
GLuint MatProj, MatModel;
float angolo = 0.0;
float projectileAngolo = 0.0;
float ang = 0, dy = 0;
int pval = 140;
int punteggio = 0;
float lastFrame;
bool asteroidMoving = true;
bool projectileMoving = false;
bool paused = false, ded = false;
float asteroidSpeed = 0.7;
float projectileSpeed = 7.0;
vec2 asteroid = { 0.0, 0.0 };
vec2 projectile = { 2000.0, 1000.0 };

typedef struct {
	GLuint VAO;
	GLuint VBO_G;
	GLuint VBO_C;
	int nTriangles;
	int radius;
	// Vertici
	vector<vec3> vertici;
	vector<vec3> CP;
	vector<vec4> colors;
	// Numero vertici
	int nv;
	//Matrice di Modellazione: Traslazione*Rotazione*Scala
	mat4 Model;
} Figura;

Figura Navetta, Asteroid, Projectile;

bool CheckCollision() {
	bool collisionX = (projectile.x + Projectile.radius * 10 >= asteroid.x && projectile.x + Projectile.radius * 10 <= asteroid.x + Asteroid.radius * 20) ||
		(projectile.x - Projectile.radius * 10 <= asteroid.x && projectile.x - Projectile.radius * 10 >= asteroid.x - Asteroid.radius * 20);
	bool collisionY = (projectile.y + Projectile.radius * 7 >= asteroid.y && projectile.y + Projectile.radius * 7 <= asteroid.y + Asteroid.radius * 15) ||
		(projectile.y - Projectile.radius * 7 <= asteroid.y && projectile.y - Projectile.radius * 7 >= asteroid.y - Asteroid.radius * 15);
	return collisionX && collisionY;
}

void update_projectile(int a)
{
	if (!paused && !ded) {
		if (projectileMoving) {
			projectile = vec2(projectile.x - sin(projectileAngolo) * projectileSpeed, projectile.y + cos(projectileAngolo) * projectileSpeed);
			glutTimerFunc(24, update_projectile, 0);
			if (CheckCollision()) {
				punteggio++;
				asteroidSpeed += 0.05;
				asteroidMoving = false;
				projectileMoving = false;
			}
			if (projectile.x < 0 || projectile.x > 1280 || projectile.y < 0 || projectile.y > 720) {
				projectile = vec2(2000.0, 1000.0);
				projectileMoving = false;
			}
		}
		else {
			projectile = vec2(2000.0, 1000.0);
		}
	}
	else {
		glutTimerFunc(24, update_projectile, 0);
	}
	glutSetWindow(idfg);
	glutPostRedisplay();
	glutSetWindow(idfi);
	glutPostRedisplay();
}

void update_asteroid(int a)
{
	if (!paused && !ded) {
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
			int axisRand = linearRand(0, 3);
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
	glutSetWindow(idfg);
	glutPostRedisplay();
	glutSetWindow(idfi);
	glutPostRedisplay();
	glutTimerFunc(24, update_asteroid, 0);
}

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

//Per Curve di hermite
#define PHI0(t)  (2.0*t*t*t-3.0*t*t+1)
#define PHI1(t)  (t*t*t-2.0*t*t+t)
#define PSI0(t)  (-2.0*t*t*t+3.0*t*t)
#define PSI1(t)  (t*t*t-t*t)
float dix(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig)
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
float diy(int i, float* t, float Tens, float Bias, float Cont, Figura* Fig)
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
	Fig->colors.push_back(vec4(1.0, 1.0, 0.0, 1.0));




	for (tg = 0; tg <= 1; tg += passotg)
	{
		if (tg > t[is + 1]) is++;

		ampiezza = (t[is + 1] - t[is]);
		tgmapp = (tg - t[is]) / ampiezza;

		x = Fig->CP[is].x * PHI0(tgmapp) + dix(is, t, p_t, p_b, p_c, Fig) * PHI1(tgmapp) * ampiezza + Fig->CP[is + 1].x * PSI0(tgmapp) + dix(is + 1, t, p_t, p_b, p_c, Fig) * PSI1(tgmapp) * ampiezza;
		y = Fig->CP[is].y * PHI0(tgmapp) + diy(is, t, p_t, p_b, p_c, Fig) * PHI1(tgmapp) * ampiezza + Fig->CP[is + 1].y * PSI0(tgmapp) + diy(is + 1, t, p_t, p_b, p_c, Fig) * PSI1(tgmapp) * ampiezza;
		Fig->vertici.push_back(vec3(x, y, 0.0));
		Fig->colors.push_back(color_top);
	}

}

void costruisci_forma(vec4 color_top, vec4 color_bot, Figura* forma)
{


	float* t;
	forma->CP.push_back(vec3(0.0, 0.0, 0.0));
	forma->CP.push_back(vec3(-2.0, 0.0, 0.0));
	forma->CP.push_back(vec3(-2.0, 3.0, 0.0));
	forma->CP.push_back(vec3(-2.0, 5.0, 0.0));
	forma->CP.push_back(vec3(-6.0, 6.0, 0.0));
	forma->CP.push_back(vec3(-5.0, 8.0, 0.0));
	forma->CP.push_back(vec3(0.0, 14.0, 0.0));
	forma->CP.push_back(vec3(5.0, 8.0, 0.0));
	forma->CP.push_back(vec3(6.0, 6.0, 0.0));
	forma->CP.push_back(vec3(2.0, 5.0, 0.0));
	forma->CP.push_back(vec3(2.0, 3.0, 0.0));
	forma->CP.push_back(vec3(2.0, 0.0, 0.0));
	forma->CP.push_back(vec3(0.0, 0.0, 0.0));


	t = new float[forma->CP.size()];
	int i;
	float step = 1.0 / (float)(forma->CP.size() - 1);

	for (i = 0; i < forma->CP.size(); i++)
		t[i] = i * step;


	InterpolazioneHermite(t, &Navetta, color_top, color_bot);
	forma->vertici.push_back(vec3(0.0, 0.0, 0.0));
	forma->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	forma->nv = forma->vertici.size();

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

void INIT_SHADER(void)
{
	GLenum ErrorCheckValue = glGetError();

	char* vertexShader = (char*)"vertexShader_M.glsl";
	char* fragmentShader = (char*)"fragmentShader_M.glsl";

	programId = ShaderMaker::createProgram(vertexShader, fragmentShader);
	glUseProgram(programId);

	//Generazione del program shader per la gestione del testo
	vertexShader = (char*)"VertexShader_Text.glsl";
	fragmentShader = (char*)"FragmentShader_Text.glsl";

	programId_text = ShaderMaker::createProgram(vertexShader, fragmentShader);
}

void INIT_VAO(void)
{
	vec4 col_top = vec4{ 0.5203, 0.6054, 0.6093, 1.0000 };
	vec4 col_bottom = vec4{ 0.5625,0.8750, 0.9218, 1.0000 };
	costruisci_forma(col_top, col_bottom, &Navetta);
	crea_VAO_Vector(&Navetta);

	Asteroid.nTriangles = 20;
	Asteroid.radius = 4;
	col_top = vec4{ 0.1, 0.1, 0.1, 1.0000 };
	col_bottom = vec4{ 0.5, 0.5, 0.5, 0.8 };
	costruisci_cerchio(col_top, col_bottom, &Asteroid);
	crea_VAO_Vector(&Asteroid);

	Projectile.nTriangles = 15;
	Projectile.radius = 1;
	col_top = vec4{ 0.1, 0.9, 0.1, 1.0000 };
	col_bottom = vec4{ 0.1, 0.5, 0.1, 0.8 };
	costruisci_cerchio(col_top, col_bottom, &Projectile);
	crea_VAO_Vector(&Projectile);

	Projection = ortho(0.0f, float(width), 0.0f, float(height));
	MatProj = glGetUniformLocation(programId, "Projection");
	MatModel = glGetUniformLocation(programId, "Model");


	glClearColor(0.0, 0.0, 0.0, 1.0);


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


	glClear(GL_COLOR_BUFFER_BIT);

	glUniformMatrix4fv(MatProj, 1, GL_FALSE, value_ptr(Projection));

	glBindVertexArray(Navetta.VAO);
	Navetta.Model = mat4(1.0);
	Navetta.Model = translate(Navetta.Model, vec3(float(width) * 0.5, float(height) * 0.5, 0.0));
	Navetta.Model = scale(Navetta.Model, vec3(10.0, 10.0, 1.0));
	Navetta.Model = rotate(Navetta.Model, radians(angolo + ang), vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Navetta.Model));

	glDrawArrays(GL_TRIANGLE_FAN, 0, Navetta.nv);

	glBindVertexArray(Asteroid.VAO);
	Asteroid.Model = mat4(1.0);
	Asteroid.Model = translate(Asteroid.Model, vec3(asteroid.x, asteroid.y, 0.0));
	Asteroid.Model = scale(Asteroid.Model, vec3(20.0, 15.0, 1.0));
	Asteroid.Model = rotate(Asteroid.Model, radians(angolo), vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Asteroid.Model));

	glDrawArrays(GL_TRIANGLE_FAN, 0, Asteroid.nv);

	glBindVertexArray(Projectile.VAO);
	Projectile.Model = mat4(1.0);
	Projectile.Model = translate(Projectile.Model, vec3(projectile.x, projectile.y, 0.0));
	Projectile.Model = scale(Projectile.Model, vec3(10.0, 7.5, 1.0));
	Projectile.Model = rotate(Projectile.Model, radians(angolo + ang), vec3(0.0f, 0.0f, 1.0f));
	glUniformMatrix4fv(MatModel, 1, GL_FALSE, value_ptr(Projectile.Model));

	glDrawArrays(GL_TRIANGLE_FAN, 0, Projectile.nv);


	glutSwapBuffers();

}

void drawScene1(void)
{
	//In questa finestra vengono visualizzate le informazioni
	glClearColor(0.0, 1.0, 0.0, 0.0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Projection_text = ortho(0.0f, (float)w_up, 0.0f, (float)h_up);
	string punteggioText = "Punteggio: " + std::to_string(punteggio);

	RenderText(programId_text, Projection_text, punteggioText, VAO_Text, VBO_Text, 10.0f, 300.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	RenderText(programId_text, Projection_text, "A: Ruota antiorario", VAO_Text, VBO_Text, 10.0f, 260.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	RenderText(programId_text, Projection_text, "D: Ruota orario", VAO_Text, VBO_Text, 10.0f, 220.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	RenderText(programId_text, Projection_text, "R: Ruota 180 gradi", VAO_Text, VBO_Text, 10.0f, 180.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	RenderText(programId_text, Projection_text, "Spazio: Spara", VAO_Text, VBO_Text, 10.0f, 140.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));

	if (paused && !ded) {
		RenderText(programId_text, Projection_text, "Gioco in pausa", VAO_Text, VBO_Text, 10.0f, 100.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
		RenderText(programId_text, Projection_text, "Premi esc per uscire", VAO_Text, VBO_Text, 10.0f, 60.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
		RenderText(programId_text, Projection_text, "Spazio per riprendere", VAO_Text, VBO_Text, 10.0f, 20.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	}

	if (!paused && !ded) {
		RenderText(programId_text, Projection_text, "Gioco in movimento", VAO_Text, VBO_Text, 10.0f, 100.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
		RenderText(programId_text, Projection_text, "Premi esc per mettere in pausa", VAO_Text, VBO_Text, 10.0f, 60.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	}

	if (ded) {
		RenderText(programId_text, Projection_text, "Partita finita", VAO_Text, VBO_Text, 10.0f, 100.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
		punteggioText = "Punteggio finale: " + std::to_string(punteggio);
		RenderText(programId_text, Projection_text, punteggioText, VAO_Text, VBO_Text, 10.0f, 60.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
		RenderText(programId_text, Projection_text, "Premi esc per uscire", VAO_Text, VBO_Text, 10.0f, 20.0f, 0.5f, glm::vec3(1.0, 0.0f, 0.2f));
	}

	glutSwapBuffers();

}

void keyboardPressedEvent(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'd':
		if (!paused && !ded) {
			ang -= 5;
		}
		break;

	case 'a':
		if (!paused && !ded) {
			ang += 5;
		}
		break;

	case 'r':
		if (!paused && !ded) {
			ang += 180;
		}
		break;

	case 32:
		if (!paused && !ded) {
			if (!projectileMoving) {
				projectileAngolo = angolo + ang;
				projectileAngolo = radians(projectileAngolo);
				projectile = vec2(width / 2 - sin(projectileAngolo) * 100, height / 2 + cos(projectileAngolo) * 100);
				projectileMoving = true;
				glutTimerFunc(24, update_projectile, 0);
			}
		}
		else {
			paused = false;
		}
		break;

	case 27:
		if (!paused && !ded) {
			paused = true;
		}
		else {
			exit(0);
		}
		break;

	default:
		break;
	}
	glutSetWindow(idfi);
	glutPostRedisplay();
	glutSetWindow(idfg);
	glutPostRedisplay();
}



int main(int argc, char* argv[])
{
	srand(time(NULL));

	glutInit(&argc, argv);

	glutInitContextVersion(4, 0);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);

	glutInitWindowSize(800, 800);
	glutInitWindowPosition(100, 100);
	idfg = glutCreateWindow("Navicella Spaziale");
	glutDisplayFunc(drawScene);
	glutTimerFunc(66, update_asteroid, 0);
	glutKeyboardFunc(keyboardPressedEvent);

	glewExperimental = GL_TRUE;
	glewInit();
	INIT_SHADER();
	INIT_VAO();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutInitWindowSize(h_up, w_up);
	glutInitWindowPosition(500, 350);
	idfi = glutCreateWindow("Punteggio");
	glutDisplayFunc(drawScene1);
	INIT_SHADER();
	INIT_VAO_Text();
	Init_Freetype();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glutMainLoop();
}