#pragma once
/*Varie funzioni per la gestione di eventi del gioco, come la risposta all'input da tastiera o il controllo delle collisioni*/
void myKeyboard(unsigned char key, int x, int y);

bool checkCollision(BoundingBox boundingBox1, BoundingBox boundingBox2);
