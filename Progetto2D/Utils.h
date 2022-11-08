#pragma once
#include "Lib.h"
#include "Definizioni.h"

void costruisci_pod(vec4 col_primario, vec4 col_secondario, vec4 col_accenti, Figura* fig);

void costruisci_antenne(vec4 col_primario, Figura* fig1, Figura* fig2);

void costruisci_fondale(vec4 col_top, vec4 col_bottom, Figura* fig);

//Funzione per l'interpolazione delle curve di Hermite
void InterpolazioneHermite(float* t, Figura* Fig, vec4 color_top, vec4 color_bot);

//Calcola la boundingBox di una data figura (restituisce un pair con il topLeftCorner e bottomRightCorner)
pair<vec4, vec4> calcolaBoundingBox(Figura* fig);

//Inizializza una entity associandole una figura e variabili per velocità e posizione
void inizializzaEntity();

//Resetta la posizione di player a quella di default
void resettaPosizionePlayer();