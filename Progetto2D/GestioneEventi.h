#pragma once
/*Varie funzioni per la gestione di eventi del gioco, come la risposta all'input da tastiera o il controllo delle collisioni*/
void keyboardPressedEvent(unsigned char key, int x, int y);

void keyboardReleasedEvent(unsigned char key, int x, int y);

void specialKeyPressedEvent(int key, int x, int y);

void specialKeyReleasedEvent(int key, int x, int y);

void reshape(int w, int h);

//Funzione checkCollision non pi? utilizzata
bool checkCollision(vec4 tl1, vec4 br1, vec4 tl2, vec4 br2);

//Funzione checkCollision in uso
bool checkCollision(Figura fig1, Figura fig2);

void mouseClick(int x, int y);

void update(int a);

void updateAsteroidi(int a);

void updateAngolo(int value);