/*Implementazione di Utils.h*/

#include "Utils.h"
#include "Definizioni.h"

extern Figura Asteroide1, Asteroide2, Asteroide3;
extern Entity player, nemico1, nemico2, nemico3;
extern vec2 player_default_pos, nemico1_default_pos, nemico2_default_pos, nemico3_default_pos;
extern int pval;

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

pair<vec4, vec4> calcolaBoundingBox(Figura* fig) {
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

	pair<vec4, vec4> pair = make_pair(vec4(topLeftCorner, 1.0), vec4(bottomRightCorner, 1.0));
	return pair;
}
/*
pair<float, float> calcolaDimensioniFigura(Figura* fig) {
	pair<float,float> pair = make_pair(fig->tl_original)
}*/

void inizializzaEntity() {
	player.posX = player_default_pos.x; player.posY = player_default_pos.y; player.speed = PLAYER_SPEED;

	nemico1.posX = nemico1_default_pos.x; nemico1.posY = nemico1_default_pos.y;
	nemico1.speed = ENEMY_SPEED_1;
	nemico1.dx = (float) nemico1.speed;

	nemico2.posX = nemico2_default_pos.x; nemico2.posY = nemico2_default_pos.y;
	nemico2.speed = ENEMY_SPEED_2;
	nemico2.dx = (float) nemico2.speed;
	nemico3.speed = ENEMY_SPEED_3;
	nemico3.dx = (float) nemico3.speed;


}

void resettaPosizionePlayer() {
	player.posX = player_default_pos.x;
	player.posY = player_default_pos.y;
	printf("Posizione resettata\n");
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

	pair<vec4, vec4> boundingBoxPair = calcolaBoundingBox(fig);
	fig->tl_original = boundingBoxPair.first;
	fig->br_original = boundingBoxPair.second;


}

void costruisci_antenne(vec4 col_primario, Figura* fig1, Figura* fig2) {

}
