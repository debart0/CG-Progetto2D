#pragma once
#include "Lib.h"
#include "Definizioni.h"

void costruisci_pod_deprecated(vec4 col_primario, vec4 col_secondario, vec4 col_accenti, Figura* fig);

void costruisci_pod(vec4 col_primario, vec4 col_secondario, vec4 col_ter, vec4 col_accenti, Figura* fig);

void costruisci_fondale(vec4 col_top, vec4 col_bottom, vec4 col_mid, vec4 col_alt, Figura* fig);

void costruisci_asteroide(vec4 color_top, vec4 color_bot, Figura* forma);

void costruisci_cerchio(vec4 color_center, vec4 color_edges, Figura* fig);


//Funzione per l'interpolazione delle curve di Hermite
void InterpolazioneHermite(float* t, Figura* Fig, vec4 color_top, vec4 color_bot);

//Calcola la boundingBox di una data figura
//Restituisce un vector con questo ordine: Top Left Corner, Bottom Right Corner, Top Right Corner e Bottom Left Corner
vector<vec4> calcolaBoundingBox(Figura* fig);

void assestaRotazioneBoundingBox(Figura* fig);

//Inizializza una entity associandole una figura e variabili per velocità e posizione
void inizializzaEntity();

//Resetta la posizione di player a quella di default
void resettaPosizionePlayer();