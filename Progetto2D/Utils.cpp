/*Implementazione di Utils.h*/

#include "Utils.h"
#include "Definizioni.h"

extern Figura Asteroide1, Asteroide2, Asteroide3;
extern Entity player, nemico1, nemico2, nemico3;
extern vec2 player_default_pos, nemico1_default_pos, nemico2_default_pos, nemico3_default_pos;
extern int pval;
extern float player_dx, player_dy;

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

vector<vec4> calcolaBoundingBox(Figura* fig) {
	vec3 min = fig->vertici.at(0);
	vec3 max = fig->vertici.at(0);
	vec3 topLeftCorner, bottomRightCorner, topRightCorner, bottomLeftCorner;
	for (vec3 vertice : fig->vertici) {
		if (min.x > vertice.x) min.x = vertice.x;
		if (min.y > vertice.y) min.y = vertice.y;
		if (max.x < vertice.x) max.x = vertice.x;
		if (max.y < vertice.y) max.y = vertice.y;

	}
	//Angoli principali
	topLeftCorner.x = min.x;
	topLeftCorner.y = max.y;
	bottomRightCorner.x = max.x;
	bottomRightCorner.y = min.y;
	//Angoli secondari
	topRightCorner.x = max.x;
	topRightCorner.y = max.y;
	bottomLeftCorner.x = min.x;
	bottomLeftCorner.y = min.y;

	//Aggiungo i vertici della spezzata per costruire il bounding box
	fig->vertici.push_back(vec3(min.x, min.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(max.x, min.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(max.x, max.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(min.x, min.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(min.x, max.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->vertici.push_back(vec3(max.x, max.y, 0.0));
	fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	fig->nv = fig->vertici.size();

	//pair<vec4, vec4> pair = make_pair(vec4(topLeftCorner, 1.0), vec4(bottomRightCorner, 1.0));
	vector<vec4> boundingBox;
	boundingBox.push_back(vec4(topLeftCorner, 1.0));
	boundingBox.push_back(vec4(bottomRightCorner, 1.0));

	boundingBox.push_back(vec4(topRightCorner, 1.0));
	boundingBox.push_back(vec4(bottomLeftCorner, 1.0));
	/*printf("VECTOR\n");
	for (vec4 vertice : boundingBox) {
		printf("vert:\t%f, %f\n", vertice.x, vertice.y);
	}*/
	return boundingBox;
}

void assestaRotazioneBoundingBox(Figura* fig)
{
	vector<vec3> tmp { fig->TL_model, fig->BR_model, fig->TR_model, fig->BL_model };
	//printf("VECTOR\n");
	vec3 min = fig->TL_model;
	vec3 max = fig->TL_model;
	vec3 topLeftCorner, bottomRightCorner, topRightCorner, bottomLeftCorner;
	for (vec3 vertice : tmp) {
		//printf("vert:\t%f, %f\n", vertice.x, vertice.y);
		if (min.x > vertice.x) min.x = vertice.x;
		if (min.y > vertice.y) min.y = vertice.y;
		if (max.x < vertice.x) max.x = vertice.x;
		if (max.y < vertice.y) max.y = vertice.y;
	}
	//Angoli principali
	topLeftCorner.x = min.x;
	topLeftCorner.y = max.y;
	bottomRightCorner.x = max.x;
	bottomRightCorner.y = min.y;
	//Angoli secondari
	topRightCorner.x = max.x;
	topRightCorner.y = max.y;
	bottomLeftCorner.x = min.x;
	bottomLeftCorner.y = min.y;

	//Numero di vertici totali:
	int nv = fig->nv;
	//fig->vertici.resize(nv - 6);
	//Aggiungo i vertici della spezzata per costruire il bounding box
	fig->vertici[nv - 6] = (vec3(min.x, min.y, 0.0));
	fig->vertici[nv-5]=(vec3(max.x, min.y, 0.0));
	fig->vertici[nv-4]=(vec3(max.x, max.y, 0.0));
	fig->vertici[nv-3]=(vec3(min.x, min.y, 0.0));
	fig->vertici[nv-2]=(vec3(min.x, max.y, 0.0));
	fig->vertici[nv-1]=(vec3(max.x, max.y, 0.0));

	/*fig->vertici.push_back((vec3(min.x, min.y, 0.0)));
	fig->vertici.push_back((vec3(max.x, min.y, 0.0)));
	fig->vertici.push_back((vec3(max.x, max.y, 0.0)));
	fig->vertici.push_back((vec3(min.x, min.y, 0.0)));
	fig->vertici.push_back((vec3(min.x, max.y, 0.0)));
	fig->vertici.push_back((vec3(max.x, max.y, 0.0)));
	fig->nv = fig->vertici.size();*/


	fig->TL_model = vec4(topLeftCorner, 1.0);
	fig->BR_model = vec4(bottomRightCorner, 1.0);

	fig->TR_model = vec4(topRightCorner, 1.0);
	fig->BL_model = vec4(bottomLeftCorner, 1.0);
}

void inizializzaEntity() {
	player.posX = player_default_pos.x; player.posY = player_default_pos.y; player.speed = PLAYER_SPEED;

	nemico1.posX = nemico1_default_pos.x; nemico1.posY = nemico1_default_pos.y;
	nemico1.speed = ENEMY_SPEED_1;
	nemico1.dx = (float) nemico1.speed;

	nemico2.posX = nemico2_default_pos.x; nemico2.posY = nemico2_default_pos.y;
	nemico2.speed = ENEMY_SPEED_2;
	nemico2.dx = (float) nemico2.speed;

	nemico3.posX = nemico3_default_pos.x; nemico3.posY = nemico3_default_pos.y;
	nemico3.speed = ENEMY_SPEED_3;
	nemico3.dx = (float) nemico3.speed;


}


void costruisci_pod(vec4 col_primario, vec4 col_secondario, vec4 col_accenti, Figura* fig) {
	float* t;
	float stepA = (PI) / fig->nTriangles;

	/*fig->CP.push_back(vec3(0.0, -10.0, 0.0));
	fig->CP.push_back(vec3(7.0, -10.0, 0.0));*/
	fig->CP.push_back(vec3(10.0, -6.0, 0.0));
	fig->CP.push_back(vec3(0.0, 10.0, 0.0));
	fig->CP.push_back(vec3(-10.0, -6.0, 0.0));

	//fig->CP.push_back(vec3(-7.0, -10.0, 0.0));

	//fig->CP.push_back(vec3(0, -10, 0.0));

	t = new float[fig->CP.size()];
	int i;
	float step = 1.0 / (float)(fig->CP.size() - 1);

	for (i = 0; i < fig->CP.size(); i++)
		t[i] = i * step;


	InterpolazioneHermite(t, fig, col_primario, col_primario);
	//fig->vertici.push_back(vec3(0.0, -10.0, 0.0));
	//fig->vertici.push_back(vec3(0.0, -6.0, 0.0));

	//fig->vertici.push_back(vec3(-10.0, -6.0, 0.0));
	fig->vertici.at(0) = vec3(0.0, -6.0, 0.0);	//QUA CAMBIO IL CENTRO DELLA MESH DI HERMITE PER IL MIO SCOPO
	//fig->colors.push_back(vec4(1.0, 0.0, 0.0, 1.0));
	
	//Porta
	fig->vertici.push_back(vec3(2.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(2.0, -2.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-2.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-2.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-2.0, -2.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(2.0, -2.0, 0.0));
	fig->colors.push_back(col_secondario);

	//Parte inferiore della nave
	fig->vertici.push_back(vec3(10.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(7.0, -10.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-7.0, -10.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-7.0, -10.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-10.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(10.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);


	//Antenne
	fig->vertici.push_back(vec3(-7.0, 4.0, 0.0));
	fig->colors.push_back(col_accenti);
	fig->vertici.push_back(vec3(-8.0, 8.0, 0.0));
	fig->colors.push_back(col_accenti);
	fig->vertici.push_back(vec3(-6.0, 6.0, 0.0));
	fig->colors.push_back(col_accenti);

	fig->vertici.push_back(vec3(7.0, 4.0, 0.0));
	fig->colors.push_back(col_accenti);
	fig->vertici.push_back(vec3(8.0, 8.0, 0.0));
	fig->colors.push_back(col_accenti);
	fig->vertici.push_back(vec3(6.0, 6.0, 0.0));
	fig->colors.push_back(col_accenti);

	fig->nv = fig->vertici.size();

	vector<vec4> boundingBox = calcolaBoundingBox(fig);
	fig->TL_original = boundingBox[0];
	fig->BR_original = boundingBox[1];

	fig->TR_original = boundingBox[2];
	fig->BL_original = boundingBox[3];


}

void costruisci_pod_alt(vec4 col_primario, vec4 col_secondario, vec4 col_ter, vec4 col_accenti, Figura* fig) {
	float t;
	int triangles = fig->nTriangles;
	float stepA = (PI) / triangles;
	int i;

	//Le due antenne (TOT VERTICI = 9 + 9)
	fig->vertici.push_back(vec3(7.0, 8.0, 0.0));
	fig->vertici.push_back(vec3(9.0, 5.0, 0.0));
	fig->vertici.push_back(vec3(7.0, 6.0, 0.0));
	fig->vertici.push_back(vec3(9.0, 5.0, 0.0));
	fig->vertici.push_back(vec3(7.0, 6.0, 0.0));
	fig->vertici.push_back(vec3(6.0, 5.0, 0.0));
	fig->vertici.push_back(vec3(6.0, 5.0, 0.0));
	fig->vertici.push_back(vec3(6.0, 1.0, 0.0));
	fig->vertici.push_back(vec3(9.0, 5.0, 0.0));
	//Antenna 2
	fig->vertici.push_back(vec3(-7.0, 8.0, 0.0));
	fig->vertici.push_back(vec3(-9.0, 5.0, 0.0));
	fig->vertici.push_back(vec3(-7.0, 6.0, 0.0));
	fig->vertici.push_back(vec3(-9.0, 5.0, 0.0));
	fig->vertici.push_back(vec3(-7.0, 6.0, 0.0));
	fig->vertici.push_back(vec3(-6.0, 5.0, 0.0));
	fig->vertici.push_back(vec3(-6.0, 5.0, 0.0));
	fig->vertici.push_back(vec3(-6.0, 1.0, 0.0));
	fig->vertici.push_back(vec3(-9.0, 5.0, 0.0));

	for (i = 1; i <= 9*2; i++) {
		fig->colors.push_back(col_accenti);
	}

	//Cupola dell'astronave (TOT VERTICI 1 + NTRIANGLES)
	fig->vertici.push_back(vec3(0.0, 7.0, 0.0));	//Centro della semi-circonferenza

	fig->colors.push_back(col_primario);

	for (i = 0; i <= triangles; i++)
	{
		t = (float)i * stepA + PI;
		fig->vertici.push_back(vec3(0.0 - 5.0 * cos(t), 7.0 - 3.0 * sin(t), 0.0));
		//fig->vertici.push_back(vec3(radius * cos(i * twicePi / triangles), radius * sin(i * twicePi / triangles), 0.0));
		fig->colors.push_back(col_primario);


	}

	//"Corpo" dell'astronave, iniziando dal trapezio superiore (TOT VERTICI = 6 + 6)
	fig->vertici.push_back(vec3(-9.0, 0.0, 0.0));
	fig->colors.push_back(col_primario);
	fig->vertici.push_back(vec3(-5.0, 7.0, 0.0));
	fig->colors.push_back(col_primario);
	fig->vertici.push_back(vec3(5.0, 7.0, 0.0));
	fig->colors.push_back(col_primario);
	fig->vertici.push_back(vec3(5.0, 7.0, 0.0));
	fig->colors.push_back(col_primario);
	fig->vertici.push_back(vec3(9.0, 0.0, 0.0));
	fig->colors.push_back(col_primario);
	fig->vertici.push_back(vec3(-9.0, 0.0, 0.0));
	fig->colors.push_back(col_primario);

	fig->vertici.push_back(vec3(-10.0, -6.0, 0.0));
	fig->colors.push_back(col_ter);
	fig->vertici.push_back(vec3(-9.0, 0.0, 0.0));
	fig->colors.push_back(col_primario);
	fig->vertici.push_back(vec3(9.0, 0.0, 0.0));	
	fig->colors.push_back(col_primario);
	fig->vertici.push_back(vec3(9.0, 0.0, 0.0));
	fig->colors.push_back(col_primario);
	fig->vertici.push_back(vec3(10.0, -6.0, 0.0));
	fig->colors.push_back(col_ter);
	fig->vertici.push_back(vec3(-10.0, -6.0, 0.0));
	fig->colors.push_back(col_ter);

	//Porta (TOT VERTICI = 6)
	fig->vertici.push_back(vec3(3.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(3.0, -2.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-3.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-3.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-3.0, -2.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(3.0, -2.0, 0.0));
	fig->colors.push_back(col_secondario);

	//Parte inferiore della nave (TOT VERTICI = 6)
	fig->vertici.push_back(vec3(10.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(7.0, -10.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-7.0, -10.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-7.0, -10.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(-10.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);
	fig->vertici.push_back(vec3(10.0, -6.0, 0.0));
	fig->colors.push_back(col_secondario);

	fig->nv = fig->vertici.size();

	vector<vec4> boundingBox = calcolaBoundingBox(fig);
	fig->TL_original = boundingBox[0];
	fig->BR_original = boundingBox[1];

	fig->TR_original = boundingBox[2];
	fig->BL_original = boundingBox[3];


}

void costruisci_fondale(vec4 col_top, vec4 col_bottom, Figura* fig)
{
	fig->vertici.push_back(vec3(0.0, 0.0, 0.0));
	fig->vertici.push_back(vec3(1.0, 0.0, 0.0));
	fig->vertici.push_back(vec3(0.0, 1.0, 0.0));
	fig->vertici.push_back(vec3(1.0, 1.0, 0.0));
	fig->colors.push_back(col_bottom);
	fig->colors.push_back(col_top);
	fig->colors.push_back(col_top);
	fig->colors.push_back(col_top);
	fig->nv = fig->vertici.size();

	//Costruzione matrice di Modellazione Sole, che rimane la stessa(non si aggiorna)
	fig->Model = mat4(1.0);
	fig->Model = translate(fig->Model, vec3(0.0, 0.0, 0.0));
	fig->Model = scale(fig->Model, vec3(WINDOW_WIDTH, WINDOW_HEIGHT, 1.0));
}

void costruisci_asteroide(vec4 color_top, vec4 color_bot, Figura* forma) {
	float* t;

	//forma->CP.push_back(vec3(0.0, 0.0, 0.0));
	forma->CP.push_back(vec3(-10, 0, 0.0));
	forma->CP.push_back(vec3(-9.5, 3, 0.0));
	forma->CP.push_back(vec3(-7, 6, 0.0));
	forma->CP.push_back(vec3(-6, 9, 0.0));
	forma->CP.push_back(vec3(-4, 10, 0.0));
	forma->CP.push_back(vec3(-2, 9, 0.0));
	forma->CP.push_back(vec3(0, 7, 0.0));
	forma->CP.push_back(vec3(2, 6, 0.0));

	forma->CP.push_back(vec3(5, 7, 0.0));	//Questo punto sembra problematico
	forma->CP.push_back(vec3(9, 4, 0.0));
	forma->CP.push_back(vec3(10, 2, 0.0));
	forma->CP.push_back(vec3(8, -1, 0.0));
	forma->CP.push_back(vec3(6.5, -2.5, 0.0));
	forma->CP.push_back(vec3(7, -7, 0.0));
	forma->CP.push_back(vec3(3, -10, 0.0));
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

	vector<vec4> boundingBox = calcolaBoundingBox(forma);
	forma->TL_original = boundingBox[0];
	forma->BR_original = boundingBox[1];

	forma->TR_original = boundingBox[2];
	forma->BL_original = boundingBox[3];

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

void resettaPosizionePlayer()
{
	player.posX = player_default_pos.x;
	player.posY = player_default_pos.y;
	printf("Posizione resettata\n");
}