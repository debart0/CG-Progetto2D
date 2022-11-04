#pragma once
#include "Lib.h"
#include "Definizioni.h"

//Calcola la boundingBox di una data figura (restituisce un pair con il topLeftCorner e bottomRightCorner)
pair<vec4, vec4> calcolaBoundingBox(Figura* fig);

//Inizializza una entity associandole una figura e variabili per velocità e posizione
void inizializzaEntity();

//Resetta la posizione di player a quella di default
void resettaPosizionePlayer();